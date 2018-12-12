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

OTCPSocketBase::OTCPSocketBase()
{
	_AF_XXX = FamilyType_IPV4;
	OWSAStartupSingleton::AddRef();
}

OTCPSocketBase::OTCPSocketBase(FamilyType ft)
{
	_AF_XXX = ft;
	OWSAStartupSingleton::AddRef();
}

OTCPSocketBase::~OTCPSocketBase()
{
	OWSAStartupSingleton::DeRef();
}