/* $Header: /home/cvs/src/osecserv/src/secserv/srv_tmap.h,v 1.2 2004/11/29 23:48:10 khodod Exp $ */
/****************************************************************
**
**	srv_tmap.h - transaction map functions interface
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Log: srv_tmap.h,v $
**	Revision 1.2  2004/11/29 23:48:10  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.1  1998/12/16 21:57:08  brownb
**	initial revision
**	internal API to transaction map array
**	
****************************************************************/

#ifndef IN_SECSERV_SRV_TMAP_H
#define IN_SECSERV_SRV_TMAP_H


#include <secserv/secserv.h>            // for SRV_STATE
#include <secdb.h>                      // for SDB_TRANS_ID


// Structure used to map transaction numbers to files

typedef struct SrvTransMapStructure
{
	SDB_TRANS_ID
		StartingTransNum;

	short	Year,
			Month,
			Day,
			Version;

} SRV_TRANS_MAP;



int		SrvTransMapLoad(				char *FileName ),
		SrvTransMapSave(				char *FileName );

SRV_TRANS_MAP
		*SrvTransMapLookup(				SDB_TRANS_ID TransactionID );

void	SrvTransMapAppend(				SRV_TRANS_MAP *SrvTransMap ),
		SrvTransMapNewEntry(			SRV_STATE *State,
										SDB_TRANS_ID StartingTransaction ),
		SrvTransMapDeleteLastEntry(		void );

SRV_TRANS_MAP
		*SrvTransMapGetMapAtOffset(		int SyncPointOffset );

char	*SrvTransMapSyncPointDirectory(	const SRV_TRANS_MAP	*Map );

int		SrvTransMapEntryCount(			void );

SRV_TRANS_MAP
		*SrvTransMapEntryAtIndex(		int Index );

#endif
