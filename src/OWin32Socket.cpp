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

// 类外初始化静态类成员变量
int OWSAStartupSingleton::RefCount = 0;

// 实现静态类成员函数
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
//   signal是一个函数，有2个参数，第一个是int类型，第二个参数是一个函数指针
// 2.void(*signal(int signo, void(*func) (int))) (int)
//   signal仍然是一个函数，他返回一个函数指针，这个指针指向的函数没有返回值，只有一个int类型的参数
// 3.简化：
//   typedef void(*sighandler_t)(int)
//   sighandler_t 代表的就是 返回值是void，有一个int参数的函数指针
// 4.最后就成了这样
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