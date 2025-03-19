#pragma once

#include "adc_reading.h"
#include <cstddef>
#include <utility>
#include <span>
#include <memory>
#include "tcp_connection.h"
enum class transmission_types
{
    NONE,
    TCP,
    UDP,
    UART,
    BLUETOOTH
};

class Transmission_protocols
{
public:
    using ptr = std::unique_ptr<Transmission_protocols>;
    
    virtual void send_data(void *pvParameters) = 0;
};

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

Transmission_protocols::ptr create_transmission();

/// @brief
class PACKET_DATA
{
    uint32_t _rpm;                    // done                    // Rotation per minute
    uint32_t _ADC_Readings[8]; // done
    std::byte _crc;                   // crc
public:
    PACKET_DATA() = default;

    uint32_t PACKET_DATA::rpm() { return _rpm; }
    void PACKET_DATA::rpm_set(const uint32_t rpm) { _rpm = rpm; }
    
    uint32_t PACKET_DATA::adc(const uint32_t index) { return _ADC_Readings[index]; };
    void PACKET_DATA::adc_set(const int adc, const uint32_t index) { _ADC_Readings[index] = adc; }

    std::byte PACKET_DATA::crc() { return _crc; }
    void PACKET_DATA::crc_set(const std::byte crc) { _crc = crc; }
};

extern struct PACKET_DATA packet_to_send;