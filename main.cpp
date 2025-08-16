/**
 * \file rp2040-vl5310x.cpp
 * \brief Exemplo de uso do sensor VL53L0X no RP2040 (Pico/Pico W).
 *
 * Demonstra inicialização do I2C, configuração do sensor e leituras
 * periódicas de distância em milímetros via USB/serial.
 */
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "I2C.h"
#include "vl53l0x/VL53L0X.h"

// Pinos do barramento I2C utilizados
#define I2C_SDA_PIN 2
#define I2C_SCL_PIN 3

/**
 * \brief Função principal: inicializa comunicação, configura VL53L0X e exibe leituras.
 */
int main() {
    // Inicializa STDIO para saída via USB
    stdio_init_all();

    // Aguarda conexão USB para logging
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    printf("RP2040-VL53L0X Refactored Project\n");

    // Instância do I2C e configuração básica
    I2C i2c(i2c1, I2C_SDA_PIN, I2C_SCL_PIN);
    i2c.begin();
    i2c.setClock(400000); // 400kHz

    // Exporta info de pinos para picotool
    bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));

    // Cria e configura o sensor
    VL53L0X sensor;
    sensor.setBus(&i2c);

    printf("Initializing VL53L0X sensor...\n");
    if (!sensor.init()) {
        printf("Failed to detect and initialize VL53L0X sensor!\n");
        while (1) { tight_loop_contents(); }
    }
    printf("VL53L0X sensor initialized.\n");

    // Configuração recomendada
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
