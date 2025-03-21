#include "packet.h"

PACKET_DATA packet_to_send;
// class Bluetooth : public Transmission_protocols
// {
//     void send_data() final
//     {
//     }
// };

// class UDP : public Transmission_protocols
// {
//     void send_data() final
//     {
//     }
// };

// class UART : public Transmission_protocols
// {
//     void send_data() final
//     {
//     }
// };
// TODO: remake to crc table
std::byte crc8(std::span<std::byte> buffer)
{
    std::byte crc{0};
    for (const auto byte : buffer)
    {
        std::byte data = byte;
        for (int j = 8; j > 0; j--)
        {
            crc = static_cast<bool>((crc ^ data) & std::byte{1}) ? (crc >> 1) ^ std::byte{0x8C} : (crc >> 1);
            data >>= 1;
        }
    }
    return crc;
}