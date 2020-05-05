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
  std::string target_block = "";
};

class Requester {
public:
  Requester();

  void run(const Request & request);

private:
  std::string receive_block(int sock, char client_buf[], int client_buf_size);
  std::vector<std::string> chunkify_data(const std::string & data, int server_buf_size, 
                                         const std::string & mode);

};


#endif // REQUESTOR_HPP
