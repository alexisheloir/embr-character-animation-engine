
#include "UDPFeedbackSender.h"

static boost::asio::io_service static_io_service;

// Global static pointer used to ensure a single instance of the class.
UDPFeedbackSender *UDPFeedbackSender::m_singleton = NULL;


UDPFeedbackSender::UDPFeedbackSender(boost::asio::io_service& io_service):m_socket(io_service)
{
  udp::resolver resolver(io_service);
  udp::resolver::query query(udp::v4(), "localhost", "5556" );

  m_receiver_endpoint = *resolver.resolve(query);
  m_socket.open(udp::v4());

}

UDPFeedbackSender::~UDPFeedbackSender()
{
}

void UDPFeedbackSender::sendFeedbackMessage(const std::string &message)
{
  m_socket.send_to(boost::asio::buffer(message), m_receiver_endpoint);
}

UDPFeedbackSender *UDPFeedbackSender::getInstance( void )
{
  if( m_singleton== NULL ) {
    try
    {
      m_singleton = new UDPFeedbackSender(static_io_service);
    }
    catch (std::exception& e)
    {
      std::cerr << e.what() << std::endl;
    }
  }
  return m_singleton;
}

void UDPFeedbackSender::kill( void )
{
  if( m_singleton ) {
  delete m_singleton;
  m_singleton = NULL;
  }
}