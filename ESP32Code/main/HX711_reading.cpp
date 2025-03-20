#include "HX711_reading.h"
#include "freertos/FreeRTOS.h"
#include "packets_and_sending.h"

/// @brief 
/// @param arg 
static void weight_reading_task(void *arg)
{
    HX711_init(GPIO_SCALES_DATA, GPIO_SCLK, eGAIN_128);
    HX711_tare();

    ESP_LOGI("WEIGHTS", "Initialization is done");
    uint64_t weight = 0;

    for (;;)
    {
        weight = HX711_get_units(AVG_SAMPLES);

        ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(ONE_SECOND_MS));

        packet_to_send.adc_set(weight, 0);
        vTaskDelay(100);
    }
}