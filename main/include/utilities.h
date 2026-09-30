#pragma once

// Assumed pin mapping for the ESP32-2432S028 / TPM408 board.
// These values are intentionally centralized so they can be edited for a specific
// revision without hunting through the app code.

#define BOARD_POWERON        GPIO_NUM_27

#define BOARD_I2C_SDA        GPIO_NUM_21
#define BOARD_I2C_SCL        GPIO_NUM_22

#define CARDKB_I2C_ADDR      0x5F
#define CARDKB_I2C_SDA       GPIO_NUM_21
#define CARDKB_I2C_SCL       GPIO_NUM_22

#define BOARD_BAT_ADC        GPIO_NUM_34

#define BOARD_TOUCH_INT      GPIO_NUM_36
#define BOARD_KEYBOARD_INT   GPIO_NUM_35

// Standard 2432S028 SPI display pins (assumed TPM408).
#define BOARD_SPI_MOSI       GPIO_NUM_23
#define BOARD_SPI_MISO       GPIO_NUM_19
#define BOARD_SPI_SCK        GPIO_NUM_18
#define BOARD_TFT_CS         GPIO_NUM_5
#define BOARD_TFT_DC         GPIO_NUM_2
#define BOARD_TFT_RST        GPIO_NUM_4
#define BOARD_TFT_BACKLIGHT  GPIO_NUM_32

// Touch controller is usually XPT2046 on this board family.
#define BOARD_TOUCH_CS       GPIO_NUM_33
#define BOARD_TOUCH_IRQ      GPIO_NUM_36

// Keep SD support optional and board-configurable. Do not assume a T-Deck layout.
#define BOARD_SDCARD_CS      GPIO_NUM_13

#define BOARD_BOOT_PIN       GPIO_NUM_0

#ifndef BOARD_TBOX_G01
#define BOARD_TBOX_G01       GPIO_NUM_NC
#endif
#ifndef BOARD_TBOX_G03
#define BOARD_TBOX_G03       GPIO_NUM_NC
#endif
