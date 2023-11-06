#ifndef server_h
#define server_h

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fstream>
#include <sstream>

std::string getFileType(std::string& file_path );

int initializeSocket();

void listenConnection(int server_socket);

#endif