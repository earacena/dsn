//
// Project: dsn
//
// Description: Requester handles connections and requests to other nodes
//

#ifndef REQUESTER_HPP
#define REQUESTER_HPP

#include <algorithm>
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
  
  // type: nodes_distrib
  std::vector<std::string> nodes_copy;

  // type: block_transmit, used for sending blocks to other nodes (transmit)
  std::string block_name = "";
  std::vector<std::string> block_copy;
  
  // type: blockchain_distrib
  std::vector<std::string> blockchain_copy;

  // type: block_fetch, used for fetching single blocks (fetch)
  std::string target_block = "";
};

class Requester {
public:
  Requester();
  void run(const Request & request);

private:

  std::ofstream log_file_;
};


#endif // REQUESTOR_HPP
