/************************************************************************************

PublicHeader:   n/a
Filename    :   OWin32Socket.cpp
Content     :   Windows-specific socket-based networking implementation
Created     :   December 12, 2018
Authors     :   defeng.liang
************************************************************************************/

#include "OSocket.h"

// WSAStartupSingleton
class OWSAStartupSingleton
{
public:
	static void AddRef(void);
	static void DeRef(void);
protected:
	static int RefCount;
};

// �����ʼ����̬���Ա����
int OWSAStartupSingleton::RefCount = 0;

// ʵ�־�̬���Ա����
void OWSAStartupSingleton::AddRef()
{
	if (++RefCount == 1)
	{
		WSADATA wsaData;
		const int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (err != 0)
		{
			std::cout << "[Socket] Unable to initialize Winsock " << err << std::endl;
		}
	}
}

void OWSAStartupSingleton::DeRef()
{
	if (RefCount > 0)
	{
		if (--RefCount == 0)
		{
			WSACleanup();
			RefCount = 0;
		}
	}
}

void OBerkleySocket::OClose()
{
	if (_TheSocket != INVALID_SOCKET)
	{
		closesocket(_TheSocket);
		_TheSocket = INVALID_SOCKET;
	}
}

OTCPSocket::OTCPSocket() : _IsListenSocket(false)
{
	_AF_XXX = FamilyType_None;
	OWSAStartupSingleton::AddRef();
}

OTCPSocket::OTCPSocket(FamilyType ft) : _IsListenSocket(false)
{
	_AF_XXX = ft;
	OWSAStartupSingleton::AddRef();
}

OTCPSocket::OTCPSocket(FamilyType ft, SocketHandle handle) : _IsListenSocket(false)
{
	_AF_XXX = ft;
	_TheSocket = handle;
	OWSAStartupSingleton::AddRef();
}

OTCPSocket::~OTCPSocket()
{
	OWSAStartupSingleton::DeRef();
}

// void (*signal(int signo, void (*func)(int)))(int);
// 1.signal(int signo, void(*func)(int))
//   signal��һ����������2����������һ����int���ͣ��ڶ���������һ������ָ��
// 2.void(*signal(int signo, void(*func) (int))) (int)
//   signal��Ȼ��һ��������������һ������ָ�룬���ָ��ָ��ĺ���û�з���ֵ��ֻ��һ��int���͵Ĳ���
// 3.�򻯣�
//   typedef void(*sighandler_t)(int)
//   sighandler_t ����ľ��� ����ֵ��void����һ��int�����ĺ���ָ��
// 4.���ͳ�������
//  sighandler_t signal(int signo, sighandler_t handler);
sighandler_t OTCPSocket::OSetSIGHandler(OSIGParams* osig)
{
	sighandler_t previousHandler = NULL;
	int signo = 0;
	switch (osig->signo)
	{
	case O_SIGINT:
		signo = SIGINT;
		break;
	case O_SIGILL:
		signo = SIGILL;
		break;
	case O_SIGFPE:
		signo = SIGFPE;
		break;
	case O_SIGSEGV:
		signo = SIGSEGV;
		break;
	case O_SIGTERM:
		signo = SIGTERM;
		break;
	case O_SIGBREAK:
		signo = SIGBREAK;
		break;
	case O_SIGABRT:
		signo = SIGABRT;
		break;
	}
	previousHandler = signal(signo, osig->handler);
	return previousHandler;
}