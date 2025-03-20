#pragma once

#ifndef __PACKETS_AND_SENDING_H__
#define __PACKETS_AND_SENDING_H__

#include "adc_reading.h"
#include <cstddef>
#include <utility>
#include <span>
#include <memory>

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
class PACKET_DATA
{
    uint32_t _rpm;                    // done                    // Rotation per minute
    uint32_t _ADC_Readings[8]; // done
    std::byte _crc;                   // crc
public:
    PACKET_DATA() = default;

    uint32_t rpm() { return _rpm; }
    void rpm_set(const uint32_t rpm) { _rpm = rpm; }
    
    uint32_t adc(const uint8_t index) { return _ADC_Readings[index]; };
    void adc_set(uint32_t adc, const uint8_t index) { _ADC_Readings[index] = adc; }

    std::byte crc() { return _crc; }
    void crc_set(const std::byte crc) { _crc = crc; }
};

extern struct PACKET_DATA packet_to_send;

#endif