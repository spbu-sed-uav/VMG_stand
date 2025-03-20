#include "tcp_connection.h"

TaskHandle_t VOLTAGE_TASK_HANDLE = NULL;
TaskHandle_t CURRENT_TASK_HANDLE = NULL;
TaskHandle_t DISTURBNCE_TASK_HANDLE = NULL;

TaskHandle_t TEMPERATURE1_TASK_HANDLE = NULL;
TaskHandle_t TEMPERATURE2_TASK_HANDLE = NULL;
TaskHandle_t TEMPERATURE3_TASK_HANDLE = NULL;

TaskHandle_t WEIGHT_TASK_HANDLE = NULL;
TaskHandle_t RPM_TASK_HANDLE = NULL;
TaskHandle_t SEND_TASK_HANDLE = NULL;

void log_socket_error(const char *tag, const int sock, const int err, const char *message)
{
    ESP_LOGE(tag, "[sock=%d]: %s\n"
                  "error=%d: %s",
             sock, message, err, strerror(err));
}

int try_receive(const char *tag, const int sock, char *data, size_t max_len)
{
    int len = recv(sock, data, max_len, 0);
    if (len < 0)
    {
        if (errno == EINPROGRESS || errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return 0; // Not an error
        }
        if (errno == ENOTCONN)
        {
            ESP_LOGW(tag, "[sock=%d]: Connection closed", sock);
            return -2; // Socket has been disconnected
        }
        log_socket_error(tag, sock, errno, "Error occurred during receiving");
        return -1;
    }

    return len;
}

int socket_send(const char *tag, const int sock, const char *data, const size_t len)
{
    int to_write = len;
    while (to_write > 0)
    {
        int written = send(sock, data + (len - to_write), to_write, 0);
        if (written < 0 && errno != EINPROGRESS && errno != EAGAIN && errno != EWOULDBLOCK)
        {
            log_socket_error(tag, sock, errno, "Error occurred during sending");
            return -1;
        }
        to_write -= written;
    }
    return len;
}

void notify_all_with_value(uint32_t value)
{
    xTaskNotify(VOLTAGE_TASK_HANDLE, value, eSetValueWithOverwrite);
    xTaskNotify(CURRENT_TASK_HANDLE, value, eSetValueWithOverwrite);
    xTaskNotify(DISTURBNCE_TASK_HANDLE, value, eSetValueWithOverwrite);

    xTaskNotify(TEMPERATURE1_TASK_HANDLE, value, eSetValueWithOverwrite);
    xTaskNotify(TEMPERATURE2_TASK_HANDLE, value, eSetValueWithOverwrite);
    xTaskNotify(TEMPERATURE3_TASK_HANDLE, value, eSetValueWithOverwrite);

    xTaskNotify(WEIGHT_TASK_HANDLE, value, eSetValueWithOverwrite);
    xTaskNotify(RPM_TASK_HANDLE, value, eSetValueWithOverwrite);
}

void socket_error_handling(int sock, const addrinfo &addr_info)
{
    if (sock != INVALID_SOCK)
    {
        close(sock);
    }
    free(const_cast<addrinfo *>(&addr_info));
    vTaskDelete(SEND_TASK_HANDLE);
}

