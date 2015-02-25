/* $Header: /home/cvs/src/tsdb/src/tsdbupdt.h,v 1.1 2000/04/07 09:48:53 goodfj Exp $ */
/****************************************************************
**
**	TSDBUPDT.H
**
**	$Id: tsdbupdt.h,v 1.1 2000/04/07 09:48:53 goodfj Exp $
**
****************************************************************/

/*
**	define error codes for updating
*/

#define	UPDATE_ALLOC_ERROR			1
#define	UPDATE_CREATE_ERROR 		2
#define	UPDATE_REMOVE_ERROR 		3
#define	UPDATE_RENAME_ERROR 		4
#define	UPDATE_READ_ERROR			5
#define	UPDATE_WRITE_ERROR			6
#define	UPDATE_SORTED_ERROR			7
#define UPDATE_UNSUPPORTED_ERROR	8
#define UPDATE_CREATE_TEMP_ERROR    9
#define UPDATE_INSERT_ERROR        10

/*
** Update type flags
*/

#define UPDATE_NO_SYNC 		0x01
#define UPDATE_NO_INSERT	0x02

