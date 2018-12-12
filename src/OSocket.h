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
using namespace std;

// Bind parameters for Berkley sockets	// ��Berkeley�׽��ֵĲ���
struct OBindParams
{
public:
	OBindParams();

public:
	uint16_t _Port;
	string   _Address;
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
	virtual int32_t OGetSockname(sockaddr* pSaOut, int* salen);
	SocketHandle OGetSocketHandle() const { return _TheSocket; }

protected:
	SocketHandle _TheSocket;     // Socket handle

};

// TCP Berkley socket
// Base class for TCP sockets, code shared between platforms  // TCP�׽��ֵ�����࣬ƽ̨֮�乲��Ĵ���
class OTCPSocketBase : public OBerkleySocket
{
public:
	OTCPSocketBase();
	OTCPSocketBase(FamilyType ft);
	~OTCPSocketBase();

public:
	virtual SocketHandle OBindLocalAddr(OBindParams* pBindParams);
	
	// ��һ��δ���ӵ��׽���ת���ɱ����׽��֣��������׽���
	virtual int          OListen();

	// ����һ���Ѿ����ӵ��׽��֣�����ȡ�ͻ��˵ĵ�ַ��Ϣ
	virtual SocketHandle OAccept();
	virtual int          OConnectRemoteAddr(OBindParams* pBindParams);
	virtual int          OSend(const void* pData, int bytes);
	virtual int          ORecv(uint8_t* pData, int bytesRead);
	
private:
	sockaddr_in  OSetAddr4(OBindParams* pBindParams);
	sockaddr_in6 OSetAddr6(OBindParams* pBindParams);
protected:
	bool _IsListenSocket;
};

#endif  /*OTCP_BASE_H_*/
