#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "CrossPlatform.h"
#include "OTask.h"
#include "OSocket.h"
using namespace std;

class TCPSrv
{
public:
	TCPSrv();
	TCPSrv(FamilyType ft);
	~TCPSrv();

	bool StartEchoSrv(OBindParams* obp);
	bool StartEchoSrvNonBlockSelect(OBindParams* obp);
private:
	OTCPSocket*			_SockListener;
	vector<OTCPSocket*> _SockClienter;
};

#define MAXPACKETSIZE 4096
class TCPServer
{
public:
	SocketHandle sockfd, newsockfd, n, pid;
	int _Port;
	
	string _LocalAddrIPv4;
	struct sockaddr_in _localAddrIPv4;
	struct sockaddr_in _clientAddrIPv4;

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
