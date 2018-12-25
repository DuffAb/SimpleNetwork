#include <iostream>
#include <string>
#include <signal.h>
#include <time.h>
#include "Wrap.h"
#include "TCPClient.h"


TCPClient tcp;
#ifdef _WIN32
#define  TCP_Sleep(x) Sleep(x)
#else
#define  TCP_Sleep(x) sleep(x/1000)
#endif

// 信号处理函数
void sig_exit(int s)
{
	tcp.exit();
	exit(0);
}

int main(int argc, char *argv[])
{
	int mode = 0;
	TCPCli cli4(FamilyType_IPV4);
	TCPCli cli6(FamilyType_IPV6);
	printf("select client mode: \n"
		"\t0: echo IPv4 client\n"
		"\t1: echo IPv6 client\n"
		"\t2: unknow now\n");
	OBindParams obp;
	
	obp._Port = 11999;
	if (scanf("%d", &mode))
	{
		switch (mode)
		{
		case 0:
			//obp._Address = "192.168.200.118";
			//obp._Address = "DESKTOP-CTQ37B5";
			obp._Address = "127.0.0.1";
			cli4.StartEchoCli(&obp);
			break;
		case 1:
			//obp._Address = "DESKTOP-CTQ37B5";
			obp._Address = "0:0:0:0:0:0:0:1";
			cli6.StartEchoCli(&obp);
			break;
		default:
			break;
		}
	}

#if 0

	

	// 信号
	// 不能被捕获：SIGKILL   SIGSTOP
	// SIGIO  SIGPOLL  SIGURG  SIGCHLD   SIG_IGN   SIG_DEF
	signal(SIGINT, sig_exit);


	tcp.setup(ipAddr,11999);

#if 0
	struct hostent *res = wrap_gethostbyname("ldf");
	wrap_gethostbyaddr(*res->h_addr_list, res->h_length, res->h_addrtype);
	wrap_getservbyname("domain", "udp");
	wrap_getservbyname("ftp", "tcp");
	wrap_getservbyname("ftp", NULL);
	wrap_getservbyname("ftp", "udp");

	wrap_getservbyport(htons(53), "udp");
	wrap_getservbyport(htons(21), "tcp");
	wrap_getservbyport(htons(21), NULL);
	wrap_getservbyport(htons(21), "udp");
#endif
	while(1)
	{
		srand(time(NULL));
		tcp.Send(to_string(rand()%25000));
		string rec = tcp.receive();
		if( rec != "" )
		{
			cout << "Server Response:" << rec << endl;
		}
		TCP_Sleep(1000);
	}
#endif
	return 0;
}
