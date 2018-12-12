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

// Bind parameters for Berkley sockets	// 绑定Berkeley套接字的参数
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

// Abstraction for a network socket. Inheritance hierarchy		// 网络套接字的抽象层。 继承层次结构
	// modeled after RakNet so that future support can be added		// 以RakNet为模型，以便可以添加未来的支持
	// for Linux, Windows RT, consoles, etc.						// 适用于Linux，Windows RT，consoles 等
class OSocket
{
public:
	OSocket();
	virtual void OClose() = 0;// 纯虚函数

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
// Base class for TCP sockets, code shared between platforms  // TCP套接字的虚基类，平台之间共享的代码
class OTCPSocketBase : public OBerkleySocket
{
public:
	OTCPSocketBase();
	OTCPSocketBase(FamilyType ft);
	~OTCPSocketBase();

public:
	virtual SocketHandle OBindLocalAddr(OBindParams* pBindParams);
	
	// 把一个未连接的套接字转化成被动套接字，即监听套接字
	virtual int          OListen();

	// 返回一个已经连接的套接字，并获取客户端的地址信息
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
