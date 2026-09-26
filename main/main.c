/*
 * M0 占位固件：打印版本、芯片和 Flash 信息，板载 LED D4 闪烁表示在运行。
 * M1 起由开发者替换为真正的应用入口。
 */
#include <inttypes.h>

#include "driver/gpio.h"
#include "esp_app_desc.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED_D4_GPIO GPIO_NUM_12 /* 合宙 ESP32C3-CORE 板载 LED，高电平点亮 */

static const char *TAG = "main";

static void log_system_info(void)
{
    const esp_app_desc_t *app = esp_app_get_description();
    ESP_LOGI(TAG, "hello, %s %s (ESP-IDF %s)", app->project_name, app->version, app->idf_ver);

    esp_chip_info_t chip;
    esp_chip_info(&chip);
    ESP_LOGI(TAG, "chip: %s rev v%d.%d, %d core(s)", CONFIG_IDF_TARGET,
             chip.revision / 100, chip.revision % 100, chip.cores);

    /* configured 来自固件头里声明的容量，physical 来自 Flash 芯片的 JEDEC ID，两者不一致说明配置要改 */
    uint32_t configured = 0;
    uint32_t physical = 0;
    uint32_t jedec_id = 0;
    esp_flash_get_size(NULL, &configured);
    esp_flash_get_physical_size(NULL, &physical);
    esp_flash_read_id(NULL, &jedec_id);
    ESP_LOGI(TAG, "flash: configured %" PRIu32 " KB, physical %" PRIu32 " KB, JEDEC ID 0x%06" PRIx32,
             configured / 1024, physical / 1024, jedec_id);

    ESP_LOGI(TAG, "free heap: %" PRIu32 " bytes", esp_get_free_heap_size());
}

void app_main(void)
{
    log_system_info();

    gpio_reset_pin(LED_D4_GPIO);
    gpio_set_direction(LED_D4_GPIO, GPIO_MODE_OUTPUT);

    uint32_t level = 0;
    for (uint32_t tick = 0;; tick++) {
        level = !level;
        gpio_set_level(LED_D4_GPIO, level);
        if (tick % 10 == 0) {
            ESP_LOGI(TAG, "alive, uptime %" PRIu32 " s", tick / 2);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
