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
  int sock = 0;
  int value = 0;
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

  std::cout << "[Requester] node reply: " << response << std::endl;

  // If not in reply format
  if (!valid_reply_form(response)) {
    std::cout << "[Requester] Invalid reply format. Exiting request thread..." << std::endl;
    return;
  }

  // Reply will contain server buffer size, strip relevant part
  server_buf_size = std::stoi(response.substr(1, response.find('_')-1));

  //std::cout << target_block << std::endl;
  //std::string block_request_message = std::string("?") + target_block + "_";
  //std::cout << "[Client] Sending block request message to node(" << target_address << ", " 
  //          << target_port << "): " << block_request_message << std::endl;
  //const char * request_message = block_request_message.c_str();
  //send(sock, request_message, strlen(request_message), 0);


  if (request.type == "fat_distrib") {
    std::string data = "";

    for (std::string entry : request.fat_copy)
      data = data + entry + '\n';
    
    // Chunk the data
     
    if (data.length()+1 > server_buf_size) {
      // Split block into chunks of client buffer size (minus two for the >..._ reply form and 
      // terminating underscore)
      std::vector<std::string> chunks;
      chunks.reserve(data.size()/server_buf_size-2);
      int chunk_size = server_buf_size-2;
      for (int i = 0; i < data.length(); i += chunk_size) {
        chunks.push_back(data.substr(i, chunk_size));
      }
    
    
      // Transmission
      // Conform to reply format
      for (int i = 0; i < chunks.size(); ++i) {
        if (i == 0)
          chunks[0] = std::string("^") + chunks[0] + "&";
        // last element
        else if (i == chunks.size()-1)
          chunks[i] = std::string("&") + chunks[i] + "_";
        else
          chunks[i] = std::string("&") + chunks[i] + "&";
      }
      // for debug
      std::cout << "Data: " << data;
      for (std::string & chunk : chunks)
        std::cout << "\tChunk: " << chunk << std::endl;
    
      // send
      for (std::string & chunk : chunks) {
        send(sock, chunk.c_str(), strlen(chunk.c_str()), 0);
      }

    }

    std::cout << "[Requester] FAT transmission complete. Exiting..." << std::endl;
  
  } else {
    std::cout << "[Requester] Unrecognized request type: " << request.type << " ..." << std::endl;
    return;
  }
  
  
  //for (char & ch : client_buf)
  //  ch = 0;
  //msg_size = recv(sock, client_buf, client_buf_size, 0);
  //response = client_buf;
  //response = response.substr(0, msg_size);  

  //std::cout << "[Client] Message received: " << response << std::endl;

  //if (!valid_transmit_form(response)) {
  //  std::cout << "[Client] Invalid reply format. Exiting client thread..." << std::endl;
  //  return;
  //}

  //std::vector<std::string> chunks;
  //chunks.reserve(15);

  //chunks.push_back(response);

  //while ((response.substr(0,1) == "&" || response.substr(response.length()-1, 1) == "&") && 
  //       (response.substr(response.length()-1, 1) != "_")) {
  //  for (char & ch : client_buf)
  //    ch = 0;
  //  msg_size = recv(sock, client_buf, client_buf_size, 0);
  //  response = client_buf;
  //  response = response.substr(0, msg_size);

  //  std::cout << "[Client] Chunk received: " << response << std::endl;
  //  chunks.push_back(response);
  //}

  //// process chunks, extract data
  //for (std::string & chunk : chunks)
  //  chunk = chunk.substr(1, chunk.length()-2);

  //std::string data = "";

  //for (std::string & chunk : chunks)
  //  data = data + chunk;

  //std::cout << "[Client] Block received: " << data << std::endl;

  return;

}
