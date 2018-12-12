#include "TCPClient.h"
#ifdef _WIN32
#pragma comment(lib,"ws2_32.lib")
#endif




TCPCli::TCPCli()
{
}

TCPCli::~TCPCli()
{
}

bool TCPCli::StartEchoCli()
{
	OBindParams obp;
	obp._Address = "DESKTOP-CTQ37B5";
	obp._Address = "127.0.0.1";
	obp._Port = 11999;
	OConnectRemoteAddr(&obp);
	char sendline[4096];
	char recvline[4096];
	char	*rptr;
	while ((rptr = fgets(sendline, 4096, stdin)) != NULL)// 键入EOF <CTRL + D> fgets() 返回空指针
	{
		send(_TheSocket, sendline, 4096, 0);
		if (sendline[0] == 'q')
		{
			return true;
		}

		if (recv(_TheSocket, recvline, 4096, 0) == 0)
		{
			std::cout << "server terminated prematurely\n";
		}
		
		fputs(recvline, stdout);
	}
	return true;
}


TCPClient::TCPClient()
{
	_sock = -1;
	_ServerPort = 0;
	address = "";
	memset(&_ServerAddr, 0, sizeof(_ServerAddr));
}

bool TCPClient::setup(string address, int port)
{
	if (_sock == -1)
	{
		_sock = socket(AF_INET, SOCK_STREAM, 0);
		if (_sock == -1)
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
			_ServerAddr.sin_addr = *addr_list[i];
			break;
		}
	}
	else
	{
		_ServerAddr.sin_addr.s_addr = inet_addr(address.c_str());
	}
	
	if (connect(_sock, (struct sockaddr *)&_ServerAddr, sizeof(_ServerAddr)) < 0)
	{
		cout << "connect error ";//WSAGetLastError() << endl;
		return false;
	}

	return true;
}

bool TCPClient::SetUpWithHostName(const char* hostname, const char* servname, const char* proto)
{
	// 使用主机名称获取IPv4的IP地址
	struct hostent *ht;
	struct in_addr **inAddr = NULL;
	ht = gethostbyname(hostname);
	if (ht)
	{
		inAddr = (struct in_addr **)ht->h_addr_list;//对方服务器主机可能返回多个IP地址
	}
	else
	{
		return false;
	}

	//使用服务名称获取端口号
	struct servent *st;
	st = getservbyname(servname, proto);
	if (!st)
	{
		return false;
	}

	for ( ; inAddr != NULL; inAddr++)
	{
		if (_sock == -1)
		{
			_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (_sock == -1)
			{
				cout << "Could not create socket" << endl;
			}

			_ServerAddr.sin_family = AF_INET;
			_ServerAddr.sin_port = st->s_port;
			memcpy(&_ServerAddr.sin_addr, *inAddr, sizeof(struct in_addr));

			if (connect(_sock, (struct sockaddr *)&_ServerAddr, sizeof(_ServerAddr)) < 0)
			{
				cout << "connect error ";//WSAGetLastError() << endl;
				return false;
			}
		}
	}
	
	return true;
}

bool TCPClient::Send(string data)
{
	if (_sock != -1)
	{
		if (send(_sock, data.c_str(), strlen(data.c_str()), 0) < 0)
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
	if (recv(_sock, buffer, size, 0) < 0)
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
		if (recv(_sock, buffer, sizeof(buffer), 0) < 0)
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
	TCP_Close(_sock);
}