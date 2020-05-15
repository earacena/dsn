//
// Project: dsn
//

#include "Requester.hpp"

Requester::Requester() {  }

// Helper

bool valid_reply_form(const std::string & response);


bool valid_transmit_form(const std::string & response);



void Requester::run(const Request & request) {
  std::cout << "[Requester] Attempting request (addr: " << request.target_address << " port: "
            << request.target_port << " type: " << request.type << ")..." << std::endl;

  // Prepare socket connection 
  int sock = 0;
  // int value = 0;
  std::string response = "";

  struct sockaddr_in serv_addr;

  const int client_buf_size = 10;
  char client_buf[client_buf_size] = {0};

  int server_buf_size = 100;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    std::cout << "[Requester] Socket creation error. Exiting..." << std::endl;
    return;
  }
   
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(request.target_port);

  if (inet_pton(AF_INET, request.target_address.c_str(), &serv_addr.sin_addr) <= 0) {
    std::cout << "[Requester] Invalid address/ not supported. Exiting..." << std::endl;
    return;
  }

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    std::cout << "\n[Requester] Connection failed. Exiting..." << std::endl;
    return;
  }

  // Prepare messages to send to target node
  std::string buffer_size_message = std::string("$") + std::to_string(client_buf_size) + "_";
  const char * size_message = buffer_size_message.c_str();
 
  std::cout << "[Requester] Sending buffer size message to node(" << request.target_address << ", " 
            << request.target_port << "): " << buffer_size_message << std::endl;

  send(sock, size_message, strlen(size_message), 0);

  // Wait for response
  int msg_size = 0;
  for (char & ch : client_buf)
    ch = 0;

  msg_size = recv(sock, client_buf, client_buf_size, 0);
  response = client_buf;
  response = response.substr(0, msg_size);

  std::cout << "[Requester] node (" << request.target_address << ", " << request.target_port 
            << ") reply: " << response << std::endl;

  // If not in reply format
  if (!valid_reply_form(response)) {
    std::cout << "[Requester] Invalid reply format. Exiting request thread..." << std::endl;
    return;
  }

  // Reply will contain server buffer size, strip relevant part
  server_buf_size = std::stoi(response.substr(1, response.find('_')-1));

  if (request.type == "fat_distrib") {
    std::cout << "\n[Requester] Distributing FAT to node (" << request.target_address << ", " 
              << request.target_port << ")..." << std::endl;
    std::string data = "";

    for (std::string line : request.fat_copy) {
      data = data + line.substr(0, line.size()-1) + '*';
    }
 
    // std::cout << "[debug] string data: " << data << std::endl;

    // Chunk the data
    if ((int) data.length()+2 > server_buf_size) {
      std::vector<std::string> chunks;
      chunks.reserve(data.size()/server_buf_size-2);
      int chunk_size = server_buf_size-2;
      for (size_t i = 0; i < data.length(); i += chunk_size) 
      chunks.push_back(data.substr(i, chunk_size));
  
      std::string symbol = "^";
 
      // Conform to reply format
      for (size_t i = 0; i < chunks.size(); ++i) {
        if (i == 0)
          chunks[0] = symbol + chunks[0] + "&";
        // last element
        else if (i == chunks.size()-1)
          chunks[i] = std::string("&") + chunks[i] + "_";
        else
          chunks[i] = std::string("&") + chunks[i] + "&";
      }

      // debug
      std::cout << "Data: " << data;
      for (std::string & chunk : chunks)
        std::cout << "\n\tChunk: " << chunk;
  
      std::cout << std::endl;
   

      // send
      for (std::string & chunk : chunks) 
        send(sock, chunk.c_str(), strlen(chunk.c_str()), 0);

    } else {
      std::string block = std::string("^") + data + "_";
      block = block.substr(0, server_buf_size);
      std::cout << "[Requester] Sending data:" << block << std::endl; 
      send(sock, block.c_str(), strlen(block.c_str()), 0);
    }

    std::cout << "[Requester] FAT transmission complete. Exiting..." << std::endl;
 
  } else if (request.type == "nodes_distrib") {
    std::cout << "\n[Requester] Distributing nodes.txt to node (" 
              << request.target_address << ", " << request.target_port << ")..." << std::endl;
    std::string data = "";

    for (std::string line : request.nodes_copy) {
      data = data + line + '*';
    }
 
    // std::cout << "[debug] string data: " << data << std::endl;

    // Chunk the data
    if ((int) data.length()+2 > server_buf_size) {
      std::vector<std::string> chunks;
      chunks.reserve(data.size()/server_buf_size-2);
      size_t chunk_size = server_buf_size-2;
      for (size_t i = 0; i < data.length(); i += chunk_size) 
      chunks.push_back(data.substr(i, chunk_size));
  
      std::string symbol = "@";
 
      // Conform to reply format
      for (size_t i = 0; i < chunks.size(); ++i) {
        if (i == 0)
          chunks[0] = symbol + chunks[0] + "&";
        // last element
        else if (i == chunks.size()-1)
          chunks[i] = std::string("&") + chunks[i] + "_";
        else
          chunks[i] = std::string("&") + chunks[i] + "&";
      }

      // debug
      std::cout << "Data: " << data;
      for (std::string & chunk : chunks)
        std::cout << "\n\tChunk: " << chunk;
  
      std::cout << std::endl;
   

      // send
      for (std::string & chunk : chunks) 
        send(sock, chunk.c_str(), strlen(chunk.c_str()), 0);

    } else {
      std::string block = std::string("@") + data + "_";
      block = block.substr(0, server_buf_size);
      std::cout << "[Requester] Sending data:" << block << std::endl; 
      send(sock, block.c_str(), strlen(block.c_str()), 0);
      
    }
    std::cout << "[Requester] Nodes (nodes.txt) transmission complete. " << std::endl;

  } else if (request.type == "block_transmit") {
    std::string data = "";

    data = data + request.block_name + '*';
    for (std::string line : request.block_copy) {
      data = data + line + '*';
    }
 
    // std::cout << "[debug] string data: " << data << std::endl;

    // Chunk the data
    if ((int) data.length() + (int) request.block_name.length() > server_buf_size) {
      std::vector<std::string> chunks;
      chunks.reserve(data.size()/server_buf_size-2);
      size_t chunk_size = server_buf_size-2;
      for (size_t i = 0; i < data.length(); i += chunk_size) 
      chunks.push_back(data.substr(i, chunk_size));
  
      std::string symbol = "%";
 
      // Conform to reply format
      for (size_t i = 0; i < chunks.size(); ++i) {
        if (i == 0)
          chunks[0] = symbol + chunks[0] + "&";
        // last element
        else if (i == chunks.size()-1)
          chunks[i] = std::string("&") + chunks[i] + "_";
        else
          chunks[i] = std::string("&") + chunks[i] + "&";
      }

      // debug
      std::cout << "Data: " << data;
      for (std::string & chunk : chunks)
        std::cout << "\n\tChunk: " << chunk;
  
      std::cout << std::endl;
   

      // send
      for (std::string & chunk : chunks) 
        send(sock, chunk.c_str(), strlen(chunk.c_str()), 0);

    } else {
      std::string block = std::string("%") + data + "_";
      block = block.substr(0, server_buf_size);
      std::cout << "[Requester] Sending data:" << block << std::endl; 
      send(sock, block.c_str(), strlen(block.c_str()), 0);
      
    }
    
    std::cout << "[Requester] Block transmission complete." << std::endl;
  
  } else if (request.type == "block_fetch") {

    std::cout << request.target_block << std::endl;
    std::string block_request_message = std::string("?") + request.target_block + "_";
    std::cout << "[Requester] Sending block request message to node(" 
              << request.target_address << ", " 
              << request.target_port << "): " << block_request_message << std::endl;
    const char * request_message = block_request_message.c_str();
    send(sock, request_message, strlen(request_message), 0);
  
    for (char & ch : client_buf)
      ch = 0;

    int msg_size = recv(sock, client_buf, client_buf_size, 0);
    std::string response = client_buf;
    response = response.substr(0, msg_size);  

    std::cout << "[Requester] Message received: " << response << std::endl;

    if (!valid_transmit_form(response)) {
      std::cout << "[Requester] Invalid reply format. Exiting client thread..." << std::endl;
      return;
    }

    std::vector<std::string> chunks;
    chunks.reserve(15);

    chunks.push_back(response);

    while ((response.substr(0,1) == "&" || response.substr(response.length()-1, 1) == "&") && 
          (response.substr(response.length()-1, 1) != "_")) {
      
      for (char & ch : client_buf)
        ch = 0;
    
      msg_size = recv(sock, client_buf, client_buf_size, 0);
      response = client_buf;
      response = response.substr(0, msg_size);

      std::cout << "[Requester] Chunk received: " << response << std::endl;
      chunks.push_back(response);
    }

    // process chunks, extract data
    for (std::string & chunk : chunks)
      chunk = chunk.substr(1, chunk.length()-2);

    std::string data = "";

    for (std::string & chunk : chunks)
    data = data + chunk;

    std::cout << "[Requester] Block received: " << data << std::endl;  

  } else {
    std::cout << "[Requester] Unrecognized request type: " << request.type << " ..." << std::endl;
    return;
  }

  return;

}

