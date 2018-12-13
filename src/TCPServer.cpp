#include "TCPServer.h" 
#include "Wrap.h"
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif

void sig_child(int signo);

TCPSrv::TCPSrv()
{
}

TCPSrv::TCPSrv(FamilyType ft)
{
	_AF_XXX = ft;
}

TCPSrv::~TCPSrv()
{
}

bool TCPSrv::StartEchoSrv(OBindParams* obp)
{
	OSIGParams osp;
	OBindLocalAddr(obp);
	OListen();
	osp.signo = O_SIGCHLD;
	osp.handler = sig_child;
	OSetSIGHandler(&osp);
	SocketHandle clientfd;
	while (true)
	{
		size_t n = 0;
		if ((clientfd = OAccept()) < 0)
		{
			if (errno == EINTR)// 自己重启被中断的系统调用
			{
				continue;
			}
			else
			{
				std::cout << "accept errror \n";
				return false;
			}
		}

		char buf[4096];
		memset(buf, 0, 4096);
		while ((n = recv(clientfd, buf, 4096, 0)) > 0)
		{
			if (buf[0] == 'q')
			{
				break;
			}
			fputs(buf, stdout);
			send(clientfd, buf, 4096, 0);
			memset(buf, 0, 4096);
		}

#if 0
		pid_t pid;
		if ((pid = fork()) == 0)
		{
			close(_TheSocket);
			char buf[4096];
			memset(buf, 0, 4096);
			while ((n = recv(clientfd, buf, 4096, 0)) > 0)
			{
				if (buf[0] == 'q')
				{
					break;
				}
				fputs(buf, stdout);
				send(clientfd, buf, 4096, 0);
				memset(buf, 0, 4096);
			}
		}
#endif
	}
	
	return true;
}

#if 0


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
	memset(&_localAddrIPv4, 0, sizeof(_localAddrIPv4));
	_localAddrIPv4.sin_family = AF_INET;
	_localAddrIPv4.sin_addr.s_addr = htonl(INADDR_ANY);
	_localAddrIPv4.sin_port = htons(port);

	bind(sockfd, (struct sockaddr *)&_localAddrIPv4, sizeof(_localAddrIPv4));
	listen(sockfd, 5);
}

string TCPServer::receive()
{
	string str;
	while (1)
	{
		socklen_t sosize = sizeof(_clientAddrIPv4);
		newsockfd = accept(sockfd, (struct sockaddr*)&_clientAddrIPv4, &sosize);
		str = inet_ntoa(_clientAddrIPv4.sin_addr);
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

#endif