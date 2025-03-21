#include "transmission.h"
#include "tcp_connection.h"

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

void task_transmission(void *arg)
{
    TCP *transmission = static_cast<TCP *>(arg);
    transmission->establish_connection();
    for (;;)
    {

        notify_all_with_value(0);

        const char *payload = transmission->get_data();

        notify_all_with_value(32000);

        transmission->send_data(payload);

        vTaskDelay(200);
    }
}