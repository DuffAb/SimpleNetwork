/************************************************************************************

PublicHeader:   n/a
Filename    :   OWin32Socket.cpp
Content     :   Windows-specific socket-based networking implementation
Created     :   December 12, 2018
Authors     :   defeng.liang
************************************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include "OSocket.h"

void OBerkleySocket::OClose()
{
	if (_TheSocket != INVALID_SOCKET)
	{
		close(_TheSocket);
		_TheSocket = INVALID_SOCKET;
	}
}

OTCPSocket::OTCPSocket()
{
	_AF_XXX = FamilyType_None;
}

OTCPSocket::OTCPSocket(FamilyType ft)
{
	_AF_XXX = ft;
}

OTCPSocket::OTCPSocket(FamilyType ft, SocketHandle handle)
{
	_AF_XXX = ft;
	_TheSocket = handle;
}

OTCPSocket::~OTCPSocket()
{
}

void sig_child(int signo)
{
	pid_t pid;
	int   stat;
#if 0
	pid = wait(&stat);// 返回已终止的进程ID号，以及终止状态
	printf("child %d terminated\n", pid);
#endif
	// 因为Unix信号是不排队的，因此必须在while循环内调用waitpid以获取所有已终止的子进程的状态
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)//WNOHANG：告知waitpid在有尚未终止的子进程在运行时不要阻塞
		printf("child %d terminated\n", pid);
	return;
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
	struct sigaction	sigact, prvsigact;
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
	case O_SIGABRT:
		signo = SIGABRT;
		break;
	case O_SIGHUP:
		signo = SIGHUP;
		break;
	case O_SIGQUIT:
		signo = SIGQUIT;
		break; 
	case O_SIGKILL:
		signo = SIGKILL;
		break;
	case O_SIGCHLD:
		signo = SIGCHLD;
		break;
	default:
		break;
	}

	sigact.sa_handler = osig->handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		sigact.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
	} else {
#ifdef	SA_RESTART
		sigact.sa_flags |= SA_RESTART;		/* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &sigact, &prvsigact) < 0)
		return(SIG_ERR);
	return(prvsigact.sa_handler);//将相应信号的旧行为作为signal函数的返回值
}