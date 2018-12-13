#ifndef _CROSS_PLATFORM_H_
#define _CROSS_PLATFORM_H_

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#ifdef _WIN32
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <basetsd.h>
#include <signal.h>
#include <io.h>
#elif __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif


// Cross-platform Function
#ifdef _WIN32
#define UW_ErrMessage(x) (x)
#define UW_Read(x, y, z)   _read(x, y, z)
#define UW_Write(x, y, z)  _write(x, y, z)
#define TCP_Close(x)       closesocket(x)
#define TCP_Error(x)       printf(x)
#elif __linux__
#define UW_ErrMessage(x)   hstrerror(x)
#define UW_Read(x, y, z)   read(x, y, z)
#define UW_Write(x, y, z)  write(x, y, z)
#define TCP_Close(x)       close(x)
#define TCP_Error(x)       herror(x)
#endif


// Cross-platform Type
#ifdef _WIN32
typedef SSIZE_T ssize_t;
typedef SOCKET  SocketHandle;
#elif __linux__
typedef int     SocketHandle;
static const SocketHandle INVALID_SOCKET = -1;
static const int SOCKET_ERROR = -1;
#endif

#endif  /*_CROSS_PLATFORM_H_*/
