#include <iostream>
#include <string>
#include <signal.h>
#include "TCPClient.h"
#include <time.h>

TCPClient tcp;
#ifdef _WIN32
#define  TCP_Sleep(x) Sleep(x)
#else
#define  TCP_Sleep(x) sleep(x/1000)
#endif
void sig_exit(int s)
{
	tcp.exit();
	exit(0);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, sig_exit);

	tcp.setup("127.0.0.1",11999);
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
	return 0;
}
