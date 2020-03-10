// Name: Emanuel Aracena Beriguete
// Date: February 11, 2020
// Filename: main.cpp
// Description: Simple TCP client/server.

#include <array>
#include <arpa/inet.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <thread>
#include <unistd.h>

#include "Protocol.hpp"
#include "Logger.hpp"
#include "Server.hpp"
#include "Client.hpp"

void run_server (const bool logging, int port) {
  Server server(logging, port);
  server.initialize();
  server.run();
}


int run_client(const bool logging, const std::string & address, int port) {
  Client client(logging);
  int result = 0;
  result = client.connect_to_server(address, port);
  return result;
}

void print_help() {
  std::cout << "Proper usage: ./node [PORT]" << std::endl;
  std::cout << "PORT:\t specify port to listen on" << std::endl;
}

int main(int argc, const char **argv) {

  // Check arguments for proper format  
  if (argc < 2) {
    print_help();
    return 0;
  }

  int port = std::stoi(argv[1]);
  bool logging = false;
  int result = 0;


  // Start "Server/Listening" thread
  std::thread listener(run_server, logging, port);
  
  // Start client thread , used for connecting to other servers
  //result = run_client(logging, address, port);

  //if (result < 0)
  // std::cout << "[-] Exited with result: " << result << std::endl;

  listener.join();
  
  return result;
}
