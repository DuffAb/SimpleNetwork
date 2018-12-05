#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#include <iostream>
#include <string.h>
typedef SOCKET SocketHandle;
#elif __linux__	// linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int SocketHandle;
#endif




using namespace std;
class TCPClient
{
private:
	SocketHandle sock;
	std::string address;
	int port;
	struct sockaddr_in server;
public:
	TCPClient();
	bool setup(string address, int port);
	bool Send(string data);
	string receive(int size = 4096);
	string read();
	void exit();
};
#endif  /*TCP_CLIENT_H*/
