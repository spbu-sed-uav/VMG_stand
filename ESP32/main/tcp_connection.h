#pragma once

#include <string.h>
extern "C" {
#include "errno.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "fcntl.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "netdb.h"
#include "nvs_flash.h"
#include "sys/socket.h"
#include "xtensa/hal.h"
}
#include "uart.h"
#include "packet.h"
#include "transmission.h"
#include <cstddef>
#define INVALID_SOCK (-1)

#define YIELD_TO_ALL_MS 50

static constexpr uint8_t RX_BUFFER_SIZE{128};

constexpr auto TCP_CLIENT_CONNECT_ADDRESS{"192.168.0.105"};
constexpr auto TCP_CLIENT_CONNECT_PORT{"2001"};
//=================================================================
// TASK HANDLERS(For notifies)
//=================================================================

extern TaskHandle_t VOLTAGE_TASK_HANDLE;
extern TaskHandle_t CURRENT_TASK_HANDLE;
extern TaskHandle_t DISTURBNCE_TASK_HANDLE;

extern TaskHandle_t TEMPERATURE1_TASK_HANDLE;
extern TaskHandle_t TEMPERATURE2_TASK_HANDLE;
extern TaskHandle_t TEMPERATURE3_TASK_HANDLE;

extern TaskHandle_t WEIGHT_TASK_HANDLE;
extern TaskHandle_t RPM_TASK_HANDLE;
extern TaskHandle_t SEND_TASK_HANDLE;

extern TaskHandle_t UART_TASK_HANDLE;
//==================================================================
//
//==================================================================

static void log_socket_error(char const* tag, int const sock, int const err,
                             char const* message);

static int try_receive(char const* tag, int const sock, char* data,
                       size_t max_len);

static int socket_send(char const* tag, int const sock, char const* data,
                       size_t const len);

class TCP : public Transmission_protocols {
 protected:
  char const* TRANSMISSION_TAG = "TCP NON_BLOCKING_SOCKET";
  addrinfo* address_info;
  int sock;

 public:
  void send_data(char const* payload) final;
  std::array<char,PACKET_SIZE> get_data() final;
  void establish_connection() final;
};

void notify_all_with_value(uint32_t value);
void socket_error_handling(int sock, addrinfo const& addr_info);