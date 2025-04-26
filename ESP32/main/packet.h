#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <utility>

// class Bluetooth : public Transmission_protocols
// {
//     void send_data() final;
// };

// class UDP : public Transmission_protocols
// {
//     void send_data() final;
// };

// class UART : public Transmission_protocols
// {
//     void send_data() final;
// };

std::byte crc8(std::span<std::byte> buffer);

/// @brief
class PACKET_DATA {
  uint32_t _rpm;              // done                    // Rotation per minute
  uint32_t _ADC_Readings[8];  // done
  std::byte _crc;             // crc
 public:
  PACKET_DATA() = default;

  uint32_t
  rpm()
  {
    return _rpm;
  }
  void
  rpm_set(uint32_t const rpm)
  {
    _rpm = rpm;
  }

  uint32_t
  adc(uint8_t const index)
  {
    return _ADC_Readings[index];
  };
  void
  adc_set(uint32_t adc, uint8_t const index)
  {
    _ADC_Readings[index] = adc;
  }

  std::byte
  crc()
  {
    return _crc;
  }
  void
  crc_set(std::byte const crc)
  {
    _crc = crc;
  }
};

extern struct PACKET_DATA packet_to_send;

constexpr size_t PACKET_SIZE {sizeof(PACKET_DATA)};