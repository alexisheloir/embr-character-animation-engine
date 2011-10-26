/// \ingroup Examples
/// \file client.cpp
/// \brief This program demonstrate the usage of SMR lirbary in a basic client/server erchitecture
/*
#include <stdio.h>
#include <stdlib.h>
#include "SmrSTL.h"

//#ifdef WIN32
//  #include <winsock2.h>
//  #include <windows.h>
//#else
//  #include <sys/socket.h>
//  //#include <netdb.h>
//  #include <arpa/inet.h>
//  #include <sys/types.h>
//  #include <netdb.h>
//  #include <sys/types.h>
//  #include <netinet/in.h>
//#endif


*/


#include <string.h>
#include <stdio.h>
#include <iostream>
#include <boost/asio.hpp>
#include <stdlib.h>
#include "SmrSTL.h"



#define MSG_LENGTH 100000
#define PORT 5555
#define IP "127.0.0.1"

using boost::asio::ip::tcp;


int main(int argc, char* argv[])
{
  try
  {

  ifstream inFile(argv[1]);

  if(!inFile)
  {
    cout << "Cannot open input file embotCommands.txt" << endl;
    //exit(1);
  }

  string nextCommand("");  
  
  //create connection to the server
  std::cout<<"connecting to server..."<<endl;
  boost::asio::io_service io_service;
  tcp::resolver resolver(io_service); 
  tcp::resolver::query query(tcp::v4(),"127.0.0.1","5555" );
  tcp::resolver::iterator iterator = resolver.resolve(query);
  tcp::socket s(io_service);
  s.connect(*iterator);

  //read from file
  std::cout<<"reading from file..."<<endl;
  string command;
  string currentline;
  //char command_buffer[MSG_LENGTH] = "";
  
  while(inFile)
  {
    getline(inFile,currentline);
    currentline += "\n";
    command += currentline;
  }

  using namespace std; // For strlen.
  size_t buffer_length = command.size();
  //command_buffer[sizeof(command_buffer) - 1] = '\0';
  cout<<command;
  std::cout<<"Sending Buffer Length: "<<buffer_length<<endl;
  boost::asio::write(s, boost::asio::buffer(command, buffer_length));
  std::cout<<"...finished"<<endl;
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << " while sending content \n";
  }

  return 0;

}

