#ifndef _UDPFEEDBACKSENDER_H
#define _UDPFEEDBACKSENDER_H


#include <string>
// begin asio include for UdpFeedbackSender
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
// end asio include for UdpFeedbackSender

#include "../EMBRLoggers.h"


using boost::asio::ip::udp;
// declare a UdpFeedbackSender singleton here (port 5556)
class UDPFeedbackSender
{
public:
  UDPFeedbackSender(boost::asio::io_service& io_service);
  ~UDPFeedbackSender();

  void sendFeedbackMessage(const std::string &message);

public:
  static UDPFeedbackSender *getInstance( void );
  static void kill( void );

protected:
 /**
  * \brief UdpFeedbackSenderSingleton is a singleton class (one instance through the runtime)
  */
  static UDPFeedbackSender *m_singleton;

private:
  udp::socket m_socket;
  udp::endpoint m_remote_endpoint;
  udp::endpoint m_receiver_endpoint;
};

#endif