#include "uart.h"

#include <cstring>
constexpr uart_port_t UART_NUMB{UART_NUM_1};  // gpio rp5-4tx
constexpr int BUFFER{1024};

static constexpr int RX_PORT{16};
static constexpr int TX_PORT{17};

int const ECHO_TEST_RTS = UART_PIN_NO_CHANGE;
int const ECHO_TEST_CTS = UART_PIN_NO_CHANGE;

void
uart_config(uart_config_t uart_config)
{
  ESP_ERROR_CHECK(uart_param_config(UART_NUMB, &uart_config));
}

void
uart_setup()
{
  uart_config_t const uart_cfg = uart_config_t{
      .baud_rate  = 115200,
      .data_bits  = UART_DATA_8_BITS,
      .parity     = UART_PARITY_DISABLE,
      .stop_bits  = UART_STOP_BITS_1,
      .flow_ctrl  = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };

  ESP_ERROR_CHECK(uart_driver_install(UART_NUMB, BUFFER * 2, 0, 0, NULL, 0));
  uart_config(uart_cfg);
  ESP_ERROR_CHECK(
      uart_set_pin(UART_NUMB, TX_PORT, RX_PORT, ECHO_TEST_RTS, ECHO_TEST_CTS));
}

void
uart_task(void* arg)
{
  ESP_LOGI("AAAA", "AKOGSOKG");
  uint8_t data[BUFFER];
  ESP_LOGI("AAAA", "AKOGSOKG");
  uart_setup();
  recieving_package adc_pack;
  for (;;) {
    int len = uart_read_bytes(UART_NUMB, data, (sizeof(recieving_package)), 10);
    std::memcpy(&adc_pack, data, sizeof(recieving_package));
    // Write data back to the UART
    //    uart_write_bytes(ECHO_UART_PORT_NUM, (char const*)data, len);
    if (len) {
      data[len] = '\0';
      ESP_LOGI("UART", "Recv str: %s", (char*)data);
      analogue_reader.adc_set(0, adc_pack.voltage);
      analogue_reader.adc_set(1, adc_pack.current);
    }
    ESP_LOGI("UART", "got there");
    vTaskDelay(200);
  }
}