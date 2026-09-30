#include "cardkb_keyboard.hpp"
#include "esp_log.h"

#define I2C_KEYPAD_ADDR 0x5F
#define I2C_MASTER_FREQ_HZ 100000

static const char *TAG = "CARDKB";

CardKBKeyboard::CardKBKeyboard(i2c_master_bus_handle_t i2c_handle)
{
    this->i2c_handle = i2c_handle;
}

esp_err_t CardKBKeyboard::init()
{
    if (i2c_handle == NULL)
    {
        ESP_LOGE(TAG, "I2C not initialized! Call bsp_i2c_init() first.");
        return ESP_FAIL;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = I2C_KEYPAD_ADDR,
        .scl_speed_hz = I2C_MASTER_FREQ_HZ
    };

    esp_err_t err = i2c_master_bus_add_device(i2c_handle, &dev_config, &keypad_dev);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "CardKB initialized successfully on I2C.");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to initialize CardKB: %s", esp_err_to_name(err));
    }

    return err;
}

uint32_t CardKBKeyboard::get_key()
{
    uint8_t key_ch = 0;
    uint8_t dummy_register = 0x00;

    esp_err_t ret = i2c_master_transmit_receive(
        keypad_dev,
        &dummy_register, 1,
        &key_ch, 1,
        1000
    );

    if (ret != ESP_OK)
    {
        return 0;
    }

    if (key_ch == 0)
    {
        return 0;
    }

    return key_ch;
}
