/****************************************************************
**
**	rtsdb/rtsdb.h	- RTsdb API
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/rtsdb/src/rtsdb/rtsdb.h,v 1.8 2001/11/27 23:15:59 procmon Exp $
**
****************************************************************/

#if !defined( IN_RTSDB_RTSDB_H )
#define IN_RTSDB_RTSDB_H

#include	<rtsdb/base.h>
#include	<sys/types.h>

CC_BEGIN_NAMESPACE( Gs )

/*
	Handle = RTsdbOpen( DbName, UserName, OpenFlags )
	RTsdbClose( RTsdbHandle )

	RTsdbDataGet( RTsdbHandle, BaseSymbol, Mode, Data )
	RTsdbDataSet( RTsdbHandle, BaseSymbol, Mode, Data )
	RTsdbModeSet( RTsdbHande, NewMode )

	DbName:		Name of desired real-time gateway
	BaseSymbol:		e.g., "swp_demsw5y"
	
	Mode contains:
		Suffix:		"_L" for last point, "_LDN" for london close, etc.
		Date:		[NotYet] Date to pull from tsdb series (not used if mode is "L")
		Prefix:		[NotYet] Prefix for each symbol, e.g., "swp" --> "swp_demsw5y_ldn"

	Initially only "L" mode will be supported.
*/


/*
**	From spreadsheet:
**
**	=RtOpen( "Non-default Server" )
**	=Rt( "Symbol" )
**	=RtX( "Symbol", Suffix , Date )
**	=RtModeSet( Suffix, Date )
**	=RtModeGetDate()
**	=RtModeGetSuffix()
**	
*/

#define RTSDB_NAME_SIZE 32

typedef struct RTsdbDataStruct
{
	time_t	LastUpdate;
	double	Value;
} RTSDB_DATA;

typedef struct RTsdbSymbolNameStruct
{
	char	Name[ RTSDB_NAME_SIZE ];
} RTSDB_SYMBOL_NAME;

typedef struct RTsdbSymbolStruct
{
	RTSDB_SYMBOL_NAME	Name;
	RTSDB_DATA			Data;
} RTSDB_SYMBOL;

#define RTSDB_OPEN_READ  0x0001
#define RTSDB_OPEN_WRITE 0x0002
#define RTSDB_OPEN_SUPER 0x1000
typedef int RTSDB_OPEN_FLAGS;

typedef struct RTsdbHandleStruct RTSDB_HANDLE;
typedef struct RTsdbEnumStruct   RTSDB_ENUM;
typedef struct RTsdbModeStruct   RTSDB_MODE;


// Open & Close the server
EXPORT_C_RTSDB RTSDB_HANDLE
		*RTsdbOpen( const char *DbName, const char *UserId, RTSDB_OPEN_FLAGS Flags );

EXPORT_C_RTSDB GsStatus
		RTsdbOpenIncrement(	RTSDB_HANDLE *Handle ),
		RTsdbClose(			RTSDB_HANDLE *Handle );

EXPORT_C_RTSDB const char
		*RTsdbHandleName(	RTSDB_HANDLE *Handle );

EXPORT_C_RTSDB GsBool
		RTsdbIsConnected(	RTSDB_HANDLE *Handle );

// Mode APIs
EXPORT_C_RTSDB GsStatus
		RTsdbModeGet( RTSDB_HANDLE *Handle,       RTSDB_MODE *Mode ),
		RTsdbModeSet( RTSDB_HANDLE *Handle, const RTSDB_MODE *Mode );

EXPORT_C_RTSDB RTSDB_MODE
		*RTsdbModeNew( void );

EXPORT_C_RTSDB void
		RTsdbModeDelete(		  RTSDB_MODE *Mode );

EXPORT_C_RTSDB GsStatus
		RTsdbModeSuffixGet( const RTSDB_MODE *Mode,       char *Suffix ),
		RTsdbModeSuffixSet(       RTSDB_MODE *Mode, const char *Suffix );

// Read/write data
EXPORT_C_RTSDB GsStatus
		RTsdbDataGet( RTSDB_HANDLE *Handle, const char *SymbolName, const RTSDB_MODE *Mode,       RTSDB_DATA *Data ),
		RTsdbDataSet( RTSDB_HANDLE *Handle, const char *SymbolName, const RTSDB_MODE *Mode, const RTSDB_DATA *Data );

// Delete symbols [NotYet]
EXPORT_C_RTSDB GsStatus
		RTsdbDataDel( RTSDB_HANDLE *Handle, const char *SymbolName );


// Enumerate symbols [NotYet]
// Use it like this:
// RTSDB_ENUM *R = RTsdbEnumFirst( ... );
// for( ; RTSDB_ENUM_MORE(R); RTsdbEnumNext(R))
//	printf( "Symbol is %s\n", RTSDB_ENUM_SYMBOL(R) );
EXPORT_C_RTSDB RTSDB_ENUM *RTsdbEnumFirst( RTSDB_HANDLE *Handle, const char *StartHere );
EXPORT_C_RTSDB GsBool RTsdbEnumNext( RTSDB_ENUM *Enum );
EXPORT_C_RTSDB GsBool RTsdbEnumDone( RTSDB_ENUM *Enum );
#define RTSDB_ENUM_MORE(ENUM) ...
#define RTSDB_ENUM_SYMBOL(ENUM) ...

CC_END_NAMESPACE
#endif

