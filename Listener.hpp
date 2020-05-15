//
// Project: dsn
//
// Description: Listener class for handling incoming requests
//

#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <algorithm>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

// Serverside libraries
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 



class Listener {
public:
  Listener();
  Listener(const int server_port);
  ~Listener();
  void run();

private:
  // 1111 is default port if not assigned, assigning is reccommended
  // to prevent errors using alreadly allocated ports
  int server_port_ = 1111;
  std::ofstream log_file_;

};

#endif // LISTENER_HPP
