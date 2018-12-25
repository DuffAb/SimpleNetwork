#ifndef _WRAP_H_
#define _WRAP_H_

#include "CrossPlatform.h"

using namespace std;

int OListen(SocketHandle sock)
{
	// (1)未完成连接队列				(2)已完成连接队列
	//    SYN_RCVD状态				   ESTABLISHED状态
	int ret = 0;
	int backlog = SOMAXCONN;
	char* ptr;
	if ((ptr = getenv("LISTENQ")) != NULL)
	{
		backlog = atoi(ptr);
	}

	ret = listen(sock, backlog);
	if (ret < 0)
	{
		// listen 返回值的错误处理
	}
	else
	{
		_IsListenSocket = true;
	}
	return ret;
}

// 解析器函数，接触DNS服务器，把主机名映射成IPv4地址，只支持IPv4
static void OGetHostByNameV4(const char* hostname, struct sockaddr* sa)
{
	//struct  hostent {
	//	char    *h_name;      /* official name of host - 正式主机名*/
	//	char    **h_aliases;  /* alias list - 别名列表*/
	//	short   h_addrtype;   /* host address type - AF_INET*/
	//	short   h_length;     /* length of address */
	//	char    **h_addr_list;/* list of addresses */
	//};
	struct hostent* ht = NULL;
	char **pp = NULL;
	char strTmp[INET_ADDRSTRLEN];
	if ((ht = gethostbyname(hostname)) == NULL)
	{
		switch (h_errno)
		{
		case HOST_NOT_FOUND:
		case TRY_AGAIN:
		case NO_RECOVERY:
		case NO_DATA:       // 表示指定的名字有效，但是没有A记录
			cout << UW_ErrMessage(h_errno) << endl; // 输出相应的错误说明
			return;
		}
	}
	
	cout << "official hostname: " << ht->h_name << endl;
	
	int i = 1;
	for (pp = ht->h_aliases; *pp != NULL; pp++)
	{
		cout << "aliases " << i++ << " : " << *pp << endl;
	}

	struct in_addr **addr_list;
	addr_list = (struct in_addr **)ht->h_addr_list;
	switch (ht->h_addrtype)
	{
	case AF_INET: {
		struct sockaddr_in * sa4 = (struct sockaddr_in *)sa;
		for (; *addr_list != NULL; addr_list++)
		{
			// 将数值格式转化为点分十进制的ip地址格式 返回值：若成功则为指向结构的指针，若出错则为NULL
			cout << "IPv4 address: " << inet_ntop(ht->h_addrtype, *addr_list, strTmp, sizeof(strTmp)) << endl;
			sa4->sin_addr = **addr_list;
		}
		break;
	}
	default:
		cout << "unknown address type\n";
		break;
	}

	return ;
}

// 解析器函数，接触DNS服务器，把二进制IPv4地址映射成主机名，只支持IPv4
static string OGetHostByAddrV4(struct sockaddr* addr, socklen_t len)
{
	char **pp = NULL;
	struct hostent* ht = NULL;
	char strTmp[INET_ADDRSTRLEN];
	if (addr->sa_family == AF_INET)
	{
		sockaddr_in* addr4 = (sockaddr_in*)addr;
		in_addr* addr_in = &(addr4->sin_addr);
		// addr 参数实际上不是char *类型，而是一个指向存放IPv4地址的某个in_addr结构指针
		ht = gethostbyaddr((const char*)addr_in, 4, AF_INET);

	}
	else
	{
		return "";
	}
	
	switch (h_errno)
	{
	case HOST_NOT_FOUND:
	case TRY_AGAIN:
	case NO_RECOVERY:
	case NO_DATA:       // 表示指定的名字有效，但是没有A记录
		cout << UW_ErrMessage(h_errno) << endl; // 输出相应的错误说明
		return "";
	}
	cout << "official hostname: " << ht->h_name << endl;

	int i = 1;
	for (pp = ht->h_aliases; *pp != NULL; pp++)
	{
		cout << "aliases " << i++ << " : " << *pp << endl;
	}

	switch (ht->h_addrtype)
	{
	case AF_INET:
		pp = ht->h_addr_list;
		for (; *pp != NULL; pp++)
		{
			// 将数值格式转化为点分十进制的ip地址格式 返回值：若成功则为指向结构的指针，若出错则为NULL
			cout << "IPv4 address: " << inet_ntop(ht->h_addrtype, *pp, strTmp, sizeof(strTmp)) << endl;
		}
		break;
	default:
		cout << "unknown address type\n";
		break;
	}

	return ht->h_name;
}

