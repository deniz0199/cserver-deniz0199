#include "server.h"

int port = 8080;
std::string dir = "../site";

std::string getFileType(std::string& file_path ) {
     if (file_path.find(".html") != std::string::npos) {
        return "text/html";
    } else if (file_path.find(".css") != std::string::npos) {
        return "text/css";
    } else {
        return "text/plain; charset=utf-8";
    }
}

int initiliazeSocket() {
    //opens a new socket
    int server_socket = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server_address;

    return server_socket;
}

void listenConnection(int server_socket) {
    //listens to the socket
    if (listen(server_socket, 10) < 0) {
    printf("Oops. The server is not listening.\n");
    close(server_socket);
  }
}

int main() {
    int server_socket = initiliazeSocket();

    struct sockaddr_in server_address;
    //configures a server address in ipv4, port 8080, allows any connections
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);                  
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //binds the server socket to the server address
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
    printf("Oops. The server has not been bound to this address.\n");
    close(server_socket);
    }
    
    listenConnection(server_socket);

    while (1) {
        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        //attempts to accept connections
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_size);

        if (client_socket < 0) {
            printf("Oops. The connection was not accepted.");
            continue;
        }

        char buffer[1024];
        ssize_t data_received = recv(client_socket, buffer, sizeof(buffer), 0);

        //attempts to receive data
        if (data_received < 0) {
            printf("Oops. Data from client has not been read");
            close(client_socket);
            continue;
        }

        std::string request(buffer, data_received);
        std::string requested_file;
        if (request.find("GET") != std::string::npos) {
            size_t start = request.find("GET") + 4;
            size_t end = request.find("HTTP") - 1;
            requested_file = request.substr(start, end - start);
        }

        std::string file_path = dir + requested_file;


        // stores the file that will be sent in a stream
        std::ifstream stream_file(file_path.c_str(), std::ios::in | std::ios::binary);
        std::stringstream stream_response;

        if (stream_file) {
            stream_response << "HTTP/1.1 200 OK\r\n";
            stream_file.seekg(0, std::ios::end);
            size_t size_of_file = stream_file.tellg();
            stream_file.seekg(0, std::ios::beg);
            stream_response << "Content-Length: " << size_of_file << "\r\n";
            stream_response << "Content-Type: " << getFileType(requested_file) << "\r\n\r\n";
            stream_response << stream_file.rdbuf();
        } else {
            stream_response << "HTTP/1.1 404 Not Found\r\n";
            stream_response << "Content-Length: 13\r\n";
            stream_response << "Content-Type: text/plain; charset=utf-8\r\n\r\n";
            stream_response << "File not found\r\n";
        }
        
        std::string response = stream_response.str();
        send(client_socket, response.c_str(), response.size(), 0);

        close(client_socket);
    }
     close(server_socket);

    return 0;
    
}
