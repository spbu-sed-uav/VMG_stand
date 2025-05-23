#pragma once
#include <stdio.h>

#include "adc_reading.hpp"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "packet.hpp"
#include "sdkconfig.h"

struct recieving_package {
  uint32_t voltage;
  uint32_t current;
};

void
uart_config(uart_config_t uart_config);
void
uart_setup();

void
uart_task(void* arg);