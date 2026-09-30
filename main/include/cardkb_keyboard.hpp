#ifndef CARDKB_KEYBOARD_HPP
#define CARDKB_KEYBOARD_HPP

#include "driver/i2c_master.h"
#include "esp_err.h"
#include <cstdint>

class CardKBKeyboard
{
public:
    CardKBKeyboard(i2c_master_bus_handle_t i2c_handle);
    esp_err_t init();
    uint32_t get_key();

private:
    i2c_master_bus_handle_t i2c_handle;
    i2c_master_dev_handle_t keypad_dev;
};

#endif // CARDKB_KEYBOARD_HPP
