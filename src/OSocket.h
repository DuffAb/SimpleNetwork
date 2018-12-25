/************************************************************************************

PublicHeader:   n/a
Filename    :   OSocket.h
Content     :   Socket common data shared between all platforms.
Created     :   December 12, 2018
Authors     :   defeng.liang
************************************************************************************/
#ifndef OTCP_BASE_H_
#define OTCP_BASE_H_
#include <string>
#include "CrossPlatform.h"
#include "OTask.h"
typedef void (*sighandler_t)(int);	/* for signal handlers */
using namespace std;

// Bind parameters for Berkley sockets	// ��Berkeley�׽��ֵĲ���
struct OBindParams
{
public:
	OBindParams();

public:
	uint16_t _Port;
	string   _Address;

	// ����Э���޹ص�������
	const char* hostname;
	const char* servname;
};

// Types of network transport
enum FamilyType
{
	FamilyType_None,          // No transport (useful placeholder for invalid states)
	FamilyType_IPV4,           // TCP/IPv4/v6
	FamilyType_IPV6,           // UDP/IPv4/v6
};

// Abstraction for a network socket. Inheritance hierarchy		// �����׽��ֵĳ���㡣 �̳в�νṹ
	// modeled after RakNet so that future support can be added		// ��RakNetΪģ�ͣ��Ա�������δ����֧��
	// for Linux, Windows RT, consoles, etc.						// ������Linux��Windows RT��consoles ��
class OSocket
{
public:
	OSocket();
	virtual void OClose() = 0;// ���麯��

public:
	FamilyType _AF_XXX; // family type
};

// Berkley socket
class OBerkleySocket : public OSocket
{
public:
	OBerkleySocket();
	virtual ~OBerkleySocket();

	virtual void    OClose();    // Linux:close()   Windows:closesocket()
	virtual int32_t OGetSockname(sockaddr* pSaOut, socklen_t* salen);
	SocketHandle OGetSocketHandle() const { return _TheSocket; }

protected:
	SocketHandle _TheSocket;     // Socket handle

};

// TCP Berkley socket
// Base class for TCP sockets, code shared between platforms  // TCP�׽��ֵ�����࣬ƽ̨֮�乲��Ĵ���

enum osigno
{
	O_SIGINT,		// interrupt(Ctrl+C�ж�)
	O_SIGILL,		// illegal instruction - invalid function image(�Ƿ�ָ��)
	O_SIGFPE,       // floating point exception(�����쳣)
	O_SIGSEGV,		// segment violation(�δ���)
	O_SIGTERM,		// Software termination signal from kill(Kill�����������ֹ)
	O_SIGABRT,		// abnormal termination triggered by abort call(Abort)

	// Windows
	O_SIGBREAK,		// Ctrl-Break sequence(Ctrl+Break�ж�) Windows ����

	// Linux/Unix
	O_SIGHUP,		// �ն˹������ƽ�����ֹ�����û��˳�Shellʱ���ɸý������������н��̶����յ�����źţ�Ĭ�϶���Ϊ��ֹ����
	O_SIGQUIT,		// �����˳��������¡����û�����<Ctrl+D>��<Ctrl+\>��ϼ�ʱ���û��ն������������е��ɸ��ն������ĳ��򷢳����źš�Ĭ�϶���Ϊ�˳�����
	O_SIGKILL,		// ��������ֹ���̡����̽��յ����źŻ�������ֹ��������������ݴ湤�������źŲ��ܱ����ԡ����������������ϵͳ����Ա�ṩ�˿���ɱ���κν��̵ķ���
	O_SIGCHLD,
	O_SIGPIPE,
};
struct OSIGParams {
	osigno        signo;
	sighandler_t  handler;
};

class OTCPSocket : public OBerkleySocket
{
public:
	OTCPSocket();
	OTCPSocket(FamilyType ft);
	OTCPSocket(FamilyType ft, SocketHandle handle);
	~OTCPSocket();

public:
	virtual SocketHandle OBindLocalAddr(OBindParams* pBindParams);
	
	// ��һ��δ���ӵ��׽���ת���ɱ����׽��֣��������׽���
	virtual int          OListen();

	// ����һ���Ѿ����ӵ��׽��֣�����ȡ�ͻ��˵ĵ�ַ��Ϣ
	virtual SocketHandle OAccept();
	virtual int          OConnectRemoteAddr(OBindParams* pBindParams);

	virtual void         OListenOnSrv(OBindParams* pBindParams);
	virtual void         OConnetToSrv(OBindParams* pBindParams);

	virtual int          OSend(const void* pData, int bytes);
	virtual int          ORecv(char* pData, int bytesRead);

	// ����ϵͳ�źŴ�����
	sighandler_t OSetSIGHandler(OSIGParams* osig);
	
private:
	sockaddr_in  OSetAddr4(OBindParams* pBindParams);
	sockaddr_in6 OSetAddr6(OBindParams* pBindParams);

	
public:
	bool _IsListenSocket;  // ����<�����׽���>����<�����׽���>
};

// OTCPSocketPollState
// Polls multiple blocking TCP sockets at once
class OTCPSocketPollState
{
public:
	fd_set _readFD;
	fd_set _exceptionFD;
	fd_set _writeFD;

	fd_set _allFD;
	SocketHandle _largestDescriptor;

public:
	OTCPSocketPollState();
	bool OIsValid() const;
	void OAdd(OTCPSocket* tcpSocket, bool bexception = false);
	bool OPoll(long usec = 30000, long seconds = 0);
	SocketHandle OHandleEvent(OTCPSocket* tcpSocket, OTask* task);
};

#endif  /*OTCP_BASE_H_*/