static struct servent* wrap_getservbyname(const char* servname, const char* protoname)
{
	//struct  servent {
	//	char    *s_name;      /* official service name */
	//	char    **s_aliases;  /* alias list */
	//	short   s_port;       /* port # 网络字节序返回端口号 */
	//	char    *s_proto;     /* protocol to use */
	//};
	struct servent *st;
	// 服务名参数servname必须指定，如果同时指定协议protoname，那么指定服务必须有匹配的协议
	st = getservbyname(servname, protoname);
	if (st == NULL)
	{
		return st;
	}
	cout << "getservbyname servname: " << st->s_name << endl;
	cout << "getservbyname Port: " << st->s_port << endl;
	cout << "getservbyname proto: " << st->s_proto << endl;
	
	int i = 0;
	if (*st->s_aliases)
	{
		for (char ** pp = st->s_aliases; pp != NULL; pp++)
		{
			cout << "getservbyname aliase " << ++i << " : " << *pp << endl;
		}
	}
	else
	{
		cout << "getservbyname aliase is null." << endl;
	}
	
	return st;
}

// Windows:获取的是 C:\WINDOWS\system32\drivers\etc\services 文件的信息
// Linux  :获取的是 /etc/services 文件的信息
static struct servent* wrap_getservbyport(int port, const char* protoname)
{
	struct servent *st;
	st = getservbyport(htons(port), protoname);
	if (st == NULL)
	{
		return st;
	}

	cout << "getservbyport servname: " << st->s_name << endl;
	cout << "getservbyport Port: " << st->s_port << endl;
	cout << "getservbyport proto: " << st->s_proto << endl;

	int i = 0;
	if (*st->s_aliases)
	{
		for (char ** pp = st->s_aliases; pp != NULL; pp++)
		{
			cout << "getservbyport aliase " << ++i << " : " << *pp << endl;
		}
	}
	else
	{
		cout << "getservbyport aliase is null." << endl;
	}

	return st;
}

// 处理名字到地址以及服务到端口这两种转换
static int wrap_getaddrinfo(const char* hostname, const char* service, const struct addrinfo* hints, struct addrinfo** result)
{
	//struct addrinfo
	//{
	//	int                 ai_flags;       // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST
	//	int                 ai_family;      // PF_xxx 或 AF_xxx
	//	int                 ai_socktype;    // SOCK_xxx
	//	int                 ai_protocol;    // 0 or IPPROTO_xxx for IPv4 and IPv6
	//	size_t              ai_addrlen;     // Length of ai_addr
	//	char *              ai_canonname;   // Canonical name for nodename
	//	struct sockaddr *   ai_addr;        // Binary address
	//	struct addrinfo *   ai_next;        // Next structure in linked list
	//}
	// hostname:参数是一个主机名或地址串（IPv4 / IPv6）
	// service :参数是一个服务名或十进制端口数串
	// hints   :参数可以是空指针或一个指向某个addrinfo结构体的指针
	int ret = 0;
	ret = getaddrinfo(hostname, service, hints, result);
}


// 查看主机16字节顺序是大端序还是小端序
enum TYPE_BYTE_ORDER
{
	TYPE_BIG_ENDIAN,
	TYPE_LITTLE_ENDIAN,
	TYPE_UNKNOWN,
};
static TYPE_BYTE_ORDER HostByteOrder16()
{
	union {
		short  s;
		char   c[sizeof(short)];
	} un;

	un.s = 0x0102;
	if (sizeof(short) == 2) {
		if (un.c[0] == 1 && un.c[1] == 2)
			return TYPE_BIG_ENDIAN;    // 高序字节存储在起始地址
		else if (un.c[0] == 2 && un.c[1] == 1)
			return TYPE_LITTLE_ENDIAN; // 低序字节存储再起始地址
		else
			return TYPE_UNKNOWN;       // printf("unknown\n");
	}
	else
	{
		printf("sizeof(short) = %d\n", sizeof(short));
		return TYPE_UNKNOWN;
	}
}

