#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "I2C/I2C.h"
#include "vl53l0x/VL53L0X.h"

// Define I2C pins
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

int main() {
    // Initialize stdio for USB output
    stdio_init_all();

    // Wait for USB connection
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("RP2040-VL53L0X Refactored Project\n");

    // Create I2C instance
    I2C i2c(i2c1, I2C_SDA_PIN, I2C_SCL_PIN);
    i2c.begin();
    i2c.setClock(400000); // 400kHz

    // Add pin info for picotool
    bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));

    // Create and configure sensor
    VL53L0X sensor;
    sensor.setBus(&i2c);

    printf("Initializing VL53L0X sensor...\n");
    if (!sensor.init()) {
        printf("Failed to detect and initialize VL53L0X sensor!\n");
        while (1) { tight_loop_contents(); }
    }
    printf("VL53L0X sensor initialized.\n");

    // Configure sensor settings
    sensor.setTimeout(500);
    sensor.setMeasurementTimingBudget(50000);
    sensor.setSignalRateLimit(0.1);

    printf("Starting measurements...\n");

    while (true) {
        uint16_t range = sensor.readRangeSingleMillimeters();
        if (sensor.timeoutOccurred()) {
            printf("Timeout!\n");
        } else {
            printf("Range: %d mm\n", range);
        }
        sleep_ms(500);
    }

    return 0;
}
