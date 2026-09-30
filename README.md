/*
 * CYD / TPM408 port for PocketSSH.
 *
 * This keeps the original SSH terminal functionality but removes the T-Deck-only
 * device assumptions (GT911, C3 keyboard, trackball). The current build is aimed
 * at the common ESP32-2432S028 family board with a CardKB keyboard connected
 * over I2C.
 */

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_vfs_fat.h"
#include "driver/spi_common.h"
#include "driver/sdspi_host.h"

#include "utilities.h"
#include "cardkb_keyboard.hpp"
#include "ssh_terminal.hpp"

#include "lvgl.h"

static const char *TAG = "main";
static lv_obj_t *ssh_screen;
static SSHTerminal *ssh_terminal = NULL;

static lv_obj_t *splash_screen = NULL;
static lv_obj_t *splash_img = NULL;
static lv_timer_t *splash_timer = NULL;
static int splash_frame = 0;

static i2c_master_bus_handle_t i2c_handle;

static void dismiss_splash_screen()
{
    if (splash_timer) {
        lv_timer_delete(splash_timer);
        splash_timer = NULL;
    }
    if (splash_screen) {
        lv_scr_load(ssh_screen);
        lv_obj_delete(splash_screen);
        splash_screen = NULL;
    }
}

static void splash_touch_cb(lv_event_t *e)
{
    (void)e;
    dismiss_splash_screen();
}

static void splash_timer_cb(lv_timer_t *timer)
{
    (void)timer;
    splash_frame = (splash_frame + 1) % 8;
}

static void show_splash_screen()
{
    splash_screen = lv_obj_create(NULL);
    lv_obj_set_style_bg_color(splash_screen, lv_color_black(), 0);
    lv_obj_add_event_cb(splash_screen, splash_touch_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_flag(splash_screen, LV_OBJ_FLAG_CLICKABLE);
    splash_img = lv_image_create(splash_screen);
    lv_obj_align(splash_img, LV_ALIGN_CENTER, 0, 0);
    lv_scr_load(splash_screen);
    splash_timer = lv_timer_create(splash_timer_cb, 150, NULL);
}

static void keypad_task(void *param)
{
    (void)param;

    ESP_ERROR_CHECK(i2c_master_bus_create(I2C_NUM_0, &i2c_handle));
    CardKBKeyboard keyboard(i2c_handle);
    if (keyboard.init() != ESP_OK)
    {
        ESP_LOGE("KEYPAD", "Failed to initialize CardKB keyboard");
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
        uint32_t key = keyboard.get_key();
        if (key)
        {
            if (splash_screen) {
                dismiss_splash_screen();
                vTaskDelay(pdMS_TO_TICKS(50));
                continue;
            }
            if (ssh_terminal && ssh_screen) {
                ssh_terminal->handle_key_input((char)key);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void device_init(void)
{
    gpio_reset_pin(BOARD_POWERON);
    gpio_set_direction(BOARD_POWERON, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_POWERON, 1);

    gpio_reset_pin(BOARD_TFT_CS);
    gpio_set_direction(BOARD_TFT_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_TFT_CS, 1);

    gpio_reset_pin(BOARD_TFT_DC);
    gpio_set_direction(BOARD_TFT_DC, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_TFT_DC, 0);

    gpio_reset_pin(BOARD_TFT_BACKLIGHT);
    gpio_set_direction(BOARD_TFT_BACKLIGHT, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_TFT_BACKLIGHT, 1);

    gpio_reset_pin(BOARD_TOUCH_CS);
    gpio_set_direction(BOARD_TOUCH_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_TOUCH_CS, 1);

    gpio_reset_pin(BOARD_SDCARD_CS);
    gpio_set_direction(BOARD_SDCARD_CS, GPIO_MODE_OUTPUT);
    gpio_set_level(BOARD_SDCARD_CS, 1);

    gpio_reset_pin(BOARD_SPI_MISO);
    gpio_set_direction(BOARD_SPI_MISO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BOARD_SPI_MISO, GPIO_PULLUP_ONLY);

    if (BOARD_BOOT_PIN != GPIO_NUM_NC) {
        gpio_reset_pin(BOARD_BOOT_PIN);
        gpio_set_direction(BOARD_BOOT_PIN, GPIO_MODE_INPUT);
        gpio_set_pull_mode(BOARD_BOOT_PIN, GPIO_PULLUP_ONLY);
    }
}

extern "C" void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    device_init();

    // The original T-Deck BSP is not used on the CYD/TPM408 board. This fork keeps
    // the SSH logic but the display/touch stack must be replaced with hardware-
    // specific LVGL initialization for the chosen panel revision.
    //
    // The active build now targets the CardKB + generic ESP32 board path.
    ESP_LOGI(TAG, "PocketSSH CYD/CardKB build started");
    ESP_LOGI(TAG, "Assumed display: ESP32-2432S028 / TPM408 panel");
    ESP_LOGI(TAG, "Assumed keyboard: M5Stack CardKB at 0x5F over I2C");

    ssh_terminal = new SSHTerminal();
    ssh_screen = ssh_terminal->create_terminal_screen();
    ssh_terminal->append_text("PocketSSH CYD/CardKB port\n");
    ssh_terminal->append_text("Board assumptions are in main/include/utilities.h\n");
    ssh_terminal->append_text("Replace display/touch init for the exact panel revision before use on hardware.\n");

    xTaskCreate(keypad_task, "keypad_task", 4096, NULL, 5, NULL);
}

void load_ssh_keys_from_sd(SSHTerminal* terminal)
{
    (void)terminal;
    ESP_LOGW(TAG, "SD key loading is kept as a legacy path and may need board-specific SPI wiring.");
}