// 返回网络字节序的值
static uint16_t wrap_htons(uint16_t host16bitvalue)
{
	uint16_t ret = 0;
	ret = htons(host16bitvalue);

	return ret;
}

// 返回网络字节序的值
static uint32_t wrap_htonl(uint32_t host32bitvalue)
{
	uint32_t ret = 0;
	ret = htonl(host32bitvalue);
	
	return ret;
}

// 返回主机字节序的值
static uint16_t wrap_ntohs(uint16_t host16bitvalue)
{
	uint16_t ret = 0;
	ret = ntohs(host16bitvalue);

	return ret;
}

// 返回主机字节序的值
static uint32_t wrap_ntohl(uint32_t host32bitvalue)
{
	uint32_t ret = 0;
	ret = ntohl(host32bitvalue);

	return ret;
}

// 初始化结构体为 0
static void wrap_bzero(void* dest, size_t nbytes)
{
	memset(dest, 0, nbytes);
	return;
}

// 将指定数目的字节从源字节串移到目标字节串
static void Obcopy(const void*src, void* dest, size_t nbytes)
{
	;
}

// 比较两个任意的字节串，若相同则返回值为 0，否则返回非 0
static int Obcmp(const void* ptr1, const void* ptr2, size_t nbytes)
{
	return 0;
}

// 地址转换函数  将点分十进制数串IPv4地址转换为32为的网络字节序二进制地址
static int wrap_inet_aton(const char* strptr, struct in_addr *addrptr)
{
	int ret = 0;
	// Windows 上没有inet_aton，只能用inet_pton第一个参数传入AF_INET代替
	ret = inet_pton(AF_INET, strptr, addrptr);

	return ret;
}

// 地址转换函数  将点分十进制数串IPv4地址转换为32位的网络字节序二进制地址
static bool OIp4ToSockaddr(const char* strptr, struct sockaddr_in* sa4)
{
	uint32_t ret = 0;
	ret = inet_addr(strptr);//已被废弃？

	//若字符串有效则将字符串转换为32位二进制网络字节序的IPV4地址，否则为INADDR_NONE
	if (ret == INADDR_NONE)
	{
		return false;
	}
	sa4->sin_addr.s_addr = ret;
	
	return true;
}

// 地址转换函数  将网络字节序二进制地址转换为点分十进制IPv4地址
static char* wrap_inet_ntoa(struct in_addr inaddr)
{
	return inet_ntoa(inaddr);
}

// IPv4 和 IPv6都适用的函数  但需要调用者识别地址类型
// 点分十进制数ASCII字符串 -> 套接字地址结构中的32位IP地址转
static int OIp46ToSockAddr(const char* saddr, struct sockaddr* addrptr)
{
	int ret = 0;
	int af = addrptr->sa_family;
	/*  inet_pton() returns 1 on success(network address was successfully
		converted).  0 is returned if src does not contain a character string
		representing a valid network address in the specified address family.
		If af does not contain a valid address family, -1 is returned and
		errno is set to EAFNOSUPPORT.*/
	switch (af)
	{
	case AF_INET: {
		struct sockaddr_in * sa4 = (struct sockaddr_in *)addrptr;
		ret = inet_pton(af, saddr, &sa4->sin_addr);
		/*struct in_addr in4;
		inet_pton(AF_INET, saddr, &in4);
		memcpy(&sa4->sin_addr, &in4, sizeof(struct in_addr));*/
		break;
	}
	case AF_INET6: {
		struct sockaddr_in6 * sa6 = (struct sockaddr_in6 *)addrptr;
		ret = inet_pton(af, saddr, &sa6->sin6_addr);
		/*struct in6_addr in6;
		inet_pton(AF_INET6, saddr, &in6);
		memcpy(&sa6->sin6_addr, &in6, sizeof(struct in6_addr));*/
		break;
	}
	default:
		break;
	}
	return ret;
}

