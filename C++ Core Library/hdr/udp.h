/****************************************************************
**
**	UDP.H	- UDP header file
**
**	Copyright 2009 - Constellation Energy Group, Baltimore
**
**	$Header: /home/cvs/src/socklib/src/udp.h,v 1.4 2012/03/22 18:38:54 e19351 Exp $
**
****************************************************************/

#ifndef IN_UDP_H
#define IN_UDP_H

#include	<socklib.h>
#include	<portable.h>

struct UDP_SOCKET
{
	int		Valid;

	SOCKET	Socket;

	struct sockaddr_in 
			Sad;
};

EXPORT_C_EXPORT UDP_SOCKET 
		*UdpInit( const char *ServerName, int Port ); // Initialize socket with server name and port

EXPORT_C_EXPORT void 
		UdpPublish( UDP_SOCKET *UdpSocket, const char *Message ); // Publish message over the socket

#define	UdpClose(S)	SocketClose(S) // Close the socket

#endif

