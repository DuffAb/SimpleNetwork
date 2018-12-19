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
#if 1

	int mode = 0;
	TCPSrv srv4(FamilyType_IPV4);
	TCPSrv srv6(FamilyType_IPV6);
	printf("select server mode: \n"
		"\t0: echo IPv4 server blocking I/O\n"
		"\t1: echo IPv6 server blocking I/O\n"
		"\t2: echo IPv4 server nonblocking I/O\n"
		"\t3: echo IPv6 server nonblocking I/O\n"
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
		case 2:
			srv4.StartEchoSrvNonBlockSelect(&obp);
			break;
		case 3:
			srv6.StartEchoSrvNonBlockSelect(&obp);
			break;
		default:
			break;
		}
	}

#endif

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

#if 0
	TCPSrv srv4(FamilyType_IPV4);
	int					i, maxi, maxfd, listenfd, connfd, sockfd;
	int					nready, client[FD_SETSIZE];
	ssize_t				n;
	fd_set				rset, allset;
	char				buf[4096];
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(11999);

	bind(listenfd, (sockaddr *)&servaddr, sizeof(servaddr));

	listen(listenfd, 5);

	maxfd = listenfd;			/* initialize */
	maxi = -1;					/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
	{
		client[i] = -1;			/* -1 indicates available entry */
	}

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	/* end fig01 */

	/* include fig02 */
	for (; ; )
	{
		rset = allset;		/* structure assignment */
		// 阻塞于select，等待某个事件发生：新客户连接建立，或是数据、FIN或RST的到达
		nready = select(maxfd + 1, &rset, NULL, NULL, NULL);
		
		if (FD_ISSET(listenfd, &rset)) // 如果监听套接字可读，那么已建立了一个新的连接
		{	/* new client connection */
			cout << "client socket is connect ... " << nready << endl;
			clilen = sizeof(cliaddr);
			connfd = accept(listenfd, (sockaddr *)&cliaddr, &clilen);

			for (i = 0; i < FD_SETSIZE; i++)
			{
				if (client[i] < 0)
				{
					client[i] = connfd;	/* save descriptor */
					break;
				}
			}

			FD_SET(connfd, &allset);	/* add new descriptor to set */
			if (connfd > maxfd)
				maxfd = connfd;			/* for select */
			if (i > maxi)
				maxi = i;				/* max index in client[] array */

			if (--nready <= 0)
				continue;				/* no more readable descriptors */
		}

		for (i = 0; i <= maxi; i++)
		{	/* check all clients for data 检查现有连接*/
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset)) // 测试其描述符是否在select返回的描述符集中
			{
				if ((n = recv(sockfd, buf, 11999, 0)) == 0)
				{
					/*4connection closed by client */
					closesocket(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
				{
					send(sockfd, buf, n, 0);
				}

				if (--nready <= 0)
					break;				/* no more readable descriptors */
			}
		}
	}
#endif
	return 0;
}