// 套接字地址结构中的32位IP地址转 -> 点分十进制数ASCII字符串
static const char* wrap_inet_ntop(int family, const void* addrptr, char* strptr, size_t len)
{
	if (!strptr)// strprt不能为null， 调用者必须分配空间
	{
		return NULL;
	}
	return inet_ntop(family, addrptr, strptr, len);
}


// 协议无关的函数，兼容IPv4 和 IPv6   表达式 -> 字符串
static char* wrap_sock_ntop(const struct sockaddr* sockaddr, socklen_t addrlen)
{
	char		portstr[8];
	static char str[128];		/* Unix domain is largest */

	switch (sockaddr->sa_family) 
	{
	case AF_INET: 
	{
		struct sockaddr_in	*sin = (struct sockaddr_in *) sockaddr;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		
		if (ntohs(sin->sin_port) != 0) {
			snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
			strcat(str, portstr);
		}
		return(str);
	}
	/* end sock_ntop */

	case AF_INET6: 
	{
		printf("enter sock_ntop AF_INET6\n");
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sockaddr;

		str[0] = '[';
		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
			return(NULL);
		if (ntohs(sin6->sin6_port) != 0) {
			snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
			strcat(str, portstr);
			return(str);
		}
		return (str + 1);
	}
	default:
		printf("enter sock_ntop UNKNOWN\n");
		snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sockaddr->sa_family, addrlen);
		return(str);
	}
	return (NULL);
}

// 协议无关的函数，兼容IPv4 和 IPv6   将通配地址和一个临时端口捆绑到一个套接字
static int wrap_sock_bind_wild(int sockfd, int family)
{
	socklen_t	len;

	switch (family) 
	{
	case AF_INET: 
	{
		struct sockaddr_in	sin;

		wrap_bzero(&sin, sizeof(sin));
		sin.sin_family = AF_INET;
		sin.sin_addr.s_addr = htonl(INADDR_ANY);
		sin.sin_port = htons(0);	/* bind ephemeral port */

		if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
			return(-1);
		len = sizeof(sin);
		if (getsockname(sockfd, (struct sockaddr *)&sin, &len) < 0)
			return(-1);
		return(sin.sin_port);
	}
	case AF_INET6: 
	{
		struct sockaddr_in6	sin6;

		wrap_bzero(&sin6, sizeof(sin6));
		sin6.sin6_family = AF_INET6;
		sin6.sin6_addr = in6addr_any;
		sin6.sin6_port = htons(0);	/* bind ephemeral port */

		if (bind(sockfd, (struct sockaddr *)&sin6, sizeof(sin6)) < 0)
			return(-1);
		len = sizeof(sin6);
		if (getsockname(sockfd, (struct sockaddr *)&sin6, &len) < 0)
			return(-1);
		return(sin6.sin6_port);
	}

	}
	return(-1);
}

// 协议无关的函数，兼容IPv4 和 IPv6   比较套接字结构函数
static int wrap_sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family)
		return(-1);

	switch (sa1->sa_family) 
	{
	case AF_INET: 
	{
		return(memcmp(&((struct sockaddr_in *) sa1)->sin_addr, &((struct sockaddr_in *) sa2)->sin_addr,	sizeof(struct in_addr)));
	}
	case AF_INET6: 
	{
		return(memcmp(&((struct sockaddr_in6 *) sa1)->sin6_addr, &((struct sockaddr_in6 *) sa2)->sin6_addr,	sizeof(struct in6_addr)));
	}
	}
	return (-1);
}

// 协议无关的函数，兼容IPv4 和 IPv6   比较两个套接字的端口部分
static int wrap_sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2, socklen_t salen)
{
	if (sa1->sa_family != sa2->sa_family)
		return(-1);

	switch (sa1->sa_family) 
	{
	case AF_INET: 
	{
		return(((struct sockaddr_in *) sa1)->sin_port == ((struct sockaddr_in *) sa2)->sin_port);
	}
	case AF_INET6: 
	{
		return(((struct sockaddr_in6 *) sa1)->sin6_port == ((struct sockaddr_in6 *) sa2)->sin6_port);
	}
	}
	return (-1);
}

