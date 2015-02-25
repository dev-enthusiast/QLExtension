/****************************************************************
**
**	TSDBUDP.H	- TSDBUDP header file
**
**	Copyright 2009 - Constellation Energy Group, Baltimore
**
**	$Header: /home/cvs/src/tsdb/src/tsdbudp.h,v 1.6 2013/10/01 21:36:14 khodod Exp $
**
****************************************************************/

#ifndef IN_TSDBUDP_H
#define IN_TSDBUDP_H

#include	<turbo.h>
#include	<udp.h>

struct TSDB_UDP_INFO
{
	UDP_SOCKET
			*UdpSocket;

	const char	
            *User,
			*AppName,
			*Host,
			*Process;
};


DLLEXPORT void TsdbUdpInit( void ); // Initialize UDP socket connection and basic information
DLLEXPORT void TsdbUdpClose( void ); // Close UDP socket connection and free hash and other variables
DLLEXPORT void TsdbUdpSend( SYMBOL *TsdbSymbol, int opType ); // Send message that will be constructed from symbol information
DLLEXPORT void *TsdbGetUdpHash( void ); // Get pointer to UDP message cache

#endif

