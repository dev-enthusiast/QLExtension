/****************************************************************
**
**	TCPIP.H	- Portable TCP/IP header file
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/portable/src/tcpip.h,v 1.40 2012/10/12 13:43:43 e19351 Exp $
**
**	TCP/IP on OS/2 is an add-on and thus is not fully integrated
**	with the operating system. To ensure portability, follow
**	these rules:
**
**	  *	Always call SockInit() at the beginning of your program
**		and check that it returns TRUE.
**
**	  * Use typedef SOCKET (and constant SOCKET_INVALID)
**
**	  * Use SOCK_WOULD_BLOCK(ErrCode) macro to see if I/O would block
**
**	  * Use SOCK_CONNECT_WOULD_BLOCK(ErrCode) macro to see if a connect
**		request would block
**
**	  * Use SockSend() & SockRecv() instead of read(), write(),
**		send(), or recv() when working with sockets.
**
**	  * Use SockClose() instead of close() with sockets.
**
**	  * Use SOCK_ERRNO when using functions that use or return
**		sockets, such as SockSend() or accept().
**
**	  * Use HOST_ERRNO when using gethostbyname() or gethostbyaddr()
**		functions.
**
**	  * As far as I can tell, getservbyname() and related functions
**		never set any error variable.
**
**	  * Use SockStrError() & HostStrError() to convert SOCK_ERRNO
**		and HOST_ERRNO to error strings.
**
**	  * In .MAK files, include $(TCPIP_MAK) & use $(TCPIPLIB)
**		when linking applications using TCP/IP.
**
****************************************************************/

#if !defined( IN_TCPIP_H )
#define IN_TCPIP_H

#if defined( __unix )

typedef short	SOCKET;
#define SOCKET_INVALID	(-1)
#define SOCKET_IS_INVALID(s_)	( s_ < 0 )

#include <sys/types.h>
#if defined( linux )
#	include <linux/param.h>				// for MAXHOSTNAMELEN
#endif
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <arpa/inet.h>
#define	SOCK_ERRNO		errno
#define	HOST_ERRNO		h_errno
#define	SockClose(S)	close(S)
#define	SockInit()		TRUE
#define	SockSend		send
#define	SockRecv		recv
#define	SockStrError(E) strerror(E)
#define	SOCK_WOULD_BLOCK(X)	((X) == EWOULDBLOCK || (X) == EAGAIN)
#define	SOCK_CONNECT_WOULD_BLOCK(X)	((X) == EINPROGRESS)

extern "C" const char  *HostStrError( int HostErrno );
#if !defined( linux ) 

// on other unices we cannot prevent SIGPIPE (unless we catch the signal)
int const SOCK_SEND_NO_SIGNAL = 0;

#else

// on Linux we can prevent SIGPIPE by doing this
int const SOCK_SEND_NO_SIGNAL = MSG_NOSIGNAL;

#endif


#elif defined( WIN32 )

#	define	SECDB_NO_INCLUDE_WIN_FUNCTIONS
#	include	<winsock2.h>
#	include	<ws2tcpip.h>

#	define	BSD_SELECT
#	define	SOCK_ERRNO		(WSAGetLastError())
#	define	HOST_ERRNO		(WSAGetLastError())
#	define	SockClose(S)	WinSockClose(S)
#	define	SockInit()		(WinSockInit())
#	define	SockSend		WinSockSend
#	define	SockRecv		WinSockRecv
#	define	MAXHOSTNAMELEN	256		// no RFC spec, many systems use 256
#	define	SOCK_WOULD_BLOCK(X)	((X) == WSAEWOULDBLOCK || (X) == WSATRY_AGAIN)
#	define	SOCK_CONNECT_WOULD_BLOCK(X)	((X) == WSAEWOULDBLOCK)
#	define	endservent()

#define SOCKET_INVALID	(INVALID_SOCKET)
#define SOCKET_IS_INVALID(s_)	( s_ == SOCKET_INVALID )

DLLEXPORT const char    *HostStrError( int HostErrno ),
						*SockStrError( int SockErrno );

DLLEXPORT int			WinSockSend( int Socket, const void *Data, int Len, int Flags ),
						WinSockRecv( int Socket,       void *Data, int Len, int Flags ),
						WinSockInit(),
						WinSockClose( int Socket );

// no signals on Windows
int const SOCK_SEND_NO_SIGNAL = 0;

#else
#	error TCP/IP not supported on this platform.

#endif

#endif
