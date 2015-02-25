/* $Header: /home/cvs/src/osecserv/src/secserv/srv_io.h,v 1.2 2004/12/08 17:23:51 khodod Exp $ */
/****************************************************************
**
**	srv_io.h - functions for endian-independent file I/O
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: srv_io.h,v $
**	Revision 1.2  2004/12/08 17:23:51  khodod
**	Correctly read and write 64-bit translog offsets.
**	iBug: #16863
**
**	Revision 1.1  2004/11/29 23:48:10  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**	
****************************************************************/


#ifndef IN_SECSERV_SRV_IO_H
#define IN_SECSERV_SRV_IO_H


#include <secserv/srv_tmap.h>           // for SRV_TRANS_MAP
#include <secindex.h>                   // for SDB_INDEX_xxxx
#include <secdb.h>                      // for SDB_DISK

#include <port_int.h>
#include <stdio.h>


// Sizes of application name and user name as stored in the transaction log.

int const TRANSLOG_APP_NAME_SIZE = 16;
int const TRANSLOG_USER_NAME_SIZE = 16;


// Sizes of data structures when stored on disk.

int const STORED_SDB_DISK_SIZE = SDB_SEC_NAME_SIZE + 24;

int const STORED_TRANS_HEADER_SIZE
        = ( 6 * 4 + TRANSLOG_USER_NAME_SIZE + TRANSLOG_APP_NAME_SIZE
            + SDB_SEC_NAME_SIZE + 12 + 4 * 4 + 20 );

int const STORED_INDEX_DESCRIPTOR_SIZE
        = ( SDB_INDEX_NAME_SIZE + 4 
            + ( SDB_MAX_INDEX_PARTS
                * ( SDB_VALUE_NAME_SIZE + DT_DATA_TYPE_NAME_SIZE + 6 )));


// Routines that read and write data to a file using network byte order.


int ReadInt16( FILE *fp, int16_t *result );
int WriteInt16( FILE *fp, int16_t value );

int ReadUInt16( FILE *fp, uint16_t *result );
int WriteUInt16( FILE *fp, uint16_t value );

int ReadInt32( FILE *fp, int32_t *result );
int WriteInt32( FILE *fp, int32_t value );

int ReadUInt32( FILE *fp, uint32_t *result );
int WriteUInt32( FILE *fp, uint32_t value );

int ReadUInt64( FILE *fp, uint64_t *result );
int WriteUInt64( FILE *fp, uint64_t value );

int ReadSdbDisk( FILE *fp, SDB_DISK& result );
int WriteSdbDisk( FILE *fp, SDB_DISK& info );

int ReadTransMapEntry( FILE *fp, SRV_TRANS_MAP& Map );
int WriteTransMapEntry( FILE *fp, SRV_TRANS_MAP& Map );

int ReadIndexDescriptor( FILE *fp, SDB_INDEX_DESCRIPTOR& result );
int WriteIndexDescriptor( FILE *fp, SDB_INDEX_DESCRIPTOR& descriptor );

int ReadTransHeader( FILE *fp, SDB_TRANS_HEADER& header );
int WriteTransHeader( FILE *fp, SDB_TRANS_HEADER& header );


#endif
