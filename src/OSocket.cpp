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
	// ���ڻ�ȡ��ĳ���׽��ֹ����ı���Э���ַ  // ����ں�ѡ�����ʱ�˿�
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

// ��bind����ָ��Ҫ�󶨵�IP��ַ��/��˿ںŲ����Ľ��
sockaddr_in6 OTCPSocketBase::OSetAddr6(OBindParams * pBindParams)
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

SocketHandle OTCPSocketBase::OBindLocalAddr(OBindParams* pBindParams)
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

int OTCPSocketBase::OListen()
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

SocketHandle OTCPSocketBase::OAccept()
{
	SocketHandle _ClientSocket;

	// �Կͻ���ַЭ�鲻����Ȥ����������������λ NULL
	_ClientSocket = accept(_TheSocket, NULL, 0);

	// ��ͻ����ӽ�����getsockname���Է������ں˸�������ӵı���IP��ַ
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