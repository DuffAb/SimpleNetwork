#include <iostream>
#include <time.h>
#include "TCPServer.h"

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
		cout << "running loop function..." << ncount++ << endl;
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


int main()
{
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
	return 0;
}
