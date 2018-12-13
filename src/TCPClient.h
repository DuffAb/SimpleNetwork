#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include "CrossPlatform.h"
#include "OSocket.h"
#include "Wrap.h"
using namespace std;

class TCPCli : public OTCPSocketBase
{
public:
	TCPCli();
	TCPCli(FamilyType ft);
	~TCPCli();

	bool StartEchoCli(OBindParams* obp);
};


class TCPClient
{
private:
	SocketHandle _sock;
	std::string address;
	int _ServerPort;
	struct sockaddr_in _ServerAddr;
public:
	TCPClient();
	bool setup(string address, int port);
	bool SetUpWithHostName(const char* hostname, const char* servname, const char* proto);
	bool Send(string data);
	string receive(int size = 4096);
	string read();
	void exit();
};
#endif  /*TCP_CLIENT_H*/
