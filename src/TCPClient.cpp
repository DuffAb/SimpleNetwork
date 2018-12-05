#include "TCPClient.h"
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#define  TCP_Close(x) closesocket(x)
#define  TCP_Error(x) printf(x)
#elif __linux__	// linux
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
typedef int SocketHandle;
#define  TCP_Close(x) close(x)
#define  TCP_Error(x) herror(x)
#define  SOCK_START()
#endif

TCPClient::TCPClient()
{
	sock = -1;
	port = 0;
	address = "";
}

bool TCPClient::setup(string address, int port)
{
#ifdef _WIN32
	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0)
	{
		cout << "Load WinSock Failed!" << endl;
		return -1;
	}
#endif // _WIN32

	if (sock == -1)
	{
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == -1)
		{
			cout << "Could not create socket" << endl;
		}
	}

	if (inet_addr(address.c_str()) == -1)
	{
		struct hostent *phost = NULL;
		struct in_addr **addr_list;
		if ((phost = gethostbyname(address.c_str())) == NULL)
		{
			TCP_Error("gethostbyname");
			cout << "Failed to resolve hostname\n"; //WSAGetLastError() << endl;
			return false;
		}
		addr_list = (struct in_addr **)phost->h_addr_list;
		for (int i = 0; addr_list[i] != NULL; ++i)
		{
			server.sin_addr = *addr_list[i];
			break;
		}
	}
	else
	{
		server.sin_addr.s_addr = inet_addr(address.c_str());
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		cout << "connect error ";//WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool TCPClient::Send(string data)
{
	if (sock != -1)
	{
		if (send(sock, data.c_str(), strlen(data.c_str()), 0) < 0)
		{
			cout << "Send failed : " << data.c_str() << endl;
			return false;
		}
	}
	else
		return false;
	return true;
}

string TCPClient::receive(int size)
{
	static char buffer[4096];
	memset(&buffer[0], 0, sizeof(buffer));

	string reply;
	if (recv(sock, buffer, size, 0) < 0)
	{
		cout << "receive failed!" << endl;
	}
	buffer[size - 1] = '\0';
	reply = buffer;
	return reply;
}

string TCPClient::read()
{
	char buffer[1] = {};
	string reply;
	while (buffer[0] != '\n')
	{
		if (recv(sock, buffer, sizeof(buffer), 0) < 0)
		{
			cout << "receive failed!" << endl;
			return nullptr;
		}
		reply += buffer[0];
	}
	return reply;
}

void TCPClient::exit()
{
	TCP_Close(sock);
}
