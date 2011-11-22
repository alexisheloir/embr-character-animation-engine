/// \ingroup Examples
/// \file server.cpp
/// \brief This program demonstrate the usage of SMR lirbary in a basic client/server erchitecture

#include <stdio.h>
#include <stdlib.h>

#include "SmrSTL.h"

#ifdef WIN32
  #include <winsock2.h>
#else
  #include <sys/socket.h>
  //#include <netdb.h>
  #include <arpa/inet.h>
  #include <sys/types.h>
  #include <netdb.h>
  #include <sys/types.h>
  #include <netinet/in.h>
#endif
#include <string.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>



#define MSG_LENGTH 256
#define PORT 5555
#define IP "127.0.0.1"

int sock;
char buffer[MSG_LENGTH];

boost::mutex io_mutex;
std::vector<string> embotCommands;

void listen_socket(void)
{
  while(1)
  {
    //RECEPTION
    if (  recv(sock, buffer, MSG_LENGTH, 0) <0) {
	  printf("bind() failed \n");
    exit(1);
    } 
    else 
    {
      boost::mutex::scoped_lock
      lock(io_mutex);
      //cout << "feeding command queue"<<endl;
      embotCommands.push_back(string(buffer));
    }
  }
  //vector<string>::iterator currentPos;
  //while(!embotCommands.empty())
  //{
  //  cout << "parsing commands"<<endl;
  //  currentPos = (embotCommands.begin());
  //  cout << *currentPos << endl;
  //  embotCommands.erase(currentPos);
  //}
}

void parse_commands(void)
{
  bool commandBegun = fasle;
  bool commandEnded = true;

  string commandType;
  map<string,string> parameters;

  stringstream commandLine;
  while(1)
  {
    if (embotCommands.size() > 0)
    {
      boost::mutex::scoped_lock
      lock(io_mutex);
      commandLine.str((*(embotCommands.begin())).c_str()) ;
      embotCommands.erase(embotCommands.begin());
    }
    if (commandLine.str().length() > 0)
    {
      string currentToken = "";
      string prevToken = "";
      commandLine >> currentToken;

      while(prevToken != currentToken) 
      {
        if (currentToken == "BEGIN") // this is the begin a a new command/constraint
        {
          if (commandEnded == false)
          {
            cout << "inconsistent BEGIN / END syntax" << endl;
            exit(1);
          }else
          {
            commandBegun = TRUE;
            commandEnded = FALSE;
          }
        }else if (currentToken == "END") // this is the begin a a new command/constraint
        {
          if (commandBegun == FALSE)
          {
            cout << "inconsistent BEGIN / END syntax" << endl;
            exit(1);
          }else
          {
            commandBegun = FALSE;
            commandEnded = TRUE;
            // the command is parsed now. it should be possible to instanciate a new constraint
            if (commandType == "GESTURE")
            {
              // instanciate a new cartesian constraint on one wrist
              cout << "instanciate a new cartesian constraint on one wrist" << endl;
            }else if (commandType == "GAZE")
            {
              //  instanciate a new constraint on the eye gaze
              cout << "instanciate a new constraint on the eye gaze" << endl;
            }else
            {
              cout << "unknown comman type" << endl;
            }
            
          }
        }else if ( (currentToken == "GESTURE" || currentToken == "GAZE") && commandBegun == TRUE  && commandEnded == FALSE)
        {
          commandType = currentToken;
        }else if (commandBegun == TRUE  && commandEnded == FALSE)
        {
          //split token into parameter and value
          string::size_type idx = currentToken.find(':');
          if (idx == string::npos)
          {
            cout << "syntax error in parameter declaration" << currentToken << endl;
          }else
          {
            string parameterKey = currentToken.substr(0, idx);
            string parameterValue = currentToken.substr(idx+1);
            parameters[parameterKey] = parameterValue;
          }
        }else
        {
          cout << "syntax error" << endl;
        }

        cout << currentToken << endl;
        prevToken = currentToken;
        commandLine >> currentToken;
      }
    commandLine.seekp(ios_base::beg);
    commandLine.seekg(ios_base::beg);
    commandLine.str("");
    }
    
  }
}

int main (int argc, char * argv[])
{
  WSACleanup();
  sock = 0;
  struct sockaddr_in addr;

#ifdef WIN32
  WSADATA wsaData;
  unsigned long ip;
#else
  struct in_addr ip;
#endif

  memset(&addr,0,sizeof(addr));


#ifdef WIN32
    //---------------------------------------------
    // Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsaData);

	ip = inet_addr(IP);
	if (ip == INADDR_NONE){
	  printf("Error while retrieving host name");
	  exit(1);
	}
#else
  if ( (inet_aton(IP, &ip)) == 0){
    printf("Error while retrieving host name");
    exit(1);
  }
#endif


#ifdef WIN32
  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if ( sock == INVALID_SOCKET) {
    printf("Error while creating socket");
    exit(1);
  }
#else
  if ( (sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    printf("Error while creating socket");
    exit(1);
  }
#endif

#ifdef WIN32
  addr.sin_addr.S_un.S_addr = ip;
#else
  addr.sin_addr = ip;
#endif
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);

  int bound = bind(sock, (struct sockaddr *)&addr, sizeof(struct sockaddr));
  if (bound < 0)
  {
    printf("connexion failed");
  }

  printf("reception...\n"); 

  shutdown(sock, SD_SEND);
  recv(sock, buffer, MSG_LENGTH, 0);

  boost::thread thrd1(boost::bind(&listen_socket));
  boost::thread thrd2(boost::bind(&parse_commands));
  thrd2.join();
  thrd1.join();

  closesocket(sock);
  WSACleanup();

  printf("end\n");
  exit(0);
}
