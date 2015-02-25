/****************************************************************
**
**	SOCKLIB.H	- Definitions for socket library
**
**	Copyright 1994,1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/socklib/src/socklib.h,v 1.36 2012/04/27 14:31:40 e19351 Exp $
**
****************************************************************/

#ifndef IN_SOCKLIB_H
#define IN_SOCKLIB_H

#include <port_int.h> // for uint32_t
#include <tcpip.h>
#include <stdio.h> // for FILE type

/*
**	Constants
*/

int const SOCKET_BUFFER_SIZE      = 1024;

// Socket Name constants
int const SN_HOST_LOCAL			  = 0x0001;
int const SN_HOST_REMOTE	      = 0x0002;
int const SN_PORT_LOCAL			  = 0x0004;
int const SN_PORT_REMOTE		  = 0x0008;

int const SN_NUMERIC			  = 0x0010;

int const SN_LOCAL				  = (SN_HOST_LOCAL  | SN_PORT_LOCAL);
int const SN_REMOTE				  = (SN_HOST_REMOTE | SN_PORT_REMOTE);
int const SN_FULL				  = (SN_REMOTE | SN_LOCAL);

// Set TimeOut to SOCKET_TIMEOUT_BLOCK for blocking reads / polls
double const SOCKET_TIMEOUT_BLOCK = -1;

// Maximum time to spend in any socket call (~1y)
double const SOCKET_TIMEOUT_MAX	  = 31e6;

// Default for SocketBuffer->MaxGap
double const SOCKET_MAXGAP		  = 0.5;


/*
** Macros
*/

#define PORT_IS_INVALID( p )    ( p < 0 || p > 65535 )

/*
**	Type definitions
*/

/* don't use a typedef as it would hinder potential SW6 migration */
struct sockaddr;
#define SOCKADDR sockaddr

struct SOCKET_BUF
{
	int		RefCount,						// Reference count
			Size,							// Number of valid bytes (in READ buffer)
			WriteBufferSize;				// Size of the write buffer
	
	SOCKET	Socket;							// Socket we are open on
	
	char	Buffer[SOCKET_BUFFER_SIZE + 1],	// READ buffer
			*Next;							// Next character to read

	FILE	*fpSocket;						// File version of socket (if buffered)

	double	TimeDue,						// Time due back from read whether finished or not
			MaxGap;							// Secs - Max gap between reads not considered a TimeOut
											// On a read, always poll at least this long before TimeOut
};


/*
**	Return values from polling functions
**	These are explicitly assigned values because functions used to return int
**	Who knows what would break if the values were different?
*/

enum SOCKET_POLL_RESULT
{
	SOCKET_POLL_ERROR		= -1,
	SOCKET_POLL_TIMEOUT		= 0,
	SOCKET_POLL_READY		= 1,
	SOCKET_POLL_ABORT
};



/*
**	Prototypes
*/

DLLEXPORT int
		SocketInit(					void ),
		SocketLoopRead(				SOCKET Socket, char *Buffer, int Len ),
		SocketLoopWrite(			SOCKET Socket, const char *Buffer,
									int Len ),
		SocketGetServiceByName(		const char *Service, const char *Protocol,
									int *ReturnPort ),
		SocketGetHostByName(		const char *HostName,
									uint32_t *ReturnHostAddress ),
		SocketConnect(				SOCKET Socket,
									SOCKADDR *SocketAddress,
									int SocketAddressLength),
		SocketConnectNonBlocking(	SOCKET Socket,
									SOCKADDR *SocketAddress,
									int SocketAddressLength,
									int SecondsToWait ),
		SocketSetNonBlocking(		SOCKET	Socket, int	Set ),
		SocketSetKeepAlive(			SOCKET	Socket, int KeepAlive ),
		SocketSetLinger(			SOCKET	Socket, int	LingerTime );

DLLEXPORT SOCKET
		SocketAccept(				SOCKET Socket, SOCKADDR *PeerAddress,
									CC_SOCKET_SIZE_T *PeerAddressSize ),
		SocketConnectByAddr(		SOCKADDR *SockAddr ),
		SocketConnectHost(			const char *Host, int Port ),
		SocketConnectHostService(	const char *Host, const char *Service ),
		SocketListen(				int Port, bool bReuseAddr = true ),
		SocketListenService(		const char *Service,  bool bReuseAddr = true );

DLLEXPORT SOCKET_POLL_RESULT
		SocketPollInput(			SOCKET Socket, double TimeOut ),
		SocketPollOutput(			SOCKET Socket, double TimeOut ),
		SocketPollInputCheckAbort(	SOCKET Socket, double StaleAfterTime ),
		SocketPollOutputCheckAbort(	SOCKET Socket, double StaleAfterTime );

DLLEXPORT void
		SocketClose(				SOCKET Socket );
	
DLLEXPORT char
		*SocketName(				SOCKET Socket, int Flags, char *Buf, int BufLen ),
		*SocketGetHostName(			void );

DLLEXPORT int
		SockAddrFromHost(			SOCKADDR *SockAddr,	const char *Host, const char *Service, int Port );

DLLEXPORT SOCKET_BUF 
		*SocketBufferDup(			SOCKET_BUF *Buf ),
		*SocketBufferOpen(			SOCKET Socket );

DLLEXPORT int
		SocketBufferClose(			SOCKET_BUF *Buf ),
		SocketBufferCloseSocket(	SOCKET_BUF *Buf ),
		SocketBufferDetach(			SOCKET_BUF *Buf ),
		SocketBufferFill(			SOCKET_BUF *Buf ),
		SocketReadLine(				SOCKET_BUF *Buf, char *LineBuf, int LineSize ),
		SocketReadResult(			SOCKET_BUF *Buf, char *LineBuf, int LineSize ),
		SocketReadBinary(			SOCKET_BUF *Buf, char *BBuf, int Size, int Fill ),
		SocketWriteLine(			SOCKET_BUF *Buf, const char *Line ),
		SocketWritef(				SOCKET_BUF *Buf, const char *Fmt, ... ) ARGS_LIKE_PRINTF(2),
		SocketWrite(				SOCKET_BUF *Buf, const char *Buffer, int size );

DLLEXPORT const char
		*SocketPeerName(			SOCKET_BUF *Buf ),
		*SocketLocalName(			SOCKET_BUF *Buf );

DLLEXPORT int
		SocketBufSetWriteBufferSize(SOCKET_BUF *Buf, int WriteBufferSize ),
		SocketBufLoopWrite(			SOCKET_BUF *Buf, const char *Buffer, int Len );

#endif
