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


int32_t OBerkleySocket::OGetSockname(sockaddr * pSaOut, socklen_t* salen)
{
	// ���ڻ�ȡ��ĳ���׽��ֹ����ı���Э���ַ  // ����ں�ѡ�����ʱ�˿�
	int32_t i = getsockname(_TheSocket, (sockaddr*)pSaOut, salen);

	return i;
}

sockaddr_in OTCPSocket::OSetAddr4(OBindParams * pBindParams)
{
	static sockaddr_in sa4;
	memset(&sa4, 0, sizeof(sa4));
	sa4.sin_family = AF_INET;
	sa4.sin_port = htons(pBindParams->_Port);
	if (pBindParams->_Address.empty())
	{
		sa4.sin_addr.s_addr = htonl(INADDR_ANY);
	}
	else
	{
		if (!OIp4ToSockaddr(pBindParams->_Address.c_str(), &sa4))
		{
			OGetHostByNameV4(pBindParams->_Address.c_str(), (struct sockaddr *)&sa4);
		}
	}
	
	return sa4;
}

// ��bind����ָ��Ҫ�󶨵�IP��ַ��/��˿ںŲ����Ľ��
sockaddr_in6 OTCPSocket::OSetAddr6(OBindParams * pBindParams)
{
	static sockaddr_in6 sa6;
	memset(&sa6, 0, sizeof(sa6));
	sa6.sin6_family = AF_INET6;
	// �ں�ѡ��IP��ַ�Ͷ˿�			in6addr_any+0
	// �ں�ѡ��IP��ַ������ָ���˿�	in6addr_any+_Port
	// ����ָ��IP��ַ���ں�ѡ��˿�	_Address   +0  
	// ����ָ��IP��ַ�Ͷ˿�			_Address   +_Port
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

SocketHandle OTCPSocket::OBindLocalAddr(OBindParams* pBindParams)
{
	int ret = 0;
	if (_AF_XXX == FamilyType_IPV4)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// ����һ������ IPv4 ��TCP�׽���
		sockaddr_in sa4 = OSetAddr4(pBindParams);
		ret = bind(_TheSocket, (struct sockaddr*)&sa4, sizeof(sa4));
		
	}
	else if (_AF_XXX == FamilyType_IPV6)
	{
		_TheSocket = socket(AF_INET6, SOCK_STREAM, 0);// ����һ������ IPv6 ��TCP�׽���
		sockaddr_in6 sa6 = OSetAddr6(pBindParams);
		ret = bind(_TheSocket, (struct sockaddr*)&sa6, sizeof(sa6));
	}

	if (ret == EADDRINUSE)//��ַ�Ѿ�����
	{
		return false;
	}

	return _TheSocket;
}

int OTCPSocket::OListen()
{
	// (1)δ������Ӷ���				(2)��������Ӷ���
	//    SYN_RCVD״̬				   ESTABLISHED״̬
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
		// listen ����ֵ�Ĵ�����
	}
	else
	{
		_IsListenSocket = true;
	}
	return ret;
}

SocketHandle OTCPSocket::OAccept()
{
	SocketHandle _ClientSocket;

	// �Կͻ���ַЭ�鲻����Ȥ����������������λ NULL
	_ClientSocket = accept(_TheSocket, NULL, 0);
	if (errno == EPROTO) // Э�����
	{
	}else if (errno == ECONNABORTED)// ��������������ֹ
	{
	}

	// ��ͻ����ӽ�����getsockname���Է������ں˸�������ӵı���IP��ַ
	sockaddr sa;
	socklen_t namelen = sizeof(sa);
	getsockname(_ClientSocket, &sa, &namelen);

	return _ClientSocket;
}

