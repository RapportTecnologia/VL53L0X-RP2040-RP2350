#ifndef PICO_SDK_STUBS_H
#define PICO_SDK_STUBS_H

#include <cstdint>
#include <cstddef> // For size_t

// Adicione aqui os stubs para as funções e tipos do Pico SDK que seu código usa.
// Por exemplo:
#include <cstdint>
#include <cstddef>

typedef unsigned int uint;

// Stubs para hardware_i2c
#define i2c_default (i2c_inst_t*)0
typedef struct i2c_inst i2c_inst_t;
static inline int i2c_write_blocking(i2c_inst_t *i2c, uint8_t addr, const uint8_t *src, size_t len, bool nostop) { return 0; }
static inline int i2c_read_blocking(i2c_inst_t *i2c, uint8_t addr, uint8_t *dst, size_t len, bool nostop) { return 0; }

// Stubs para pico_stdlib
static inline void sleep_ms(uint32_t ms) {}

// Stubs para hardware_gpio
static inline void gpio_init(uint pin) {}
static inline void gpio_set_dir(uint pin, bool out) {}
static inline void gpio_put(uint pin, bool value) {}

// Missing constants from the error log
#define GPIO_FUNC_I2C 0
#define GPIO_FUNC_NULL 0
#define PICO_ERROR_GENERIC -1

// Missing function stubs from the error log
static inline void i2c_init(i2c_inst_t *i2c, uint baudrate) {}
static inline void gpio_set_function(uint pin, uint fn) {}
static inline void gpio_pull_up(uint pin) {}
static inline void i2c_deinit(i2c_inst_t *i2c) {}
static inline void i2c_set_baudrate(i2c_inst_t *i2c, uint frequency) {}

// Stubs for pico/time.h
typedef uint64_t absolute_time_t;
static inline absolute_time_t get_absolute_time() { return 0; }
static inline uint64_t to_ms_since_boot(absolute_time_t t) { return 0; }

#endif // PICO_SDK_STUBS_H
