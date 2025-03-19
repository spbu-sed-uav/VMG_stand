#include "adc_reading.h"

void ADC_Driver::change_bitmask(uint8_t bit_to_swap)
{
    bit_mask ^= std::byte(1 << bit_to_swap); // check, does it works
}

void ADC_Driver::change_bitmask(std::byte swap_bitmask)
{
    bit_mask = swap_bitmask;
}

void ADC_Driver::change_channel(uint8_t channel_bit, adc_channel_t swap_channel)
{
    adc_channels[channel_bit] = swap_channel;
}

bool ADC_Driver::check_bitmask(uint8_t index)
{
    bool bitmask_check = static_cast<uint8_t>((bit_mask >> index) & std::byte(1));
    return bitmask_check;
}

adc_channel_t ADC_Driver::get_channel(uint8_t index)
{
    if (check_bitmask(index))
        ESP_LOGE(ADC_TAG, "Channel doesn't work");

    adc_channel_t answer = (index < adc_channels.size()) ? adc_channels[index] : ADC_CHANNEL_9; // possible bug place

    if (answer == ADC_CHANNEL_9)
        ESP_LOGE(ADC_TAG, "trying to get wrong channel");

    return answer;
}

uint32_t ADC_Driver::read_adc(uint8_t sensor_bit)
{
    int readings[10];

    ESP_ERROR_CHECK(adc_oneshot_read(adc_handler, Analogue_reader.get_channel(sensor_bit), &readings[0]));

    packet_to_send.adc_set(readings[0], sensor_bit);
}

void ADC_Driver::oneshot_adc_init() // maybe remove parameters, bcz they're global
{
    adc_oneshot_unit_handle_t handle = NULL;

    adc_oneshot_unit_init_cfg_t dig_cfg = {
        .unit_id = ADC_UNIT_1,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&dig_cfg, &handle));

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = ADC_ATTEN,
        .bitwidth = ADC_BIT_WIDTH,
    };

    for (uint8_t i = 0; i < 8; i++)
        if (static_cast<uint8_t>((bit_mask >> i) & std::byte(1)))
            ESP_ERROR_CHECK(adc_oneshot_config_channel(handle, adc_channels[i], &chan_config));

    ESP_LOGI(ADC_TAG, "ADC INITIALIZATION IS DONE");

    adc_handler = handle;
}

/// @brief
/// @param arg
static void adc_reading_task(void *arg)
{

    for (;;)
    {
        ulTaskNotifyTake(COUNTING_NOTIFY, pdMS_TO_TICKS(ONE_SECOND_MS));
        Analogue_reader.read_adc(*static_cast<uint8_t*>(arg));
        vTaskDelay(10);
    }
}
