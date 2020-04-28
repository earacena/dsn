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

void run_client(const Request & request) {
  Requester requester;
  requester.run(request);
}



int main(int argc, const char **argv) {

  // Check if FAT file exists
  std::ifstream fat_file("./FAT/backup.txt");

  //if (!fat_file.is_open()) {
  //  std::cout << "[Error] FAT table file missing. Exiting..." << std::endl;
  //  return 0;
  //}

  std::vector<std::string> fat;
  fat.reserve(10);

  std::string line = "";
  // Load FAT into memory
  while(std::getline(fat_file, line))
    fat.push_back(line);
  
  // Get port, and all the nodes
  std::ofstream nodes_list("./nodes.txt");

  std::cout << "[?] What is the port of this node?: ";
  int server_port = 0;
  std::cin >> server_port;


  std::vector<std::string> addresses;
  std::vector<int> ports;

  addresses.reserve(4);
  ports.reserve(4);
  
  std::string address = "";
  int port = 0;

  while (true) {
    std::cout << "[?] Enter address for node in network, enter 0 to end: "; 
    std::cin >> address;

    if (address == "0")
      break;
    addresses.push_back(address);

    std::cout << "[?] Enter port for current node: ";
    std::cin >> port;
    ports.push_back(port);

    nodes_list << address << " " << port << std::endl;
  }

  nodes_list.close();

  // Send all nodes a copy of the FAT
  std::cout << "[?] Distribute FAT?[y/n]: ";
  std::string response = "";
  std::cin >> response;

  if (response == "y") {  
    Request send_fat_to_node2;
    Request send_fat_to_node3;
  
    send_fat_to_node2.type = "fat_distrib";
    send_fat_to_node3.type = "fat_distrib";
  
    send_fat_to_node2.target_address = addresses[0];
    send_fat_to_node3.target_address = addresses[1];
  
    send_fat_to_node2.target_port = ports[0];
    send_fat_to_node3.target_port = ports[1];
   
    send_fat_to_node2.fat_copy = fat;   
    send_fat_to_node3.fat_copy = fat;   
  
    std::thread fat_distrib_thread_1(run_client, send_fat_to_node2); 
    std::thread fat_distrib_thread_2(run_client, send_fat_to_node3); 
  
    fat_distrib_thread_1.join();
    fat_distrib_thread_2.join();
  }
  
  // Immediately start listening for requests
  std::thread listening_thread(run_server, server_port);

  listening_thread.join();
  return 0;
}

