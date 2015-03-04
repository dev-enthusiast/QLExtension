#define GSCVSID "$Header: /home/cvs/src/secdb/src/proflink.c,v 1.3 1998/01/02 21:32:46 rubenb Exp $"
/****************************************************************
**
**	PROFLINK.C	- References to force linking for profiler
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Log: proflink.c,v $
**	Revision 1.3  1998/01/02 21:32:46  rubenb
**	added GSCVSID
**
**	Revision 1.2  1996/04/16 21:37:37  gribbg
**	More profiling stuff
**	
**	Revision 1.1  1996/03/25 20:41:57  gribbg
**	Add profiling (currently disabled)
**
**
****************************************************************/

/****************************************************************
**	Routine: Bogus
**	Returns: 
**	Action : 
****************************************************************/

void Bogus( void )
{
	extern void SocketInit( void );				// socklib
	extern void SrvDrvInit( void );				// srv_drv
	extern void SecSrvDriverInit( void );		// srv_ibm

	SocketInit();
	SrvDrvInit();
	SecSrvDriverInit();
}




