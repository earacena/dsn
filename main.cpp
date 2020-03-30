// dsn - decentralized storage network


#include <iostream>
#include <thread>
#include <fstream>
#include <string>

// libraries for server-side
#include <unistd.h>
#include <cstdio>
#include <sys/socket.h>
#include <cstdlib>
#include <netinet/in.h>
#include <cstring>

// libraries for client-side, including some above
#include <arpa/inet.h>

void run_server(const int server_port) {
  int server_fd, sock, value;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  
  const int server_buf_size = 100;
  char server_buf[server_buf_size] = {0};
  int client_buf_size = 100;

  // Create socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to given port
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(server_port);
  
  // bind socket to port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  // listen on port
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::string response = "";

  while (true) {
    // accept connections
    sock = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (sock < 0) {
      perror("accept");
      exit(EXIT_FAILURE);
    }

    while (true) {
      value = recv(sock, server_buf, server_buf_size, 0);
      std::cout << "[Listener] value read from socket [" << value << "]: " << server_buf << std::endl;
      response = server_buf;
  
      if (response.substr(0,1) == "$") {
        client_buf_size = std::stoi(response.substr(1));
        
  
        std::string buffer_size_message = std::string(">") + std::to_string(server_buf_size) + " ";
        const char * size_message = buffer_size_message.c_str();
   
        std::cout << "[Listener] Sending buffer size message to node: " << buffer_size_message << std::endl;
  
        send(sock, size_message, strlen(size_message), 0);
      }
  
      if (response.substr(0,1) == "?") {
        std::string requested = response.substr(1, response.find(' ')-1);
        std::cout << "[Listener] Requested block name/hash: " << requested << std::endl;
        std::string filename = "./" + requested;
        std::ifstream file(filename);
  
        if (!file.good()) {
          std::cout << "[Listener] Requested block not found." << std::endl;       
          char * not_found_message = ">badblock";
          send(sock, not_found_message, strlen(not_found_message), 0);
          return;
        }
    
        std::string data = "";
        if (!file.is_open()) {
          std::cout << "[Listener] Error opening file." << std::endl;
        } else {
            std::string b = "";
          while (file >> b)
            data = data + b;
  
          std::string block = std::string(">") + data;
          block = block.substr(0, client_buf_size);
          std::cout << "[Listener] Sending data:" << block << std::endl; 
          send(sock, block.c_str(), strlen(block.c_str()), 0);
        }
  
        file.close();
  
      }
    }
  
  }
}

void run_client(const std::string & target_address, int target_port, const std::string & target_block) {
  int sock = 0;
  int value = 0;
  std::string response = "";

  struct sockaddr_in serv_addr;

  const int client_buf_size = 100;
  char client_buf[client_buf_size] = {0};

  int server_buf_size = 100;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cout << "[Client] Socket creation error" << std::endl;
    return;
  }
   
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(target_port);

  if (inet_pton(AF_INET, target_address.c_str(), &serv_addr.sin_addr) <= 0) {
    std::cout << "[Client] Invalid address/ not supported." << std::endl;
    return;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cout << "[Client] Connection failed." << std::endl;
    return;
  }

  // Prepare messages to send to target node
  std::string buffer_size_message = std::string("$") + std::to_string(client_buf_size) + " ";
  const char * size_message = buffer_size_message.c_str();
 
  std::cout << "[Client] Sending buffer size message to node(" << target_address << ", " 
            << target_port << "): " << buffer_size_message << std::endl;

  send(sock, size_message, strlen(size_message), 0);

  // Wait for response
  recv(sock, client_buf, client_buf_size, 0);
  response = client_buf;

  std::cout << "[Client] node reply: " << response << std::endl;

  // If not in reply format
  if (response.substr(0,1) != ">") {
    std::cout << "[Client] Invalid reply format. Exiting client thread..." << std::endl;
    return;
  }

  // Reply will contain server buffer size, strip relevant part
  server_buf_size = std::stoi(response.substr(1));

  std::cout << target_block << std::endl;
  std::string block_request_message = std::string("?") + target_block + " ";
  std::cout << "[Client] Sending block request message to node(" << target_address << ", " 
            << target_port << "): " << block_request_message << std::endl;
  const char * request_message = block_request_message.c_str();
  send(sock, request_message, strlen(request_message), 0);

  read(sock, client_buf, client_buf_size);
  response = client_buf;

  if (response.substr(0,1) != ">") {
    std::cout << "[Client] Invalid reply format. Exiting client thread..." << std::endl;
    return;
  }

  std::cout << "[Client] Block received: " << response.substr(1) << std::endl;


  std::cout << "[Client] Done, exiting..." << std::endl;
  return;
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

