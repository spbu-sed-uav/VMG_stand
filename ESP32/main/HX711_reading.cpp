#include "HX711_reading.h"

#include "freertos/FreeRTOS.h"
#include "packet.h"
#include <cmath>

/// @brief
/// @param arg
void
weight_reading_task(void* arg)
{
  HX711_init(GPIO_SCALES_DATA, GPIO_SCLK, eGAIN_128);
  HX711_tare();
  HX711_set_scale(420.52);
  ESP_LOGI("WEIGHTS", "Initialization is done");
  float weight = 0;

  for (;;) {
    weight = HX711_get_units(AVG_SAMPLES);
    printf("current weight_units - %f",weight);

    ulTaskNotifyTake(0, pdMS_TO_TICKS(ONE_SECOND_MS));
    packet_to_send.adc_set(static_cast<uint32_t>(round(weight)), 0);
    vTaskDelay(50);
  }
}