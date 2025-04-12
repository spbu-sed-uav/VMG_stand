#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"

extern "C" {
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_timer.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "sdkconfig.h"
#include "xtensa/hal.h"
}
void logging_task(void* arg);
//================================================
// MINE HEADERS
//================================================
#include "HX711_reading.h"
#include "adc_reading.h"
#include "rpm_counter.h"
#include "tcp_connection.h"
#include "temperature_sensor.h"
#include "transmission.h"
#include "wifi_connection.h"

/**
 * Brief:
 * This code is running on VMG-Stand.
 *
 * Done:
 * Part with IR Sensor, should config pins later, also need to test it
 *
 * Wip:
 * Need to add Voltage sensor support (current priority)
 * Need to add Current sensor support
 * Need to add PWM for motor
 * Need to add wifi transmission
 * Need to add some other sensors(termometers and smth else i think)
 * MAKE IT ALL READABLE
 *
 * ALL THIS UNTILL NEXT WEEK(17.02), WORK BTCH
 */

// data contains in next format: 12 bits for data and 4 for channel

//=================================================================
// LOGING TAGS
//=================================================================

static char const* MAIN_TAG = "MAIN";

extern "C" 
void app_main()
{
  packet_to_send.rpm_set(0x61+0x00610000+0x00006100+0x61000000);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,0);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,1);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,2);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,3);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,4);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,5);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,6);
  packet_to_send.adc_set(0x61+0x00610000+0x00006100+0x61000000,7);
  //=========================================================
  // WIFI CONNECTION
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI("WIFI", "ESP_WIFI_MODE_STA");
    wifi_init_sta();
  //=========================================================

  esp_timer_create_args_t const periodic_timer_args = {
      .callback = &periodic_timer_callback,
      /* name is optional, but may help identify the timer when debugging */
      .name     = "periodic"};

  time_rpm = esp_timer_get_time();

  esp_timer_handle_t periodic_timer;
  ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));

  ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, ONE_SECOND_MS));

  gpio_config_t const io_conf = {
      GPIO_INPUT_PIN_SEL,    // gpio mask
      GPIO_MODE_INPUT,       /*!< GPIO mode: set input/output mode       */
      GPIO_PULLUP_DISABLE,   /*!< GPIweight_reading_task-up   */
      GPIO_PULLDOWN_DISABLE, /*!< GPIO pull-down */
      GPIO_INTR_POSEDGE,     /*!< GPIO interrupt type     */
  };
  TCP transmission;

  gpio_config(&io_conf);
  int sensor_num = 3;
  int sensor_num_t = 4;
  // install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

  // hook isr handler for specific gpio pin
  ESP_ERROR_CHECK(
      gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_rotation_isr_handler, NULL));

   analogue_reader.oneshot_adc_init();

  xTaskCreatePinnedToCore(weight_reading_task, "Weight_reading", 4096, NULL, 15,
                          &WEIGHT_TASK_HANDLE, tskNO_AFFINITY);

//  xTaskCreatePinnedToCore(transmission_task, "Sending task", 2048,
//    &transmission, 15,
//                            &SEND_TASK_HANDLE, tskNO_AFFINITY);
  xTaskCreatePinnedToCore(logging_task, "LOGGING_TASK", 2048, NULL, 10, NULL,
                          tskNO_AFFINITY);
  
  xTaskCreatePinnedToCore(adc_reading_task, "ADC_Voltage", 2048, (void*)(0), 10,
                          &VOLTAGE_TASK_HANDLE,
                          1);  // check priorities, last null - handler
  xTaskCreatePinnedToCore(adc_reading_task, "ADC_Current", 2048, (void*)(1), 10,
                          &CURRENT_TASK_HANDLE,
                          1);  // check priorities, last null - handler
  xTaskCreatePinnedToCore(adc_reading_task, "ADC_Disturbance", 2048, (void*)(2),
                          10, &DISTURBNCE_TASK_HANDLE,
                          1);  // check priorities, last null - handler
  xTaskCreatePinnedToCore(temperature_task, "Temperature1", 2048, (void*)(sensor_num), 10, //need to use smth with actual address
                          &TEMPERATURE1_TASK_HANDLE,
                          0);  // check priorities, last null - handler
  xTaskCreatePinnedToCore(temperature_task, "Temperature2", 2048, (void*)(sensor_num_t), 10,
                          &TEMPERATURE2_TASK_HANDLE,
                          0);  // check priorities, last null - handler

  xTaskCreatePinnedToCore(rpm_safe_writing_task, "Writing_RPM", 2048, NULL, 10,
                          &RPM_TASK_HANDLE, 0);

//  xTaskNotify(RPM_TASK_HANDLE, 10000, eSetValueWithOverwrite);
//  xTaskNotify(WEIGHT_TASK_HANDLE, 10000, eSetValueWithOverwrite);

  printf("Minimum free heap size: %" PRIu32 " bytes\n",
         esp_get_minimum_free_heap_size());

  while (true) {
    vTaskSuspend(NULL);
  }
}
void
logging_task(void* arg)
{
  for (;;) {
    ESP_LOGI("LOGGING CHECK",
             "PACKET_TO_SEND_DATA: \n%d - rpm, \n%d - weight \n%d - "
             "adc_voltage,\n%d - "
             "adc_current,\n%d - adc_disturbance,\n%d - adc_temp_1,\n%d - "
             "adc_temp_2",
             int(packet_to_send.rpm()), int(packet_to_send.adc(0)),
             int(packet_to_send.adc(1)), int(packet_to_send.adc(2)),
             int(packet_to_send.adc(3)), int(packet_to_send.adc(4)),
             int(packet_to_send.adc(5)));
    vTaskDelay(1000);
  }
}