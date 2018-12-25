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

// Bind parameters for Berkley sockets	// 绑定Berkeley套接字的参数
struct OBindParams
{
public:
	OBindParams();

public:
	uint16_t _Port;
	string   _Address;

	// 用于协议无关的网络编程
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
	virtual int32_t OGetSockname(sockaddr* pSaOut, socklen_t* salen);
	SocketHandle OGetSocketHandle() const { return _TheSocket; }

protected:
	SocketHandle _TheSocket;     // Socket handle

};

// TCP Berkley socket
// Base class for TCP sockets, code shared between platforms  // TCP套接字的虚基类，平台之间共享的代码

enum osigno
{
	O_SIGINT,		// interrupt(Ctrl+C中断)
	O_SIGILL,		// illegal instruction - invalid function image(非法指令)
	O_SIGFPE,       // floating point exception(浮点异常)
	O_SIGSEGV,		// segment violation(段错误)
	O_SIGTERM,		// Software termination signal from kill(Kill发出的软件终止)
	O_SIGABRT,		// abnormal termination triggered by abort call(Abort)

	// Windows
	O_SIGBREAK,		// Ctrl-Break sequence(Ctrl+Break中断) Windows 特有

	// Linux/Unix
	O_SIGHUP,		// 终端挂起或控制进程终止。当用户退出Shell时，由该进程启动的所有进程都会收到这个信号，默认动作为终止进程
	O_SIGQUIT,		// 键盘退出键被按下。当用户按下<Ctrl+D>或<Ctrl+\>组合键时，用户终端向正在运行中的由该终端启动的程序发出此信号。默认动作为退出程序
	O_SIGKILL,		// 无条件终止进程。进程接收到该信号会立即终止，不进行清理和暂存工作。该信号不能被忽略、处理和阻塞，它向系统管理员提供了可以杀死任何进程的方法
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
	
	// 把一个未连接的套接字转化成被动套接字，即监听套接字
	virtual int          OListen();

	// 返回一个已经连接的套接字，并获取客户端的地址信息
	virtual SocketHandle OAccept();
	virtual int          OConnectRemoteAddr(OBindParams* pBindParams);

	virtual void         OListenOnSrv(OBindParams* pBindParams);
	virtual void         OConnetToSrv(OBindParams* pBindParams);

	virtual int          OSend(const void* pData, int bytes);
	virtual int          ORecv(char* pData, int bytesRead);

	// 设置系统信号处理函数
	sighandler_t OSetSIGHandler(OSIGParams* osig);
	
private:
	sockaddr_in  OSetAddr4(OBindParams* pBindParams);
	sockaddr_in6 OSetAddr6(OBindParams* pBindParams);

	
public:
	bool _IsListenSocket;  // 不是<监听套接字>就是<外来套接字>
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
