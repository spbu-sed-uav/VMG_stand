#pragma once
#include <array>

#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "packet.h"
#include "rpm_counter.h"
//================================================
// ADC VARIABLES
//================================================
constexpr uint8_t ADC_MAX_AMOUNT{8};
#define ADC_UNIT            ADC_UNIT_1
#define _ADC_UNIT_STR(unit) #unit
#define ADC_UNIT_STR(unit)  _ADC_UNIT_STR(unit)
#define ADC_CONV_MODE       ADC_CONV_SINGLE_UNIT_1
#define ADC_ATTEN           ADC_ATTEN_DB_0
#define ADC_BIT_WIDTH       ADC_BITWIDTH_12

#define ADC_OUTPUT_TYPE         ADC_DIGI_OUTPUT_FORMAT_TYPE1
#define ADC_GET_CHANNEL(p_data) ((p_data)->type1.channel)
#define ADC_GET_DATA(p_data)    ((p_data)->type1.data)

#define READ_LEN 256
float constexpr coeff_voltage{2450. / 4095.};
class ADC_Driver {
 private:
 public:
  std::byte bit_mask = std::byte(255);

  // adc_channel_t ADC_CURRENT{ADC_CHANNEL_0};
  // adc_channel_t ADC_VOLTAGE{ADC_CHANNEL_1};
  // adc_channel_t ADC_DISTURBANCE{ADC_CHANNEL_2};

  // adc_channel_t THERMISTOR_1{ADC_CHANNEL_3};
  // adc_channel_t THERMISTOR_2{ADC_CHANNEL_4};
  // adc_channel_t THERMISTOR_3{ADC_CHANNEL_5};

  // adc_channel_t EXTERNAL_1{ADC_CHANNEL_6};
  // adc_channel_t EXTERNAL_2{ADC_CHANNEL_7};
  
  std::array<adc_channel_t, ADC_MAX_AMOUNT> adc_channels{
      ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3,
      ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6, ADC_CHANNEL_7};

  ADC_Driver() = default;
  
  void
  adc_set(uint8_t bit, uint32_t value)
  {
    packet_to_send.adc_set(value, bit);
  };
  void
  change_bitmask(uint8_t bit_to_swap)
  {
    bit_mask ^= std::byte(1 << bit_to_swap);  // check, does it works
  }

  void
  change_bitmask(std::byte swap_bitmask)
  {
    bit_mask = swap_bitmask;
  }

  void
  change_channel(uint8_t channel_bit, adc_channel_t swap_channel)
  {
    adc_channels[channel_bit] = swap_channel;
  }

  bool
  check_bitmask(uint8_t index)
  {
    return static_cast<bool>(
        static_cast<uint8_t>((bit_mask >> index) & std::byte(1)) !=
        0U);  // maybe remove, but i think it's better readability
  }
  void adc_begin(std::byte bitmask);

  adc_channel_t get_channel(uint8_t index);

  void oneshot_adc_init();
  void read_adc(uint8_t sensor_bit, int& reading);
  static void read_adc(uint8_t sensor_bit);
};
static uint8_t constexpr READ_ADC_SIZE{1};
extern adc_oneshot_unit_handle_t adc_handler;

static char const* ADC_TAG = "ADC_READINGS";

//================================================
// ADC FUNCTIONS
//================================================

void adc_reading_task(void* arg);

extern ADC_Driver analogue_reader;