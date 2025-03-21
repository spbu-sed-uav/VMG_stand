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
    protected:
    const char* TRANSMISSION_TAG; 
public:
    using ptr = std::unique_ptr<Transmission_protocols>;
    
    virtual void send_data(const char* payload) = 0;
    virtual void establish_connection() = 0;
    virtual const char* get_data() = 0;
};

Transmission_protocols::ptr create_transmission();

void transmission_task(void *arg);