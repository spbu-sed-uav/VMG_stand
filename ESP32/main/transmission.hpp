#pragma once

#include <cstddef>
#include <memory>
#include <array>
#include <span>
#include <utility>

enum class transmission_types {
  NONE,
  TCP,
  UDP,
  UART,
  BLUETOOTH
};

class Transmission_protocols {
 protected:
  char const* TRANSMISSION_TAG;

 public:
  using ptr = std::unique_ptr<Transmission_protocols>;

  virtual void send_data(char const* payload) = 0;
  virtual void establish_connection()         = 0;
  virtual std::array<char,40> get_data()              = 0;
};

Transmission_protocols::ptr create_transmission();

void transmission_task(void* arg);