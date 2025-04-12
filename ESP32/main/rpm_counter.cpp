#include "rpm_counter.h"

uint64_t time_rpm       = 0;
uint16_t rotation_count = 0;
uint64_t final_rpm      = 0;

/// @brief ISR which increments amount of rotations
/// @param arg NULL
void IRAM_ATTR
gpio_rotation_isr_handler(void* arg)
{
  rotation_count++;
}

/// @brief timer callback, every second recalculates rpm based on amount of
/// rotations per second
/// @param arg NULL
void
periodic_timer_callback(void* arg)
{
  uint16_t const count         = rotation_count / AMOUNT_OF_WINGS;
//  ESP_LOGI("TAG","GOT %d", rotation_count);
  uint64_t const time_to_count = esp_timer_get_time() - time_rpm;

  final_rpm                    = count * ONE_SECOND_MS * 60 / time_to_count ;

  time_rpm                     = esp_timer_get_time();
  rotation_count               = 0;
}

/// @brief function to safe rewriting rpm
/// @param arg NULL
void
rpm_safe_writing_task(void* arg)
{
  for (;;) {
    if (final_rpm) {
      ulTaskNotifyTake(0, pdMS_TO_TICKS(ONE_SECOND_MS * 5));

      packet_to_send.rpm_set(final_rpm);

      final_rpm = 0;
    }
    else {
      vTaskDelay(100);
    }
  }
}
