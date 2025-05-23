#include "HX711_reading.hpp"

#include "freertos/FreeRTOS.h"
#include "packet.hpp"
#include <cmath>

#include "driver/gpio.h"

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
    weight = HX711_get_units(1);
    ESP_LOGI("WEIGHTS", "WEIGHT - %i", int(weight));
    ulTaskNotifyTake(0, pdMS_TO_TICKS(ONE_SECOND_MS));
    packet_to_send.adc_set(static_cast<uint32_t>(int(weight)), 2);
    vTaskDelay(10);
  }
}

void setup_pin_for_weights(){
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);
  ESP_ERROR_CHECK(gpio_set_level(POWER_PINN, 1));
};