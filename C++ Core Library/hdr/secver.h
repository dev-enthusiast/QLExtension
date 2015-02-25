/****************************************************************
**
**	SECVER.H	
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secver.h,v 1.5 2001/07/18 20:58:29 singhki Exp $
**
****************************************************************/

#if !defined( IN_SECVER_H )
#define IN_SECVER_H

/*
**	SDB_DISK->VersionInfo layout
**
**		SHTS XXXX XXXX OOOO
**		||||           |
**		||||           +-Object Version (bits 3:0)
**      ||||
**      |||+ Stream Version bit 1 (bit 12)
**		|||
**		||+-Trans Bit (bit 13); if clear this is a SDB_DISK_OLD
**		||
**		|+-Huge Bit (bit 14)
**		|
**		+-Stream Version bit 0 (bit 15)
*/

// Shift amounts
#define SDB_DIS_OBJ_VER		0
#define SDB_DIS_TRANS_BIT	13
#define SDB_DIS_HUGE_BIT	14

// Masks (after shifting)
#define SDB_DIM_OBJ_VER		0x000F
#define SDB_DIM_TRANS_BIT	0x0001
#define SDB_DIM_HUGE_BIT	0x0001
#define SDB_DIM_STREAM_VER	0x0003

// Masks (before shifting)
#define SDB_DIBS_OBJ_VER	( SDB_DIM_OBJ_VER << SDB_DIS_OBJ_VER )
#define SDB_DIBS_TRANS_BIT	( SDB_DIM_TRANS_BIT << SDB_DIS_TRANS_BIT )
#define SDB_DIBS_HUGE_BIT	( SDB_DIM_HUGE_BIT << SDB_DIS_HUGE_BIT )

// Stream version is a little hard to read/write because it was only one bit but I added
// a new one for compressed bstreams so bit 0 is at bit 15 as it used to be and bit 1 is at bit 12

#endif

