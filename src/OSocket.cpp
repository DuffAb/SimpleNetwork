/************************************************************************************

PublicHeader:   n/a
Filename    :   OSocket.cpp
Content     :   Socket common data shared between all platforms.
Created     :   December 12, 2018
Authors     :   defeng.liang
************************************************************************************/

#include "OSocket.h"
#include "Wrap.h"

OBindParams::OBindParams() : _Port(0) 
{
}

OSocket::OSocket() : _AF_XXX(FamilyType_None)
{
}

OBerkleySocket::OBerkleySocket() : _TheSocket(INVALID_SOCKET)
{
}

OBerkleySocket::~OBerkleySocket()
{
	OClose();
}


int32_t OBerkleySocket::OGetSockname(sockaddr * pSaOut, int* salen)
{
	// 用于获取与某个套接字关联的本地协议地址  // 获得内核选择的临时端口
	int32_t i = getsockname(_TheSocket, (sockaddr*)pSaOut, salen);

	return i;
}

sockaddr_in OTCPSocketBase::OSetAddr4(OBindParams * pBindParams)
{
	static sockaddr_in sa4;
	memset(&sa4, 0, sizeof(sa4));
	sa4.sin_family = AF_INET;
	sa4.sin_port = htons(pBindParams->_Port);
	if (!OIp4ToSockaddr(pBindParams->_Address.c_str(), &sa4))
	{
		OGetHostByNameV4(pBindParams->_Address.c_str(), (struct sockaddr *)&sa4);
	}
	
	return sa4;
}

// 给bind函数指定要绑定的IP地址和/或端口号产生的结果
sockaddr_in6 OTCPSocketBase::OSetAddr6(OBindParams * pBindParams)
{
	static sockaddr_in6 sa6;
	memset(&sa6, 0, sizeof(sa6));
	sa6.sin6_family = AF_INET6;
	// 内核选择IP地址和端口			in6addr_any+0
	// 内核选择IP地址，进程指定端口	in6addr_any+_Port
	// 进程指定IP地址，内核选择端口	_Address   +0  
	// 进程指定IP地址和端口			_Address   +_Port
	sa6.sin6_port = htons(pBindParams->_Port);
	if (pBindParams->_Address.empty())
	{
		sa6.sin6_addr = in6addr_any;
	}
	else
	{
		struct in6_addr in6;
		inet_pton(AF_INET6, pBindParams->_Address.c_str(), &in6);
		memcpy(&sa6.sin6_addr, &in6, sizeof(struct in6_addr));
	}
	
	return sa6;
}

SocketHandle OTCPSocketBase::OBindLocalAddr(OBindParams* pBindParams)
{
	int ret = 0;
	if (_AF_XXX == FamilyType_IPV4)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// 创建一个基于 IPv4 的TCP套接字
		sockaddr_in sa4 = OSetAddr4(pBindParams);
		ret = bind(_TheSocket, (struct sockaddr*)&sa4, sizeof(sa4));
		
	}
	else if (_AF_XXX == FamilyType_IPV6)
	{
		_TheSocket = socket(AF_INET6, SOCK_STREAM, 0);// 创建一个基于 IPv6 的TCP套接字
		sockaddr_in6 sa6 = OSetAddr6(pBindParams);
		ret = bind(_TheSocket, (struct sockaddr*)&sa6, sizeof(sa6));
	}

	if (ret == EADDRINUSE)//地址已经被绑定
	{
		return false;
	}

	return _TheSocket;
}

int OTCPSocketBase::OListen()
{
	// (1)未完成连接队列				(2)已完成连接队列
	//    SYN_RCVD状态				   ESTABLISHED状态
	int ret = 0;
	int backlog = SOMAXCONN;
	char* ptr;
	if ((ptr = getenv("LISTENQ")) != NULL)
	{
		backlog = atoi(ptr);
	}

	ret = listen(_TheSocket, backlog);
	if (ret < 0)
	{
		// listen 返回值的错误处理
	}
	else
	{
		_IsListenSocket = true;
	}
	return ret;
}

SocketHandle OTCPSocketBase::OAccept()
{
	SocketHandle _ClientSocket;

	// 对客户地址协议不感兴趣，后两个参数设置位 NULL
	_ClientSocket = accept(_TheSocket, NULL, 0);

	// 与客户连接建立，getsockname可以返回由内核赋予该连接的本地IP地址
	sockaddr sa;
	int namelen = sizeof(sa);
	getsockname(_ClientSocket, &sa, &namelen);

	return _ClientSocket;
}

int OTCPSocketBase::OConnectRemoteAddr(OBindParams* pBindParams)
{
	int ret = 0;
	if (_AF_XXX == FamilyType_IPV4)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// 创建一个基于 IPv4 的TCP套接字
		sockaddr_in sa4 = OSetAddr4(pBindParams);
		ret = connect(_TheSocket, (struct sockaddr *)&sa4, sizeof(sa4));
		
	}
	else if (_AF_XXX == FamilyType_IPV6)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// 创建一个基于 IPv6 的TCP套接字
		sockaddr_in6 sa6 = OSetAddr6(pBindParams);
		ret = connect(_TheSocket, (struct sockaddr *)&sa6, sizeof(sa6));
	}
	
	if (ret < 0)
	{
		switch (ret)
		{
		case ETIMEDOUT://客户端没有收到SYN分节的响应 P94
			std::cout << "connect error: Connect timeout !" << std::endl;
			break;
		case ECONNREFUSED://硬错误，客户端的SYN的响应是RST(表示复位)
			std::cout << "connect error: Connect refused !" << std::endl;
			break;
		case EHOSTUNREACH://软错误，目的地不可达   1.按照本地系统的转发表，没有到达远程系统的路径   2.connect调用不等待就返回
		case ENETUNREACH:
			std::cout << "connect error: Connect destination unreachable !" << std::endl;
			break;

		default:
			break;
		}

		return false;
	}
	return true;
}

int OTCPSocketBase::OSend(const void * pData, int bytes)
{
	if (bytes <= 0)
	{
		return 0;
	}
	else
	{
		return send(_TheSocket, (const char*)pData, bytes, 0);
	}
}

int OTCPSocketBase::ORecv(uint8_t * pData, int bytesRead)
{
	return recv(_TheSocket, (char*)pData, bytesRead, 0);
}