#ifndef I2C_H
#define I2C_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include <cstddef>
#include <cstdint>

class I2C {
public:
    I2C(i2c_inst_t *i2c_instance, uint sda_pin, uint scl_pin);
    void begin();
    void end();
    void setClock(uint frequency);

    void beginTransmission(uint8_t address);
    uint8_t endTransmission(void);
    size_t write(uint8_t data);
    size_t write(const uint8_t *data, size_t quantity);

    uint8_t requestFrom(uint8_t address, size_t quantity);
    int read(void);
    int available(void);

private:
    i2c_inst_t *_i2c;
    uint _sda;
    uint _scl;
    uint8_t _txAddress;
    uint8_t _rxBuffer[32];
    uint8_t _rxBufferIndex;
    uint8_t _rxBufferLength;

    uint8_t _txBuffer[32];
    uint8_t _txBufferIndex;
    uint8_t _txBufferLength;

    bool _transmitting;
};

#endif // I2C_H
