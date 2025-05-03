#include "adc_reading.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <esp_adc/adc_oneshot.h> // adc_oneshot_unit_handle_t
#include <hal/adc_types.h> // adc_channel_t

#include "packet.h"
adc_oneshot_unit_handle_t adc_handler = NULL;

ADC_Driver analogue_reader;
adc_channel_t
ADC_Driver::get_channel(uint8_t index)
{
  if (check_bitmask(index)) {
    ESP_LOGE(ADC_TAG, "Channel doesn't work");
  }

  adc_channel_t const answer = (index < adc_channels.size())
                                   ? adc_channels[index]
                                   : ADC_CHANNEL_9;  // possible bug place

  if (answer == ADC_CHANNEL_9) {
    ESP_LOGE(ADC_TAG, "trying to get wrong channel");
  }
  ESP_LOGD(ADC_TAG, "Channel is correct, returning %i", int(answer));
  return answer;
}
void
ADC_Driver::read_adc(uint8_t sensor_bit, int& reading)
{
  std::array<int, READ_ADC_SIZE> raw_data;
#if 0
  std::array<int, READ_ADC_SIZE> voltage;
#endif
  ESP_ERROR_CHECK(adc_oneshot_read(
      adc_handler, analogue_reader.get_channel(sensor_bit), &raw_data[0]));
  reading = raw_data[0];

  ESP_LOGD(ADC_TAG, "RAW DATA - %i", reading);
}

void
ADC_Driver::read_adc(uint8_t sensor_bit)
{
  std::array<int, READ_ADC_SIZE> readings;

  ESP_ERROR_CHECK(adc_oneshot_read(
      adc_handler, analogue_reader.get_channel(sensor_bit), &readings[0]));

  ESP_LOGD(ADC_TAG, "RAW DATA - %i %i %i", readings[0], readings[1],
           readings[2]);
  packet_to_send.adc_set(readings[0], sensor_bit);
}

void
ADC_Driver::oneshot_adc_init()  // maybe remove parameters, bcz they're global
{
  adc_oneshot_unit_handle_t handle          = NULL;

  adc_oneshot_unit_init_cfg_t const dig_cfg = {
      .unit_id  = ADC_UNIT_1,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };

  ESP_ERROR_CHECK(adc_oneshot_new_unit(&dig_cfg, &handle));

  adc_oneshot_chan_cfg_t const chan_config = {
      .atten    = ADC_ATTEN,
      .bitwidth = ADC_BIT_WIDTH,
  };

  for (uint8_t i = 0; i < 8; i++) {
    if (static_cast<uint8_t>((bit_mask >> i) & std::byte(1)) != 0u) {
      ESP_ERROR_CHECK(
          adc_oneshot_config_channel(handle, adc_channels[i], &chan_config));
    }
  }

  ESP_LOGD(ADC_TAG, "ADC INITIALIZATION IS DONE");

  adc_handler = handle;
}

/// @brief
/// @param arg
void
adc_reading_task(void* arg)
{
  for (;;) {
    ulTaskNotifyTake(0, pdMS_TO_TICKS(ONE_SECOND_MS));
    ADC_Driver::read_adc(*static_cast<uint8_t*>(arg));
    vTaskDelay(1000);
  }
}