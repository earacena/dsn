// dsn - decentralized storage network


#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

// libraries for client-side, including some above
#include <arpa/inet.h>

// Classes
#include "Requester.hpp"
#include "Listener.hpp"

void menu() {
  std::cout << "Menu:\n";
  std::cout << "\ttype \"fetch\" to collect all distributed blocks and store local file copy\n";
   

  std::cout << ">> ";
}

void run_server(const int server_port) {
  Listener listener(server_port);
  listener.run();
}

void run_client(const Request & request) {
  Requester requester;
  requester.run(request);
}

int main() {
  // Get port, and all the nodes
  std::ofstream nodes_list;
  nodes_list.open("./nodes.txt");

  std::cout << "[?] What is the port of this node?: ";
  int server_port = 0;
  std::cin >> server_port;


  std::vector<std::string> addresses;
  std::vector<int> ports;

  addresses.reserve(4);
  ports.reserve(4);
  
  int node_number = 0;
  std::string address = "";
  int port = 0;

  // Add this node as first node
  nodes_list << "0 127.0.0.1 " << server_port << std::endl;
  addresses.push_back("127.0.0.1");
  ports.push_back(server_port);

  while (true) {
    std::cout << "[?] Enter address for node in network, enter 0 to end: "; 
    std::cin >> address;

    if (address == "0")
      break;

    addresses.push_back(address);

    std::cout << "[?] Enter port for current node: ";
    std::cin >> port;
    ports.push_back(port);

    nodes_list << ++node_number << " " << address << " " << port << std::endl;
  }

  nodes_list.close();

  // Send all nodes a copy of the FAT
  std::cout << "[?] Distribute FAT?[y/n]: ";
  std::string response = "";
  std::cin >> response;

  if (response == "y") {  
    // Check if FAT file exists
    std::ifstream fat_file("./backup.txt");
    std::ifstream nodes_file("./nodes.txt");
    
    if (!fat_file.is_open()) {
      std::cout << "[Error] FAT table file missing. Exiting..." << std::endl;
      return 0;
    }
    
    if (!nodes_file.is_open()) {
      std::cout << "[Error] nodes.txt missing. Exiting..." << std::endl;
    }

    std::vector<std::string> fat;
    fat.reserve(10);

    std::string line = "";
    // Load FAT into memory
    while(std::getline(fat_file, line))
      fat.push_back(line);



    std::vector<std::string> nodes;
    nodes.reserve(5);

    // Load nodes list into memory
    line = "";
    while(std::getline(nodes_file, line))
      nodes.push_back(line);

    // Usecase 3, send FAT
    Request send_fat_to_node2;
    Request send_fat_to_node3;
  
    send_fat_to_node2.type = "fat_distrib";
    send_fat_to_node3.type = "fat_distrib";
  
    send_fat_to_node2.target_address = addresses[1];
    send_fat_to_node3.target_address = addresses[2];
  
    send_fat_to_node2.target_port = ports[1];
    send_fat_to_node3.target_port = ports[2];
   
    send_fat_to_node2.fat_copy = fat;   
    send_fat_to_node3.fat_copy = fat;   
  
    std::thread fat_distrib_thread_1(run_client, send_fat_to_node2); 
    std::thread fat_distrib_thread_2(run_client, send_fat_to_node3); 
  
    fat_distrib_thread_1.join();
    fat_distrib_thread_2.join();
    
    
    // Usecase 3, send addresses and ports with node number as an "ID"
    Request send_nodes_to_node2;
    Request send_nodes_to_node3;
  
    send_nodes_to_node2.type = "nodes_distrib";
    send_nodes_to_node3.type = "nodes_distrib";
  
    send_nodes_to_node2.target_address = addresses[1];
    send_nodes_to_node3.target_address = addresses[2];
  
    send_nodes_to_node2.target_port = ports[1];
    send_nodes_to_node3.target_port = ports[2];
   
    send_nodes_to_node2.nodes_copy = nodes;   
    send_nodes_to_node3.nodes_copy = nodes;   
  
    std::thread nodes_distrib_thread_1(run_client, send_nodes_to_node2); 
    std::thread nodes_distrib_thread_2(run_client, send_nodes_to_node3); 
  
    nodes_distrib_thread_1.join();
    nodes_distrib_thread_2.join();
        
        
    // Usecase 3, send respective blocks to every node
    Request send_block_to_node2;
    Request send_block_to_node3;
    
    send_block_to_node2.type = "block_transmit";
    send_block_to_node3.type = "block_transmit";
    
    std::string node_num = "";
    std::string filename = "";
    std::string block_num = "";
    
    // Distribute all blocks in fat
    std::thread block_distrib_thread_1;
    std::thread block_distrib_thread_2;
    
    for (std::string entry : fat) {
      node_num = entry.substr(0, entry.find(" "));
      entry = entry.substr(entry.find(" ")+1, entry.length());
      
      filename = entry.substr(0, entry.find(" "));
      entry = entry.substr(entry.find(" ")+1, entry.length());
      
      block_num = entry;
      
      
      // std::cout << "1: " << node_num << std::endl;
      // std::cout << "2: " << filename << std::endl;
      // std::cout << "3: " << block_num << std::endl;
      
      // Usecase 3 specific hardcoded, two nodes, node2 and node3
      if (node_num == "1") {

        std::cout << "[Requester] Distributing block (node_num: " << node_num
                  << ", filename: " << filename << ", block_num: " << block_num
                  << ")..." <<std::endl;

      
        send_block_to_node2.target_address = addresses[1];
        send_block_to_node2.target_port = ports[1];
        send_block_to_node2.block_name = filename + "_" + node_num + "_" + block_num;
        
        // Load block into memory
        std::vector<std::string> block;
        block.reserve(5);
        filename = std::string("./storage/") + filename + "_" + node_num + "_" + block_num;
        std::ifstream block_file(filename);

        std::cout << "[Requester] Filename: \"" << filename << "\"" << std::endl;


        if (!block_file.is_open()) {
          std::cout << "[Error] Error opening file \"" << filename << "\"..." << std::endl;
          return 0;
        }
        
        line = "";
        while(std::getline(block_file, line))
          block.push_back(line);
          
        std::remove(filename.c_str()); 
        
        send_block_to_node2.block_copy = block;
        block_distrib_thread_1 = std::thread{run_client, send_block_to_node2};
      }
      
      if (node_num == "2") {
        std::cout << "[Requester] Distributing block (node_num: " << node_num
                  << ", filename: " << filename << ", block_num: " << block_num
                  << ")..." <<std::endl;

      
        send_block_to_node3.target_address = addresses[2];
        send_block_to_node3.target_port = ports[2];
        send_block_to_node3.block_name = filename + "_" + node_num + "_" + block_num;
        // Load block into memory
        std::vector<std::string> block;
        block.reserve(5);
        
        
        filename = std::string("./storage/") + filename + "_" + node_num + "_" + block_num;
        std::ifstream block_file(filename);

        std::cout << "[Requester] Filename: \"" << filename << "\"" << std::endl;

        if (!block_file.is_open()) {
          std::cout << "[Error] Error opening file \"" << filename << "\"..." << std::endl;
          return 0;
        }
        
        line = "";
        while(std::getline(block_file, line))
          block.push_back(line);
        
        std::remove(filename.c_str());
          
        send_block_to_node3.block_copy = block;
        block_distrib_thread_2 = std::thread{run_client, send_block_to_node3};
      }
      
    }
    
    block_distrib_thread_1.join();
    block_distrib_thread_2.join();
    
    
  }
  
  // Immediately start listening for requests
  std::thread listening_thread(run_server, server_port);

  // User interaction loop
  std::string user_input = "";
  menu();
  std::cin >> user_input;  

  while (user_input != "q") {
    if (user_input == "fetch") {
      std::cout << "[Main] Preparing file fetch...";
      std::string filename = "";
      std::cout << "[?] What is the name of the file?: ";
      std::cin >> filename;
      std::cout << "[Main] File \"" << filename << "\"...";
      
      // Assume any node could ask, load FAT and nodes.txt into memory  
      std::ifstream fat_file("./backup.txt");
      std::vector<std::string> fat;
      
      std::string line = "";
      while(std::getline(fat_file, line))
        fat.push_back(line);
        
      
      std::ifstream nodes_file("./nodes.txt");
      std::vector<std::string> nodes;
      nodes.reserve(5);
      
      while(std::getline(nodes_file, line))
        nodes.push_back(line);

      std::vector<std::string> addresses;
      addresses.reserve(5);
      std::vector<int> ports;
      ports.reserve(5);
      
      std::string temp = "";
      std::string address = "";
      std::string port = "";
      
      for (std::string line : nodes) {
        temp = line.substr(0, line.find(" "));
        line = line.substr(line.find(" ")+1, line.length());
        
        address = line.substr(0, line.find(" "));
        line = line.substr(line.find(" ")+1, line.length());
        
        port = line;
        
        addresses.push_back(address);
        ports.push_back(std::stoi(port));
        
      }
      

      std::string node_num = "";
      std::string entry_filename = "";
      std::string block_num = "";
                  
      for (std::string entry : fat) {
        node_num = entry.substr(0, entry.find(" "));
        entry = entry.substr(entry.find(" ")+1, entry.length());
        
        entry_filename = entry.substr(0, entry.find(" "));
        entry = entry.substr(entry.find(" ")+1, entry.length());
        
        block_num = entry;
        
        if (entry_filename == filename) {
          
          std::fstream block("./storage/" + entry_filename + "_" + node_num + "_" + block_num);
          // If block is not in storage folder
          if (block.fail()) {
            // Retrieve block
            Request block_fetch;
            
            block_fetch.type = "block_fetch";
            block_fetch.target_address = addresses[std::stoi(node_num)];
            block_fetch.target_port = ports[std::stoi(node_num)];
            
            block_fetch.target_block = entry_filename + "_" + node_num + "_" + block_num;
            std::thread block_fetch_thread(run_client, block_fetch);
            
            block_fetch_thread.join();
          } else {
            block.close();
          }
          
        }
      
      }
      
      
      
    } else {
      std::cout << "[ERROR] Unknown command: \"" << user_input << "\"...\n"; 
    }
    
    menu();
    std::cin >> user_input;
  
  }




  listening_thread.join();
  return 0;
}