int OTCPSocket::OConnectRemoteAddr(OBindParams* pBindParams)
{
	int ret = 0;
	if (_AF_XXX == FamilyType_IPV4)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// ����һ������ IPv4 ��TCP�׽���
		sockaddr_in sa4 = OSetAddr4(pBindParams);
		ret = connect(_TheSocket, (struct sockaddr *)&sa4, sizeof(sa4));
		
	}
	else if (_AF_XXX == FamilyType_IPV6)
	{
		_TheSocket = socket(AF_INET, SOCK_STREAM, 0);// ����һ������ IPv6 ��TCP�׽���
		sockaddr_in6 sa6 = OSetAddr6(pBindParams);
		ret = connect(_TheSocket, (struct sockaddr *)&sa6, sizeof(sa6));
	}
	
	if (ret < 0)
	{
		switch (ret)
		{
		case ETIMEDOUT://�ͻ���û���յ�SYN�ֽڵ���Ӧ P94
			std::cout << "connect error: Connect timeout !" << std::endl;
			break;
		case ECONNREFUSED://Ӳ���󣬿ͻ��˵�SYN����Ӧ��RST(��ʾ��λ)
			std::cout << "connect error: Connect refused !" << std::endl;
			break;
		case EHOSTUNREACH://�����Ŀ�ĵز��ɴ�   1.���ձ���ϵͳ��ת����û�е���Զ��ϵͳ��·��   2.connect���ò��ȴ��ͷ���
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

void OTCPSocket::OListenOnSrv(OBindParams * pBindParams)
{
	int ret = 0;
	const int on = 1;
	struct addrinfo hints;
	struct addrinfo *result;
	struct addrinfo *res;
	
	memset(&hints, 0, sizeof(addrinfo));
	hints.ai_flags    = AI_PASSIVE;
	hints.ai_family   = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	ret = getaddrinfo(pBindParams->hostname, pBindParams->servname, &hints, &result);
	if (ret != 0)
	{
		std::cout << "getaddrinfo function failed!" << std::endl;
		return ;
	}

	res = result;
	do 
	{
		_TheSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (_TheSocket < 0)
		{
			continue;
		}
		setsockopt(_TheSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&on, sizeof(on));
		ret = bind(_TheSocket, res->ai_addr, res->ai_addrlen);
		if (ret == 0)
		{
			break;
		}
		OClose();

	} while ((res = result->ai_next) != NULL);

	if (res == NULL)
	{
		std::cout << "OListenOnSrv error for " << pBindParams->hostname << "  " << pBindParams->servname << std::endl;
	}
	OListen();
	freeaddrinfo(result);

	return ;
}

void OTCPSocket::OConnetToSrv(OBindParams * pBindParams)
{
}

int OTCPSocket::OSend(const void * pData, int bytes)
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

int OTCPSocket::ORecv(char* pData, int bytesRead)
{
	return recv(_TheSocket, pData, bytesRead, 0);
}



OTCPSocketPollState::OTCPSocketPollState()
{
	FD_ZERO(&_readFD);
	FD_ZERO(&_exceptionFD);
	FD_ZERO(&_writeFD);
	FD_ZERO(&_allFD);
	_largestDescriptor = INVALID_SOCKET;
}
bool OTCPSocketPollState::OIsValid() const
{
	return _largestDescriptor != INVALID_SOCKET;
}
void OTCPSocketPollState::OAdd(OTCPSocket* tcpSocket, bool bexception)
{
	if (!tcpSocket)
	{
		return;
	}

	SocketHandle handle = tcpSocket->OGetSocketHandle();

	if (_largestDescriptor == INVALID_SOCKET ||	_largestDescriptor < handle)
	{
		_largestDescriptor = handle;
	}

	FD_SET(handle, &_allFD);

	/*if (bexception)
	{
		FD_SET(handle, &_exceptionFD);
	}


	if (!tcpSocket->_IsListenSocket)
	{
		FD_SET(handle, &_writeFD);
	}*/
}

bool OTCPSocketPollState::OPoll(long usec, long seconds)
{
	int nready = 0;
	timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = usec;

	_readFD = _allFD;

	nready = (int)select((int)_largestDescriptor + 1, &_readFD, NULL, NULL, NULL);
	
	return nready > 0;
}
SocketHandle OTCPSocketPollState::OHandleEvent(OTCPSocket* tcpSocket, OTask* task)
{
	SocketHandle newSock = INVALID_SOCKET;
	if (!tcpSocket)
	{
		return newSock;
	}

	SocketHandle handle = tcpSocket->OGetSocketHandle();

	if (FD_ISSET(handle, &_readFD))
	{
		if (tcpSocket->_IsListenSocket) // �����׽��ֿɶ������пͻ����ӵ���
		{
			cout << "client socket is connect ... " << endl;
			struct sockaddr_storage sockAddr;
			socklen_t sockAddrSize = sizeof(sockAddr);

			newSock = accept(handle, (sockaddr*)&sockAddr, (socklen_t*)&sockAddrSize);
			if (newSock > 0)
			{
				return newSock;
			}
			
		}
		else // �ͻ��׽��ֿɶ���ִ����ͻ����������
		{
			task->ORun(handle);
		}
	}

	if (!tcpSocket->_IsListenSocket && FD_ISSET(handle, &_writeFD))
	{
		task->ORun(handle);
	}

	if (FD_ISSET(handle, &_exceptionFD))
	{
		tcpSocket->OClose();
	}
	return newSock;
}