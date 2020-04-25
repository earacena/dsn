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

#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 


class Requester {
public:
  Requester();

  void run(const std::string & target_address, const int target_port, const std::string & target_block);

private:


};


#endif // REQUESTOR_HPP
