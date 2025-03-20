#pragma once

#include <cstddef>
#include <utility>
#include <span>
#include <memory>

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
    
    virtual void send_data() = 0;
};

Transmission_protocols::ptr create_transmission();