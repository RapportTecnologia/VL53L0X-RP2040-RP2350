#include "pico_sdk_stubs.h"
#include <cstring> // For memcpy

// Global mock objects
I2CMock i2c_mock;
static uint64_t mock_time = 0;

// Mock implementations for Pico SDK functions (C linkage)
extern "C" {

void i2c_init(i2c_inst_t *i2c, uint baudrate) {}
void i2c_deinit(i2c_inst_t *i2c) {}

uint i2c_set_baudrate(i2c_inst_t *i2c, uint baudrate) {
    return baudrate;
}

void gpio_set_function(uint gpio, gpio_function fn) {}
void gpio_pull_up(uint gpio) {}

int i2c_write_blocking(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop) {
    if (len == 0) return PICO_ERROR_GENERIC;
    i2c_mock.last_addr = addr;

    // A write with nostop=true is just setting the register for a subsequent read.
    if (nostop) {
        if (len == 1) {
            i2c_mock.last_reg_addr = src[0];
            return 1; // Report 1 byte written (the address)
        }
    } 
    
    // This is a normal write transaction (or the write part of a write-then-read).
    uint8_t reg_addr = src[0];
    i2c_mock.last_reg_addr = reg_addr; // Always update last_reg_addr

    if (len > 1) {
        // Copy the data (all bytes after the address) into the mock register map.
        for (size_t i = 0; i < len - 1; ++i) {
            i2c_mock.registers[reg_addr + i] = src[i + 1];
        }
    }
    return len; // Report all bytes written
}

int i2c_read_blocking(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop) {
    if (i2c_mock.last_addr != addr) {
        // This is an unexpected condition in the mock. For a read to occur,
        // the address should have been set by a preceding write.
        return PICO_ERROR_GENERIC;
    }

    // Use the register address that was set by the last write operation.
    uint8_t reg_addr = i2c_mock.last_reg_addr;
    for (size_t i = 0; i < len; ++i) {
        dst[i] = i2c_mock.registers[reg_addr + i];
    }
    return len;
}

// pico_time stubs
uint64_t time_us_64() {
    return get_mock_time_us();
}

void sleep_ms(uint32_t ms) {
    advance_mock_time_us(ms * 1000);
}

absolute_time_t get_absolute_time() {
    return time_us_64();
}

uint64_t to_ms_since_boot(absolute_time_t t) {
    return t / 1000;
}

// hardware_gpio stubs
void gpio_init(uint pin) {}
void gpio_set_dir(uint pin, bool out) {}
void gpio_put(uint pin, bool value) {}

gpio_function gpio_get_function(uint gpio) {
    // For the purpose of the test, always return GPIO_FUNC_I2C
    return GPIO_FUNC_I2C;
}

} // extern "C"

// Helper functions for tests (C++ linkage)
void reset_mock_i2c() {
    i2c_mock.registers.clear();
    i2c_mock.last_addr = 0;
    i2c_mock.last_reg_addr = 0;
}

uint64_t get_mock_time_us() {
    uint64_t current_time = mock_time;
    mock_time += 100; // Automatically advance time a bit on each call
    return current_time;
}

void advance_mock_time_us(uint64_t us) {
    mock_time += us;
}