void TCP::send_data()
{

    // byte a; doesn't work, maybe just use char or 8bit smth idk
    static char rx_buffer[128];

    int len_msg = 0;
    int flags = 0;
    static const char *TAG_TCP_SOCKET = "nonblocking-socket-client";

    struct addrinfo hints = {.ai_socktype = SOCK_STREAM};
    struct addrinfo *address_info;
    int sock = INVALID_SOCK;

    int res = getaddrinfo(TCP_CLIENT_CONNECT_ADDRESS, TCP_CLIENT_CONNECT_PORT, &hints, &address_info);
    if (res != 0 || address_info == NULL)
    {
        ESP_LOGE(TAG_TCP_SOCKET, "couldn't get hostname for `%s` "
                                 "getaddrinfo() returns %d, addrinfo=%p",
                 TCP_CLIENT_CONNECT_ADDRESS, res, address_info);
        socket_error_handling(sock, *address_info);
    }

    // Creating client's socket
    sock = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (sock < 0)
    {
        log_socket_error(TAG_TCP_SOCKET, sock, errno, "Unable to create socket");
        socket_error_handling(sock, *address_info);
    }
    ESP_LOGI(TAG_TCP_SOCKET, "Socket created, connecting to %s:%s", TCP_CLIENT_CONNECT_ADDRESS, TCP_CLIENT_CONNECT_PORT);

    // Marking the socket as non-blocking
    flags = fcntl(sock, F_GETFL);
    if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        log_socket_error(TAG_TCP_SOCKET, sock, errno, "Unable to set socket non blocking");
    }

    if (connect(sock, address_info->ai_addr, address_info->ai_addrlen) != 0)
    {
        if (errno == EINPROGRESS)
        {
            ESP_LOGD(TAG_TCP_SOCKET, "connection in progress");
            fd_set fdset;
            FD_ZERO(&fdset);
            FD_SET(sock, &fdset);

            // Connection in progress -> have to wait until the connecting socket is marked as writable, i.e. connection completes
            res = select(sock + 1, NULL, &fdset, NULL, NULL);
            if (res < 0)
            {
                log_socket_error(TAG_TCP_SOCKET, sock, errno, "Error during connection: select for socket to be writable");
                socket_error_handling(sock, *address_info);
            }
            else if (res == 0)
            {
                log_socket_error(TAG_TCP_SOCKET, sock, errno, "Connection timeout: select for socket to be writable");
                socket_error_handling(sock, *address_info);
            }
            else
            {
                int sockerr;
                socklen_t len = (socklen_t)sizeof(int);

                if (getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)(&sockerr), &len) < 0)
                {
                    log_socket_error(TAG_TCP_SOCKET, sock, errno, "Error when getting socket error using getsockopt()");
                    socket_error_handling(sock, *address_info);
                }
                if (sockerr)
                {
                    log_socket_error(TAG_TCP_SOCKET, sock, sockerr, "Connection error");
                    socket_error_handling(sock, *address_info);
                }
            }
        }
        else
        {
            log_socket_error(TAG_TCP_SOCKET, sock, errno, "Socket is unable to connect");
            socket_error_handling(sock, *address_info);
        }
    }
    for (;;)
    {

        notify_all_with_value(0);

        PACKET_DATA initial_payload = packet_to_send;

        notify_all_with_value(32000);

        std::byte aux[sizeof(PACKET_DATA) - 1] = {};

        memcpy(&aux, &initial_payload, sizeof(PACKET_DATA) - 1);
        std::span<std::byte> span_bytes(aux);
        initial_payload.crc_set(crc8(span_bytes));

        static std::array<char, sizeof(PACKET_DATA)> payload_array; // there can be conversion errors (overflow I guess)

        memcpy(&payload_array, &initial_payload, sizeof(initial_payload)); // check later

        const char *payload = &payload_array[0]; // can be error somewhere

        ESP_LOGI(TAG_TCP_SOCKET, "Client sends data to the server...");
        len_msg = socket_send(TAG_TCP_SOCKET, sock, payload, strlen(payload));
        if (len_msg < 0)
        {
            ESP_LOGE(TAG_TCP_SOCKET, "Error occurred during socket_send");
            socket_error_handling(sock, *address_info);
        }
        ESP_LOGI(TAG_TCP_SOCKET, "struct was sent");

        // Keep receiving until we have a reply
        do
        {
            len_msg = try_receive(TAG_TCP_SOCKET, sock, rx_buffer, sizeof(rx_buffer));
            if (len_msg < 0)
            {
                ESP_LOGE(TAG_TCP_SOCKET, "Error occurred during try_receive");
                socket_error_handling(sock, *address_info);
            }
            vTaskDelay(pdMS_TO_TICKS(YIELD_TO_ALL_MS));
        } while (len_msg == 0);
        ESP_LOGI(TAG_TCP_SOCKET, "Received: %.*s", len_msg, rx_buffer);

        vTaskDelay(200);
    }
}