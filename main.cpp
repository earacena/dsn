// dsn - decentralized storage network


#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>

// libraries for client-side, including some above
#include <arpa/inet.h>

// Classes
#include "Requester.hpp"
#include "Listener.hpp"


void run_server(const int server_port) {
  Listener listener(server_port);
  listener.run();
}

void run_client(const std::string & target_address, int target_port, const std::string & target_block) {
  Requester requester;
  requester.run(target_address, target_port, target_block);
}



int main(int argc, const char **argv) {

  if (argc < 4) {
    std::cout << "proper usage: ./node [PORT] [TARGET ADDRESS] [TARGET PORT] [TARGET BLOCK]" << std::endl;
    std::cout << "example: ./node 3333 127.0.0.1 2222 3" << std::endl;
    std::cout << "\n\nexample scenario:\n\tterminal 1: ./node 3333 127.0.0.1 2222 -1\n\tterminal 2: "
              << "./node 2222 127.0.0.1 3333 3" << std::endl;
    return 0;
  }

  int server_port = std::stoi(argv[1]);
  std::string target_address = argv[2];
  int target_port = std::stoi(argv[3]);
  std::string target_block = argv[4];


  std::thread listening_thread(run_server, server_port);
  std::thread request_thread(run_client, target_address, target_port, target_block);


  listening_thread.join();
  request_thread.join();
  return 0;
}

