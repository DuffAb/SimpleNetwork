#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
typedef SOCKET SocketHandle;
#elif __linux__	// linux
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
typedef int SocketHandle;
#endif


using namespace std;

#define MAXPACKETSIZE 4096

class TCPServer
{
public:
	SocketHandle sockfd, newsockfd, n, pid;
	struct sockaddr_in serverAddress;
	struct sockaddr_in clientAddress;
#ifdef _WIN32
	DWORD     _dThreadId;
	HANDLE    serverThread;
#elif __linux__
	pthread_t serverThread;
#endif
	char msg[MAXPACKETSIZE];
	static string Message;

	void setup(int port);
	string receive();
	string getMessage();
	void Send(string msg);
	void detach();
	void clean();

private:
#ifdef _WIN32
	static DWORD WINAPI Task(LPVOID arg);
#elif __linux__	// linux
	static void * Task(void * argv);
#endif
	
};

#endif  /*TCP_SERVER_H*/