// 协议无关的函数，兼容IPv4 和 IPv6   只返回端口号
static int wrap_sock_get_port(const struct sockaddr *sa, socklen_t salen)
{
	switch (sa->sa_family) 
	{
	case AF_INET: 
	{
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		return(sin->sin_port);
	}

	case AF_INET6: 
	{
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		return(sin6->sin6_port);
	}
	}

	return(-1);		/* ??? */
}

// 协议无关的函数，兼容IPv4 和 IPv6   把套接字地址结构中的主机部分转换成表达式（不包括端口号）
static char *wrap_sock_ntop_host(const struct sockaddr *sa, socklen_t salen)
{
	static char str[128];		/* Unix domain is largest */

	switch (sa->sa_family) 
	{
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		if (inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	}

	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		if (inet_ntop(AF_INET6, &sin6->sin6_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return(str);
	}
	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return(str);
	}
	return (NULL);
}

// 协议无关的函数，兼容IPv4 和 IPv6   把套接字地址结构中的地址部分置为addr指针指向的值
static void wrap_sock_set_addr(struct sockaddr *sa, socklen_t salen, const void *addr)
{
	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
		return;
	}

	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
		return;
	}
	}

	return;
}

// 协议无关的函数，兼容IPv4 和 IPv6   设置套接字地址结构的端口部分
static void wrap_sock_set_port(struct sockaddr *sa, socklen_t salen, int port)
{
	switch (sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in	*sin = (struct sockaddr_in *) sa;

		sin->sin_port = port;
		return;
	}

	case AF_INET6: {
		struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;

		sin6->sin6_port = port;
		return;
	}
	}

	return;
}

// 协议无关的函数，兼容IPv4 和 IPv6   把套接字地址结构中的地址部分置为通配地址
static void wrap_sock_set_wild(struct sockaddr *sa, socklen_t salen)
{
	const void	*wildptr;

	switch (sa->sa_family) {
	case AF_INET: {
		static struct in_addr	in4addr_any;

		in4addr_any.s_addr = htonl(INADDR_ANY);
		wildptr = &in4addr_any;
		break;
	}

	case AF_INET6: {
		wildptr = &in6addr_any;
		break;
	}

	default:
		return;
	}
	wrap_sock_set_addr(sa, salen, wildptr);
	return;
}

/* Read "n" bytes from a descriptor. 从一个描述符读取n个字节 */
static ssize_t wrap_readn(int fd, void *vptr, size_t n)
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char	*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nread = UW_Read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR)
				nread = 0;		/* and call read() again */
			else
				return(-1);
		}
		else if (nread == 0)
			break;				/* EOF */

		nleft -= nread;
		ptr += nread; //缓冲区指针移动
	}
	return(n - nleft);		/* return >= 0 */
}

/* Write "n" bytes to a descriptor. */
static ssize_t wrap_writen(int fd, const void *vptr, size_t n)
{
	size_t		nleft;
	ssize_t		nwritten;
	const char	*ptr;

	ptr = (const char	*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = UW_Write(fd, ptr, nleft)) <= 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;		/* and call write() again */
			else
				return(-1);			/* error */
		}

		nleft -= nwritten;
		ptr += nwritten; //缓冲区指针移动
	}
	return(n);
}

// 返回与某个套接字关联的本地协议地址
static int wrap_getsockname(int sockfd, struct sockaddr *localaddr, socklen_t *addrlen)
{
	;
}

// 返回与某个套接字关联的外地协议地址
static int wrap_getpeername(int sockfd, struct sockaddr *peeraddr, socklen_t *addrlen)
{
	;
}

// 返回某个套接字的地址族
static int sockfd_to_family(int sockfd)
{
	struct sockaddr_storage ss;
	socklen_t len;
	
	len = sizeof(ss);
	if (getsockname(sockfd, (struct sockaddr *)&ss, &len) < 0)
	{
		return -1;
	}
	return ss.ss_family;
}
#endif  /*_WRAP_H_*/
