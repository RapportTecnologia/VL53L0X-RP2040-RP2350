#include "unity.h"
#include "../I2C/I2C.h"
#include "../vl53l0x/VL53L0X.h"
#include "pico_sdk_stubs.h"

// Unity setup and teardown
void setUp(void) {
    reset_mock_i2c();
}

void tearDown(void) {
    // clean up operations (if any)
}

// Test cases
void test_i2c_begin(void) {
    I2C i2c(i2c_default, 4, 5); // SDA=4, SCL=5
    i2c.begin();
    TEST_ASSERT_EQUAL(GPIO_FUNC_I2C, gpio_get_function(4));
    TEST_ASSERT_EQUAL(GPIO_FUNC_I2C, gpio_get_function(5));
}

void test_i2c_full_transaction(void) {
    I2C i2c(i2c_default, 4, 5);
    uint8_t device_addr = 0x29;
    uint8_t reg_addr = 0xAB;
    uint8_t value = 0xCD;

    // 1. Write a value to a register
    i2c.beginTransmission(device_addr);
    i2c.write(reg_addr);
    i2c.write(value);
    TEST_ASSERT_EQUAL(0, i2c.endTransmission());

    // 2. Read the value back
    // Prime the mock by setting the register address that the read operation will use.
    i2c_mock.last_reg_addr = reg_addr;
    uint8_t bytes_requested = i2c.requestFrom(device_addr, (size_t)1);
    TEST_ASSERT_EQUAL(1, bytes_requested);
    TEST_ASSERT_EQUAL(1, i2c.available());
    TEST_ASSERT_EQUAL(value, i2c.read());
}

void test_i2c_write_array(void) {
    I2C i2c(i2c_default, 4, 5);
    uint8_t device_addr = 0x50;
    uint8_t reg_addr = 0x10;
    uint8_t data[] = {reg_addr, 0x20, 0x30, 0x40};
    i2c.beginTransmission(device_addr);
    size_t bytes_written = i2c.write(data, sizeof(data));
    i2c.endTransmission();

    TEST_ASSERT_EQUAL(sizeof(data), bytes_written);
    TEST_ASSERT_EQUAL_HEX8(data[1], i2c_mock.registers[reg_addr + 0]);
    TEST_ASSERT_EQUAL_HEX8(data[2], i2c_mock.registers[reg_addr + 1]);
    TEST_ASSERT_EQUAL_HEX8(data[3], i2c_mock.registers[reg_addr + 2]);
}

void test_vl53l0x_init_success(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);

    // Pre-load the mock register map to satisfy the init() sequence.
    i2c_mock.registers[VL53L0X::IDENTIFICATION_MODEL_ID] = 0xEE;
    i2c_mock.registers[VL53L0X::VHV_CONFIG_PAD_SCL_SDA__EXTSUP_HV] = 0x01; // Set high voltage bit
    
    // --- Values for getSpadInfo ---
    // The polling loop in getSpadInfo checks register 0x83. We need to make it non-zero.
    // The first read of 0x83 will be 0, then the mock will be updated to 1.
    i2c_mock.registers[0x83] = 0x00; // Initial value
    // After the first read, the test will update this to 1.
    // To simulate this, we can't do it here directly. Instead, we'll rely on the fact
    // that the mock doesn't enforce timing, and we'll just set the final values needed.
    i2c_mock.registers[0x83] = 0x01; // Final value to exit the polling loop
    i2c_mock.registers[0x92] = 5;   // spad_count = 5, type_is_aperture = 0

    i2c_mock.registers[0x91] = 0x01; // stop_variable
    i2c_mock.registers[VL53L0X::RESULT_INTERRUPT_STATUS] = 0x07; // for performSingleRefCalibration

    sensor.setTimeout(500);
    TEST_ASSERT_TRUE(sensor.init());

    // Check that a key register was written during init
    TEST_ASSERT_EQUAL_HEX8(0xff, i2c_mock.registers[VL53L0X::SYSTEM_INTERRUPT_CONFIG_GPIO]);
}

void test_vl53l0x_set_address(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);
    uint8_t new_addr = 0x30;
    sensor.setAddress(new_addr);
    TEST_ASSERT_EQUAL(new_addr, sensor.getAddress());
    // Check that the address was written to the correct register
    TEST_ASSERT_EQUAL_HEX8(new_addr & 0x7F, i2c_mock.registers[VL53L0X::I2C_SLAVE_DEVICE_ADDRESS]);
}

void test_vl53l0x_read_write_reg(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);
    uint8_t reg = 0x2A;
    uint8_t value = 0x55;

    sensor.writeReg(reg, value);
    TEST_ASSERT_EQUAL_HEX8(value, i2c_mock.registers[reg]);
    // Prime the mock for the read operation
    i2c_mock.last_reg_addr = reg;
    TEST_ASSERT_EQUAL_HEX8(value, sensor.readReg(reg));
}

void test_vl53l0x_read_write_reg16(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);
    uint8_t reg = 0x3C;
    uint16_t value = 0xABCD;

    sensor.writeReg16Bit(reg, value);
    uint16_t read_value = (i2c_mock.registers[reg] << 8) | i2c_mock.registers[reg + 1];
    TEST_ASSERT_EQUAL_HEX16(value, read_value);
    // Prime the mock for the read operation
    i2c_mock.last_reg_addr = reg;
    TEST_ASSERT_EQUAL_HEX16(value, sensor.readReg16Bit(reg));
}

void test_vl53l0x_single_shot_measurement(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);
    sensor.init(); // Init to set stop_variable and other defaults

    // Mock the sequence for a single shot reading
    i2c_mock.registers[VL53L0X::RESULT_INTERRUPT_STATUS] = 0x07; // Indicate measurement is ready
    uint16_t expected_range = 123;
    i2c_mock.registers[VL53L0X::RESULT_RANGE_STATUS + 10] = (expected_range >> 8) & 0xFF;
    i2c_mock.registers[VL53L0X::RESULT_RANGE_STATUS + 11] = expected_range & 0xFF;

    uint16_t range = sensor.readRangeSingleMillimeters();
    TEST_ASSERT_EQUAL(expected_range, range);
}

void test_vl53l0x_timeout_occurred(void) {
    I2C i2c(i2c_default, 4, 5);
    VL53L0X sensor;
    sensor.setBus(&i2c);
    sensor.setTimeout(10); // 10 ms

    // Simulate a timeout by not setting the RESULT_INTERRUPT_STATUS register.
    // This will cause the read loop to time out.
    i2c_mock.registers[VL53L0X::RESULT_INTERRUPT_STATUS] = 0x00;
    advance_mock_time_us(11000); // Advance time past the 10ms timeout

    uint16_t range = sensor.readRangeSingleMillimeters();

    TEST_ASSERT_TRUE(sensor.timeoutOccurred());
    TEST_ASSERT_EQUAL_UINT16(65535, range); // The function should return max range on timeout
}


// Main function to run tests
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_i2c_begin);
    RUN_TEST(test_i2c_full_transaction);
    RUN_TEST(test_i2c_write_array);

    RUN_TEST(test_vl53l0x_init_success);
    RUN_TEST(test_vl53l0x_set_address);
    RUN_TEST(test_vl53l0x_read_write_reg);
    RUN_TEST(test_vl53l0x_read_write_reg16);
    RUN_TEST(test_vl53l0x_single_shot_measurement);
    RUN_TEST(test_vl53l0x_timeout_occurred);

    return UNITY_END();
}
