#include "transmission.h"

#include "tcp_connection.h"

Transmission_protocols::ptr
create_transmission(transmission_types type)
{
  switch (type) {
    case transmission_types::TCP:
      return nullptr;
      //    return std::make_unique<TCP>();
    case transmission_types::UDP:
      return nullptr;
    case transmission_types::UART:
      return nullptr;
    case transmission_types::BLUETOOTH:
      return nullptr;
    default:
      return nullptr;
  }
}

void
transmission_task(void* arg)
{
  auto* transmission = static_cast<TCP*>(arg);
  transmission->establish_connection();
  ESP_LOGI("TRANSMISSION", "GOT PAST INITALISATION");
  for (;;) {
    notify_all_with_value(0);
    ESP_LOGI("TRANSMISSION", "GOT THERE");
    std::array<char, sizeof(PACKET_DATA)> payload_array =
        transmission->get_data();

    notify_all_with_value(32000);

    char const* payload = payload_array.data();

    transmission->send_data(payload);

    vTaskDelay(200);
  }
}