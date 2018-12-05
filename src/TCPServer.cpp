#include "TCPServer.h" 

#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#define  TCP_Close(x) closesocket(x)
#define  TCP_Error(x) printf(x)
#elif __linux__	// linux
#define  TCP_Close(x) close(x)
#define  TCP_Error(x) herror(x)
#endif

string TCPServer::Message;

#ifdef _WIN32
DWORD __stdcall TCPServer::Task(LPVOID arg)
{
	int n;
	int newsockfd = (long)arg;
	char msg[MAXPACKETSIZE];
	//pthread_detach(pthread_self());
	while (1)
	{
		n = recv(newsockfd, msg, MAXPACKETSIZE, 0);
		if (n == 0)
		{
			TCP_Close(newsockfd);
			break;
}
		msg[n] = 0;
		//send(newsockfd,msg,n,0);
		Message = string(msg);
	}
	return 0;
}
#elif __linux__	
void * TCPServer::Task(void * arg)
{
	int n;
	int newsockfd = (long)arg;
	char msg[MAXPACKETSIZE];
	pthread_detach(pthread_self());
	while (1)
	{
		n = recv(newsockfd, msg, MAXPACKETSIZE, 0);
		if (n == 0)
		{
			TCP_Close(newsockfd);
			break;
		}
		msg[n] = 0;
		//send(newsockfd,msg,n,0);
		Message = string(msg);
	}
	return 0;
}
#endif



void TCPServer::setup(int port)
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(port);
	bind(sockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	listen(sockfd, 5);
}

string TCPServer::receive()
{
	string str;
	while (1)
	{
		socklen_t sosize = sizeof(clientAddress);
		newsockfd = accept(sockfd, (struct sockaddr*)&clientAddress, &sosize);
		str = inet_ntoa(clientAddress.sin_addr);
#ifdef _WIN32
		serverThread = CreateThread(0, 0, Task, this, 0, &_dThreadId);
#elif __linux__
		pthread_create(&serverThread, NULL, &Task, (void *)newsockfd);
#endif
	}
	return str;
}

string TCPServer::getMessage()
{
	return Message;
}

void TCPServer::Send(string msg)
{
	send(newsockfd, msg.c_str(), msg.length(), 0);
}

void TCPServer::detach()
{
	TCP_Close(sockfd);
	TCP_Close(newsockfd);
}

void TCPServer::clean()
{
	Message = "";
	memset(msg, 0, MAXPACKETSIZE);
}

