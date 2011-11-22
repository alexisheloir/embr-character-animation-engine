//EMBR is a real time character animation engine.
//Copyright (C) 2010 Alexis Heloir
//
//This program is free software: you can redistribute it and/or
//modify it under the terms of the GNU Lesser General Public
//License as published by the Free Software Foundation, version 3
//of the License.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY, without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//Lesser General Public License for more details.
//
//You should have received a copy of the GNU Lesser General Public
//License along with this program.  If not, see
//<http://www.gnu.org/licenses/>.


#ifndef _SOCKETLISTENER_H
#define _SOCKETLISTENER_H

#include "SmrSTL.h"
#include "Parser.h"

#include <cstdlib>
#include <boost/asio.hpp>
#define MSG_LENGTH 50000 //33 * ethernet v2 MTU  

#define PORT 5555
#define IP "127.0.0.1"
#define MAXPENDING 5    /* Maximum outstanding connection requests */
using boost::asio::ip::tcp;

typedef boost::shared_ptr<tcp::socket> socket_ptr;

//typedef struct feedbackMessageStruct{
//  typedef enum feedbackMessageStatusEnum{SUCCES,FAILURE,FATAL} feedbackMessageStatus;
//  unsigned int id;
//  unsigned int time;
//  feedbackMessageStatus status; 
//  string feedback;
//} FeedbackMessage;

class SocketListener
{
public:
  SocketListener();
  ~SocketListener();
  void listenSocket();
  void readCommandsFromFile();
  void readCommandsFromString( string _commands);
    
  void setParser(Parser *_parser);
  void server(boost::asio::io_service& _io_service, short _port);
  //void do_write(FeedbackMessage msg);
  void handle_write(socket_ptr _sock);

private:

  int m_sock;
  char m_buffer[MSG_LENGTH];
  string m_lineBuffer;
  Parser *m_parser;
  //std::vector<FeedbackMessage> m_feedback_msgs_;
  socket_ptr m_socketPtr;

};

#endif
