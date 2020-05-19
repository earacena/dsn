//
// Project: dsn
//
//
//

#include "Listener.hpp"

Listener::Listener() {}

Listener::Listener(const int server_port) {
  log_file_.open("log.txt");
  server_port_ = server_port;
}

Listener::~Listener() { log_file_.close(); }

// Helper

bool valid_reply_form(const std::string &response) {
  // >..._
  std::cout << "first: " << response.substr(0, 1) << "." << std::endl;
  std::cout << "second:" << response.substr(response.length() - 1, 1) << "."
            << std::endl;
  return (response.substr(0, 1) == ">" &&
          response.substr(response.length() - 1, 1) == "_");
}

bool valid_transmit_form(const std::string &response) {
  // One of three choices representing a stage
  // >...&    first block in transmission
  // &...&    second block and onwards in transmission
  // &..._    last block in transmission
  // transmit form must satisfy one of these

  if (response == ">badblock_")
    return false;

  std::cout << "[Client] first: " << response.substr(0, 1) << "." << std::endl;
  std::cout << "[Client] second: " << response.substr(response.length() - 1, 1)
            << "." << std::endl;

  bool one_chunk = (response.substr(0, 1) == ">" &&
                    response.substr(response.size() - 1, 1) == "_");
  bool first_chunk = (response.substr(0, 1) == ">" &&
                      response.substr(response.size() - 1, 1) == "&");
  bool chunk_n = (response.substr(0, 1) == "&" &&
                  response.substr(response.size() - 1, 1) == "&");
  bool last_chunk = (response.substr(0, 1) == "&" &&
                     response.substr(response.size() - 1, 1) == "_");

  std::cout << "[Client] transmit form [" << response << "]: "
            << ((one_chunk || first_chunk || chunk_n || last_chunk) ? "good"
                                                                    : "bad")
            << std::endl;

  return (one_chunk || first_chunk || chunk_n || last_chunk);
}

void Listener::run() {

  log_file_ << "[Listener] Initializing server..." << std::endl;

  int server_fd;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);

  const int server_buf_size = 100;
  char server_buf[server_buf_size] = {0};
  //int client_buf_size = 100;

  // Create socket file descriptor (master socket)
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  // Attach socket to given port
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(server_port_);

  // bind master socket to port
  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }

  log_file_ << "[Listener] Waiting for requests..." << std::endl;

  // listen on port
  if (listen(server_fd, 5) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  std::string response = "";

  // Create socket set, monitered using poll call
  struct pollfd pfds[10];
  int fd_count = 0;
  
  // Add listener socket (server_fd) to set
  pfds[0].fd = server_fd;
  pfds[0].events = POLLIN;
  ++fd_count;
  
  int poll_count = 0;
  int new_fd = 0;
  
  while (true) {
    if((poll_count = poll(pfds, fd_count, -1)) == -1) {
      perror("poll");
      exit(1);
    }
    
    
    // Go through existing connections/fd
    for (int i = 0; i < fd_count; ++i) {
      if (pfds[i].revents & POLLIN) {
	if (pfds[i].fd == server_fd) {
	  // listener is ready to handle new connection
	  addrlen = sizeof(address);
	  new_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
	  
	  if (new_fd == -1) {
	    perror("accept");
	  } else {
	    pfds[fd_count].fd = new_fd;
	    pfds[fd_count].events = POLLIN;
	    ++fd_count;
	    
	    log_file_ << "[Listener] New connection, new fd" << new_fd << std::endl;
	  }
	} else {
	  int val = recv(pfds[i].fd, server_buf, server_buf_size, 0);
	  if (val <= 0) {
	      if (val == 0)
		log_file_ << "[Listener] Connection terminated by remote host." << std::endl;
	      else
		perror("recv");
		
	      close(pfds[i].fd);
	      pfds[i] = pfds[fd_count-1];
	      --fd_count;
	  } else {
	    process(server_buf, server_buf_size, val, pfds[i].fd);
	  }
	}
      }
    }
    
    
  }
  
}

