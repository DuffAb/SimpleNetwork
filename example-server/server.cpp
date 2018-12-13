#include <iostream>
#include <time.h>
#include "../src/Wrap.h"
#include "../src/TCPServer.h"
#if 0


TCPServer tcp;

#ifdef _WIN32
DWORD WINAPI loop(LPVOID m)
{
	while (1)
	{
		srand(time(NULL));
		char ch = 'a' + rand() % 26;
		string s(1, ch);
		string str = tcp.getMessage();
		if (str != "")
		{
			cout << "Message:" << str.c_str() << endl;
			tcp.Send(" [client message: " + str + "] " + s);
			tcp.clean();
		}
		Sleep(1000);
	}
	tcp.detach();
}
#elif __linux__	
void * loop(void * m)
{
	static int ncount = 0;
	pthread_detach(pthread_self());
	cout << "running loop function...\n";
	while (1)
	{
		srand(time(NULL));
		char ch = 'a' + rand() % 26;
		string s(1, ch);
		string str = tcp.getMessage();
		if (str != "")
		{
			cout << "Message:" << str << endl;
			tcp.Send(" [client message: " + str + "] " + s);
			tcp.clean();
		}
		usleep(1000);
	}
	tcp.detach();
}
#endif

#endif

int main()
{	
	int mode = 0;
	TCPSrv srv4;
	TCPSrv srv6(FamilyType_IPV6);
	printf("select server mode: \n"
		"\t0: echo IPv4 server blocking I/O\n"
		"\t1: echo IPv6 server blocking I/O\n"
		"\t2: echo IPv4 server noblocking I/O\n"
		"\t3: echo IPv6 server noblocking I/O\n"
		"\t4: echo IPv4 server I/O reuse (select and poll)\n"
		"\t5: echo IPv6 server I/O reuse (select and poll)\n"
		"\t6: echo IPv4 server SIG I/O\n"
		"\t7: echo IPv6 server SIG I/O\n"
		"\t8: echo IPv4 server asynchronization I/O (POXI's aio_xxx function)\n"
		"\t9: echo IPv6 server asynchronization I/O (POXI's aio_xxx function)\n"
		"\t?: unknow now\n");
	OBindParams obp;
	obp._Address = "";
	obp._Port = 11999;
	if (scanf("%d", &mode))
	{
		switch (mode)
		{
		case 0:
			srv4.StartEchoSrv(&obp);
			break;
		case 1:
			srv6.StartEchoSrv(&obp);
			break;
		default:
			break;
		}
	}

#if 0



#ifdef _WIN32
	DWORD     _dThreadId;
	HANDLE    _hThread;
#elif __linux__	
	pthread_t msg;
#endif
	
	tcp.setup(11999);
	cout << "tcp.setup finish...\n";
#ifdef _WIN32
	if ((_hThread = CreateThread(0, 0, loop, (void *)0, 0, &_dThreadId)) != INVALID_HANDLE_VALUE)
#elif __linux__	
	if (pthread_create(&msg, NULL, loop, (void *)0) == 0)
#endif
	{
		tcp.receive();
	}

#endif


	return 0;
}
