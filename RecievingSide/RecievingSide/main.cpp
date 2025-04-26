#include <algorithm>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <iterator>

class PACKET_DATA {
 public:
  uint32_t _rpm;              // done                    // Rotation per minute
  uint32_t _ADC_Readings[8];  // done
  std::byte _crc;             // crc
};

using namespace boost::asio;
typedef boost::shared_ptr<ip::tcp::socket> socket_ptr;
boost::asio::io_context service;
boost::asio::ip::tcp::endpoint ep(ip::tcp::v4(), 2001);
void
client_session(socket_ptr sock)
{
  PACKET_DATA packet;
  while (true) {
    char data[512];
    size_t len = sock->read_some(
        buffer(data));  // add there struct and print what comes there
    std::cout << data[0];

    std::memcpy(&packet, &data, sizeof(PACKET_DATA));
    
    std::cout << "rpm - " << packet._rpm << "\n";
    std::cout << "adc readings:\n\n\n";
    std::cout << "VOLTAGE in mV: " << packet._ADC_Readings[0] << "\n";
    std::cout << "CURRENT in mV: " << packet._ADC_Readings[1] << "\n";
    std::cout << "WEIGHT in gramms: " << packet._ADC_Readings[2] << "\n";
    
    if (len > 0) {
      write(*sock, buffer("ok", 2));
    }
  }
}

int
main()
{
  std::cout << "Stared server\n";
  ip::tcp::acceptor acc(service, ep);

  while (true) {
    socket_ptr sock(new ip::tcp::socket(service));
    acc.accept(*sock);
    boost::thread(boost::bind(client_session, sock));
  }
}