void Listener::process(char (&server_buf)[100], int server_buf_size, int value, int sock) {
  std::string response = "";
  int client_buf_size = 0;

  while (true) {

    log_file_ << "[Listener] Message [" << value << "]: " << server_buf
              << std::endl;
    response = server_buf;
    response = response.substr(0, value);

    // Two-way buffer size handshake
    if (response.substr(0, 1) == "$") {
      client_buf_size = std::stoi(response.substr(1, response.find('_') - 1));

      std::string buffer_size_message =
          std::string(">") + std::to_string(server_buf_size) + "_";
      const char *size_message = buffer_size_message.c_str();

      log_file_ << "[Listener] Sending buffer size message to node: "
                << buffer_size_message << std::endl;

      send(sock, size_message, strlen(size_message), 0);
    }

    // Receiving FAT
    if (response.substr(0, 1) == "^") {
      // Multi-chunk transmission
      if (response.substr(response.length() - 1, 1) == "&") {
        for (char & ch : server_buf)
          ch = 0;

        int msg_size = recv(sock, server_buf, server_buf_size, 0);
        response = server_buf;
        response = response.substr(0, msg_size);

        log_file_ << "[Listener] Message received: " << response << std::endl;

        if (!valid_transmit_form(response)) {
          log_file_
              << "[Listener] Invalid reply format. Exiting listening thread..."
              << std::endl;
          return;
        }

        std::vector<std::string> chunks;
        chunks.reserve(15);

        chunks.push_back(response);

        while ((response.substr(0, 1) == "&" ||
                response.substr(response.length() - 1, 1) == "&") &&
               (response.substr(response.length() - 1, 1) != "_")) {
          for (char & ch : server_buf)
            ch = 0;

          msg_size = recv(sock, server_buf, server_buf_size, 0);
          response = server_buf;
          response = response.substr(0, msg_size);

          log_file_ << "[Listener] Chunk received: " << response << std::endl;
          chunks.push_back(response);
        }

        // process chunks, extract data
        for (std::string &chunk : chunks)
          chunk = chunk.substr(1, chunk.length() - 2);

        std::string data = "";

        for (std::string & chunk : chunks)
          data = data + chunk;

        log_file_ << "[Listener] Combined FAT chunks received: " << data
                  << std::endl;

        // swap '*' for newlines
        std::vector<std::string> fat;
        fat.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          fat.push_back(line + '\n');
        }

        log_file_ << "[Listener] FAT: \n";
        for (std::string &line : fat)
          log_file_ << line;

        std::ofstream fat_file("./backup.txt");
        for (std::string &line : fat)
          fat_file << line;

        fat_file.close();

        log_file_ << "[Listener] FAT stored (\"./backup.txt\")." << std::endl;

      } else {

        // single chunk transmission
        // swap '*' for newlines
        std::vector<std::string> fat;
        fat.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          fat.push_back(line + '\n');
        }
        log_file_ << "[Listener] FAT: \n";
        for (std::string &line : fat)
          log_file_ << line;

        std::ofstream fat_file("./backup.txt");
        for (std::string & line : fat)
          fat_file << line;

        fat_file.close();

        log_file_ << "[Listener] FAT stored (\"./backup.txt\")." << std::endl;
      }

      break;
    }

    // nodes.txt transmission
    if (response.substr(0, 1) == "@") {
      // Multi-chunk transmission
      if (response.substr(response.length() - 1, 1) == "&") {
        for (char &ch : server_buf)
          ch = 0;

        int msg_size = recv(sock, server_buf, server_buf_size, 0);
        response = server_buf;
        response = response.substr(0, msg_size);

        log_file_ << "[Listener] Message received: " << response << std::endl;

        if (!valid_transmit_form(response)) {
          log_file_
              << "[Listener] Invalid reply format. Exiting listening thread..."
              << std::endl;
          return;
        }

        std::vector<std::string> chunks;
        chunks.reserve(15);

        chunks.push_back(response);

        while ((response.substr(0, 1) == "&" ||
                response.substr(response.length() - 1, 1) == "&") &&
               (response.substr(response.length() - 1, 1) != "_")) {
          for (char &ch : server_buf)
            ch = 0;

          msg_size = recv(sock, server_buf, server_buf_size, 0);
          response = server_buf;
          response = response.substr(0, msg_size);

          log_file_ << "[Listener] Chunk received: " << response << std::endl;
          chunks.push_back(response);
        }

        // process chunks, extract data
        for (std::string &chunk : chunks)
          chunk = chunk.substr(1, chunk.length() - 2);

        std::string data = "";

        for (std::string &chunk : chunks)
          data = data + chunk;

        log_file_ << "[Listener] Combined nodes.txt chunks received: " << data
                  << std::endl;

        // swap '*' for newlines
        std::vector<std::string> nodes;
        nodes.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          nodes.push_back(line + '\n');
        }

        log_file_ << "[Listener] nodes.txt: \n";
        for (std::string &line : nodes)
          log_file_ << line;

        std::ofstream nodes_file("./nodes.txt");
        for (std::string &line : nodes)
          nodes_file << line;

        nodes_file.close();

        log_file_ << "[Listener] nodes.txt stored (\"./nodes.txt\")."
                  << std::endl;

      } else {

        // single chunk transmission
        // swap '*' for newlines
        std::vector<std::string> nodes;
        nodes.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          nodes.push_back(line + '\n');
        }

        log_file_ << "[Listener] nodes.txt: \n";
        for (std::string &line : nodes)
          log_file_ << line;

        std::ofstream nodes_file("./nodes.txt");
        for (std::string &line : nodes)
          nodes_file << line;

        nodes_file.close();

        log_file_ << "[Listener] nodes.txt stored (\"./nodes.txt\")."
                  << std::endl;
      }

      break;
    }

    if (response.substr(0, 1) == "%") {
      // Multi-chunk transmission
      if (response.substr(response.length() - 1, 1) == "&") {
        for (char &ch : server_buf)
          ch = 0;
        int msg_size = recv(sock, server_buf, server_buf_size, 0);
        response = server_buf;
        response = response.substr(0, msg_size);

        log_file_ << "[Listener] Message received: " << response << std::endl;

        if (!valid_transmit_form(response)) {
          log_file_
              << "[Listener] Invalid reply format. Exiting listening thread..."
              << std::endl;
          return;
        }

        std::vector<std::string> chunks;
        chunks.reserve(15);

        chunks.push_back(response);

        while ((response.substr(0, 1) == "&" ||
                response.substr(response.length() - 1, 1) == "&") &&
               (response.substr(response.length() - 1, 1) != "_")) {
          for (char &ch : server_buf)
            ch = 0;
          msg_size = recv(sock, server_buf, server_buf_size, 0);
          response = server_buf;
          response = response.substr(0, msg_size);

          log_file_ << "[Listener] Chunk received: " << response << std::endl;
          chunks.push_back(response);
        }

        // process chunks, extract data
        for (std::string &chunk : chunks)
          chunk = chunk.substr(1, chunk.length() - 2);

        std::string data = "";

        for (std::string &chunk : chunks)
          data = data + chunk;

        log_file_ << "[Listener] Combined block chunks received: " << data
                  << std::endl;

        // swap '*' for newlines
        std::vector<std::string> block;
        block.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          block.push_back(line + '\n');
        }

        log_file_ << "[Listener] Block data: \n";
        for (std::string &line : block)
          log_file_ << line;

        std::string filename =
            "./storage/" + block[0].substr(0, block[0].length() - 1);

        filename.erase(std::remove(filename.begin(), filename.end(), ' '),
                       filename.end());

        std::ofstream block_file(filename);
        for (size_t i = 1; i < block.size(); ++i)
          block_file << block[i];

        block_file.close();

        log_file_ << "[Listener] block stored (\"" << filename << "\")."
                  << std::endl;

      } else {

        // single chunk transmission
        // swap '*' for newlines
        std::vector<std::string> block;
        block.reserve(10);

        response = response.substr(1, response.length() - 2);
        std::istringstream sstr(response);
        std::string line;
        while (std::getline(sstr, line, '*')) {
          block.push_back(line + '\n');
        }

        log_file_ << "[Listener] Block data: \n";
        for (std::string &line : block)
          log_file_ << line;

        std::string filename =
            "./storage/" + block[0].substr(0, block[0].length() - 1);

        std::ofstream block_file(filename);
        for (size_t i = 1; i < block.size(); ++i)
          block_file << block[i];

        block_file.close();

        log_file_ << "[Listener] block stored (\"" << filename << "\")."
                  << std::endl;
      }

      break;
    }

    // Block fetch
    if (response.substr(0, 1) == "?") {
      std::string requested = response.substr(1, response.length() - 2);
      log_file_ << "[Listener] Requested block name/hash: " << requested
                << std::endl;
      std::string filename = "./storage/" + requested;
      log_file_ << "[Listener] Potential filename: " << filename << std::endl;
      std::ifstream file(filename);

      if (!file.good()) {
        log_file_ << "[Listener] Requested block not found." << std::endl;
        std::string not_found_message_str = ">badblock_";
        send(sock, not_found_message_str.c_str(),
             strlen(not_found_message_str.c_str()), 0);
        return;
      }

      std::string data = "";
      if (!file.is_open()) {
        log_file_ << "[Listener] Error opening file: " << requested
                  << std::endl;
      } else {
        std::string current = "";
        std::string data = "";
        // Do not ignore whitespace
        while (std::getline(file, current))
          data = data + current + '\n';

        if ((int)data.length() + 1 > client_buf_size) {
          // Split block into chunks of client buffer size (minus two for the
          // >..._ reply form and terminating underscore)
          std::vector<std::string> chunks;
          chunks.reserve(data.size() / (client_buf_size - 2));
          size_t chunk_size = client_buf_size - 2;
          for (size_t i = 0; i < data.length(); i += chunk_size) {
            chunks.push_back(data.substr(i, chunk_size));
          }

          // Transmission
          // Conform to reply format
          for (size_t i = 0; i < chunks.size(); ++i) {
            if (i == 0)
              chunks[0] = std::string(">") + chunks[0] + "&";
            // last element
            else if (i == chunks.size() - 1)
              chunks[i] = std::string("&") + chunks[i] + "_";
            else
              chunks[i] = std::string("&") + chunks[i] + "&";
          }
          // for debug
          log_file_ << "Data: " << data << std::endl;
          for (std::string &chunk : chunks)
            log_file_ << "\tChunk: " << chunk << std::endl;

          // send
          for (std::string &chunk : chunks) {
            send(sock, chunk.c_str(), strlen(chunk.c_str()), 0);
          }

        } else {
          // Send message in single transmission
          std::string block = std::string(">") + data + "_";
          block = block.substr(0, client_buf_size);
          log_file_ << "[Listener] Sending data:" << block << std::endl;
          send(sock, block.c_str(), strlen(block.c_str()), 0);
        }
      }

      file.close();
      break;
    }
    
    if ((value = recv(sock, server_buf, server_buf_size, 0)) == 0) {
      log_file_ << "[Listener] Connection terminated." << std::endl;
      exit(0);
    }
  }
}
