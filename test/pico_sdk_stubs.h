#ifndef PICO_SDK_STUBS_H
#define PICO_SDK_STUBS_H

#include <cstdint>
#include <cstddef>
#include <map>

// Define uint for compatibility, as it's used in the original headers
typedef unsigned int uint;

// Mock para a estrutura i2c_inst_t
struct i2c_inst_t;
#define i2c_default (reinterpret_cast<i2c_inst_t*>(0))

// Enum para funções GPIO (simplificado)
enum gpio_function {
    GPIO_FUNC_I2C,
    GPIO_FUNC_NULL
};

// Erros Pico (simplificado)
#define PICO_ERROR_GENERIC -1

// Estrutura para mock do I2C
struct I2CMock {
    std::map<uint8_t, uint8_t> registers;
    uint8_t last_addr = 0;
    uint8_t last_reg_addr = 0;
};

extern I2CMock i2c_mock;

// Declarações de stubs para funções do SDK do Pico
extern "C" {
    // hardware_i2c
    void i2c_init(i2c_inst_t *i2c, uint baudrate);
    void i2c_deinit(i2c_inst_t *i2c);
    uint i2c_set_baudrate(i2c_inst_t *i2c, uint baudrate);
    int i2c_write_blocking(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop);
    int i2c_read_blocking(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop);

    // hardware_gpio
    void gpio_set_function(uint gpio, gpio_function fn);
    void gpio_pull_up(uint gpio);
    void gpio_init(uint pin);
    void gpio_set_dir(uint pin, bool out);
    void gpio_put(uint pin, bool value);
    gpio_function gpio_get_function(uint gpio);

    // pico_time
    uint64_t time_us_64();
    void sleep_ms(uint32_t ms);
    typedef uint64_t absolute_time_t;
    absolute_time_t get_absolute_time();
    uint64_t to_ms_since_boot(absolute_time_t t);
}

// Funções auxiliares para controle do mock no teste
void reset_mock_i2c();
uint64_t get_mock_time_us();
void advance_mock_time_us(uint64_t us);
gpio_function gpio_get_function(uint gpio);

#endif // PICO_SDK_STUBS_H
