//
// Project: dsn
//

#include "Requester.hpp"

Requester::Requester() {  }

// Helper

bool valid_reply_form(const std::string & response);


bool valid_transmit_form(const std::string & response);



void Requester::run(const std::string & target_address, const int target_port, 
                    const std::string & target_block) {

  int sock = 0;
  int value = 0;
  std::string response = "";

  struct sockaddr_in serv_addr;

  const int client_buf_size = 10;
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
  std::string buffer_size_message = std::string("$") + std::to_string(client_buf_size) + "_";
  const char * size_message = buffer_size_message.c_str();
 
  std::cout << "[Client] Sending buffer size message to node(" << target_address << ", " 
            << target_port << "): " << buffer_size_message << std::endl;

  send(sock, size_message, strlen(size_message), 0);

  // Wait for response
  int msg_size = 0;
  for (char & ch : client_buf)
    ch = 0;

  msg_size = recv(sock, client_buf, client_buf_size, 0);
  response = client_buf;
  response = response.substr(0, msg_size);

  std::cout << "[Client] node reply: " << response << std::endl;

  // If not in reply format
  if (!valid_reply_form(response)) {
    std::cout << "[Client] Invalid reply format. Exiting client thread..." << std::endl;
    return;
  }

  // Reply will contain server buffer size, strip relevant part
  server_buf_size = std::stoi(response.substr(1, response.find('_')-1));

  std::cout << target_block << std::endl;
  std::string block_request_message = std::string("?") + target_block + "_";
  std::cout << "[Client] Sending block request message to node(" << target_address << ", " 
            << target_port << "): " << block_request_message << std::endl;
  const char * request_message = block_request_message.c_str();
  send(sock, request_message, strlen(request_message), 0);


  
  for (char & ch : client_buf)
    ch = 0;
  msg_size = recv(sock, client_buf, client_buf_size, 0);
  response = client_buf;
  response = response.substr(0, msg_size);  

  std::cout << "[Client] Message received: " << response << std::endl;

  if (!valid_transmit_form(response)) {
    std::cout << "[Client] Invalid reply format. Exiting client thread..." << std::endl;
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

    std::cout << "[Client] Chunk received: " << response << std::endl;
    chunks.push_back(response);
  }

  // process chunks, extract data
  for (std::string & chunk : chunks)
    chunk = chunk.substr(1, chunk.length()-2);

  std::string data = "";

  for (std::string & chunk : chunks)
    data = data + chunk;

  std::cout << "[Client] Block received: " << data << std::endl;


  std::cout << "[Client] Done, exiting..." << std::endl;
  return;

}
