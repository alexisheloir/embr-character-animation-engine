#include "SocketListener.h"
#include <cstring>

void handle_read_old(socket_ptr &_sock, Parser *_parser)
{
    try
    {
        size_t recvMsgSize=0;
        char m_buffer[MSG_LENGTH];
        string m_lineBuffer = "";
        for (;;)
        {
            boost::system::error_code error;
            LOG_TRACE(parserLogger,"start reading from socket");
            size_t backupsize = recvMsgSize;
            
            //do {
            recvMsgSize += _sock->read_some(boost::asio::buffer(&m_buffer[recvMsgSize],MSG_LENGTH-recvMsgSize),error);
            //} while (!error);
            
            if (error == boost::asio::error::eof)
            {
                recvMsgSize = backupsize;
                LOG_INFO(parserLogger,"Connection closed cleanly by peer");
                break; // Connection closed cleanly by peer.
            }
            else if (error)
            {
                LOG_INFO(parserLogger,"Connection closed unexpectedly");
                break;
            }
        }
        if (recvMsgSize > 0)
        {
            m_buffer[recvMsgSize] = '\0';
            stringstream ss(m_buffer);
            char buffer[MSG_LENGTH] = "";
            int line = 0;
            while (ss.getline(buffer,MSG_LENGTH))
            {
                if ( (!ss.eof() && !ss.fail()) )
                {
                    m_lineBuffer = string(buffer);
                    LOG_INFO(parserLogger,m_lineBuffer);
                    line++;
                    _parser->enqueueCommand(m_lineBuffer);
                    buffer[0] = '\0';
                }else
                {
                    LOG_ERROR(parserLogger,"buffered socket reader caused an error");
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }    
}
/**
 *
 * returns True if something has been read
 */
void handle_read_new(socket_ptr &_sock, Parser *_parser)
{
    size_t recvMsgSize=0;
    char m_buffer[MSG_LENGTH];
    char buf[128]="";
    char line[128]="";
    char remain[256]="";
    remain[0]='\0';
    int sizeOfLastRemain=0;
    string EMBRScriptSequence="";

    string m_lineBuffer = "";
    for (;;)
    {
        boost::system::error_code error;
        LOG_TRACE(parserLogger,"start reading from socket");
//
//      recvMsgSize += _sock->read_some(boost::asio::buffer(&m_buffer[recvMsgSize],MSG_LENGTH-recvMsgSize),error);
      recvMsgSize = _sock->read_some(boost::asio::buffer(&buf[0], 127),error);
      buf[recvMsgSize] = '\0';  
      LOG_INFO(parserLogger,buf << "\n"); 
//      
//      //append the buffer to the last unfinished line (stored in the remain buffer)
      strcat(remain,buf);
//      remain[0]='\0';
      LOG_INFO(parserLogger,remain << "\n"); 

      //start at the beginning of the bufffer
      char* currentPosition = remain;
      //get the first line break on the way
      char* newLinePosition = strchr(currentPosition,'\n');
      while(newLinePosition != NULL)
      {
        //copy the line in the line buffer
        std::strncpy(line, currentPosition, newLinePosition-currentPosition);
        line[newLinePosition-currentPosition]='\n';
        line[newLinePosition+1-currentPosition]='\0';
        LOG_INFO(parserLogger,line);
        //enqueue the line as a command
        //EMBRScriptSequence += string(line);  
        _parser->enqueueCommand(string(line));
        currentPosition = newLinePosition+1;
        //get the next line break position
        newLinePosition = strchr(currentPosition,'\n');  
        //if no more line break, store the remaining of the buffer in the remain buffer  
        if (newLinePosition == NULL)
        {
          strncpy(remain, currentPosition, &remain[128+sizeOfLastRemain] - currentPosition);
          remain[&remain[128+sizeOfLastRemain] - currentPosition]='\0';
          sizeOfLastRemain = strlen(remain);  
          LOG_INFO(parserLogger,"remain: " << remain << "\n" << "size: " << sizeOfLastRemain << "\n");  
        }
      }

      if (error == boost::asio::error::eof)
      {
        //if ( EMBRScriptSequence.length() > 0)
        //{
            //LOG_INFO(parserLogger, EMBRScriptSequence);
            //_parser->enqueueCommand(EMBRScriptSequence);
            //EMBRScriptSequence = "";
        //}
            
        //recvMsgSize = backupsize;
        LOG_INFO(parserLogger,"Connection closed cleanly by peer");
        //reopen socket
        //_sock = socket_ptr(new tcp::socket(_io_service));
        break; // Connection closed cleanly by peer.
      }
      else if (error)
      {
        LOG_INFO(parserLogger,"Connection closed unexpectedly");
        break;
      }
    }
//    // buffer should be filled here
//    if (recvMsgSize > 0)
//    {
//      m_buffer[recvMsgSize] = '\0';
//      stringstream ss(m_buffer);
//      char buffer[256] = "";
//      int line = 0;
//      while (ss.getline(buffer,MSG_LENGTH))
//      {
//        if ( (!ss.eof() && !ss.fail()) )
//        {
//          m_lineBuffer = string(buffer);
//          LOG_INFO(parserLogger,m_lineBuffer);
//          line++;
//          _parser->enqueueCommand(m_lineBuffer);
//        }else
//        {
//          LOG_ERROR(parserLogger,"buffered socket reader caused an error");
//        }
//      }
//      return true;  
//    }else
//    {
//      return false;
//    }
}

void SocketListener::handle_write(socket_ptr _sock)
{
  //std::vector<FeedbackMessage>::iterator messageIterator;
  //messageIterator = m_feedback_msgs_.begin();
  ////for (messageIterator = m_feedback_msgs_.begin(); messageIterator < m_feedback_msgs_.end(); messageIterator++ )
  //while(messageIterator < m_feedback_msgs_.end())
  //{
  //  try
  //  {
  //    boost::system::error_code error;
  //    string command = (*messageIterator).feedback;
  //    size_t buffer_length = command.size();
  //    _sock->write_some(boost::asio::buffer(command,buffer_length),error);
  //    messageIterator = m_feedback_msgs_.erase(messageIterator);
  //    //boost::asio::read_(_sock, boost::asio::buffer(command, buffer_length));
  //  }
  //  catch (std::exception& e)
  //  {
  //    std::cerr << "Exception iwhile sending feedback" << e.what() << "\n";
  //  }
  //}
}



SocketListener::SocketListener()
{
  m_parser = NULL;
  m_lineBuffer = "";
  m_sock = 0;
}

SocketListener::~SocketListener()
{
    
}

void SocketListener::server(boost::asio::io_service& _io_service, short _port)
{

  tcp::acceptor a(_io_service, tcp::endpoint(tcp::v4(), 5555));
  for (;;)
  {
    m_socketPtr = socket_ptr(new tcp::socket(_io_service));  
    a.accept(*m_socketPtr);
    handle_read_new(m_socketPtr, m_parser);  
    //if(handle_read(m_socketPtr, m_parser)) // If information has been read from the buffer, this means it has been closed, need to re-open it
    //{
    //  m_socketPtr = socket_ptr(new tcp::socket(_io_service));     
    //}
  }
}

void SocketListener::setParser(Parser *_parser)
{
  m_parser = _parser;
}

void SocketListener::readCommandsFromFile()
{
  ifstream inFile("embotcommands.txt");

  if(!inFile)
  {
    LOG_ERROR(logger,"Cannot open input file embotcommands.txt");
    exit(1);
  }

  string nextCommand("");

  while(inFile)
  {
    char buffer[MSG_LENGTH] = "";
    inFile.getline(buffer, MSG_LENGTH);
    LOG_DEBUG(logger,buffer);
    m_parser->enqueueCommand(string(buffer));
  }
    
}

void SocketListener::listenSocket()
{
  try
  {
   boost::asio::io_service io_service; 
   server(io_service, PORT);
  }
  catch (std::exception& e)
  {
    LOG_ERROR(parserLogger,"Exception: " << e.what() << endl);
  }
}
