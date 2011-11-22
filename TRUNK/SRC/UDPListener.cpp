#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string()
{
  using namespace std; // For time_t, time and ctime;
  time_t now = time(0);
  return ctime(&now);
}

int main()
{
  try
  {
    boost::asio::io_service io_service;

    udp::socket socket(io_service, udp::endpoint(udp::v4(), 5556));

    for (;;)
    {
      boost::array<char, 128> recv_buf;
      udp::endpoint remote_endpoint;
      boost::system::error_code error;
      size_t len = socket.receive_from(boost::asio::buffer(recv_buf),
                   remote_endpoint);

      std::cout.write(recv_buf.data(), len);
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}