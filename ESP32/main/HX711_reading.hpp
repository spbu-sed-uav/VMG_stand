#pragma once
extern "C" {
#include "HX711.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"
}
#include "rpm_counter.hpp"
//===============================================================
// HX711 CONSTANTS
//=================================================================
#define GPIO_SCALES_DATA GPIO_NUM_19
#define GPIO_SCLK        GPIO_NUM_21
#define AVG_SAMPLES      10
static constexpr int POWER_PIN{GPIO_NUM_32};
static constexpr gpio_num_t POWER_PINN{GPIO_NUM_32};
static constexpr unsigned long long GPIO_OUTPUT_PIN_SEL{1ULL << POWER_PIN};

void weight_reading_task(void* arg);
void setup_pin_for_weights();