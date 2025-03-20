#include "packets_and_sending.h"



Transmission_protocols::ptr create_transmission(transmission_types type)
{
    switch (type)
    {
    case transmission_types::TCP:
        return nullptr;
    //    return std::make_unique<TCP>();
        break;

    case transmission_types::UDP:
        return nullptr;
        break;

    case transmission_types::UART:
        return nullptr;
        break;

    case transmission_types::BLUETOOTH:
        return nullptr;
        break;

    default:
        return nullptr;
        break;
    }
}
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