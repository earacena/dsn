//
// Project: dsn
//
// Description: Requester handles connections and requests to other nodes
//

#ifndef REQUESTER_HPP
#define REQUESTER_HPP

#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 

struct Request {
  std::string type = "";
  
  std::string target_address = "";
  int target_port = 0;

  // type: fat_distrib
  std::vector<std::string> fat_copy;

  // type: block_distrib/transmission

};

class Requester {
public:
  Requester();

  void run(const Request & request);

private:


};


#endif // REQUESTOR_HPP
