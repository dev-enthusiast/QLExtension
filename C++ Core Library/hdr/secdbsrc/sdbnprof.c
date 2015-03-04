#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdbnprof.c,v 1.18 2004/11/29 23:48:12 khodod Exp $"
/****************************************************************
**
**	sdbnprof.c	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: sdbnprof.c,v $
**	Revision 1.18  2004/11/29 23:48:12  khodod
**	64-bit fixes for the secserv build.
**	iBug: #16863
**
**	Revision 1.17  2001/07/20 14:48:38  simpsk
**	remove last traces of dynamic numbers, pointers, and strings.
**	
**	Revision 1.16  2001/04/11 22:25:41  simpsk
**	Add NO_DYNAMIC_PTRS switch.
**	
**	Revision 1.15  2000/05/18 02:37:23  singhki
**	Log non terminals too. Reorganize map to map all nodes as node type may not be determined before a build children
**	
**	Revision 1.14  2000/04/24 11:04:27  singhki
**	Rewrite all internal Node operations as members of SDB_NODE.
**	
**	Revision 1.13  2000/02/08 04:50:29  singhki
**	Rename all SDB_NODE members as m_XXX and define accessor functions for
**	them all.
**	
**	Revision 1.12  1999/12/30 13:27:38  singhki
**	don't crash if file open fails
**	
**	Revision 1.11  1999/12/23 17:29:53  singhki
**	no need to log SPLIT_EXTEND messages for profiler
**	
**	Revision 1.10  1999/12/21 19:28:39  singhki
**	Use int64 on NT and long long on unix. Use binary mode on log file in NT
**	
**	Revision 1.9  1999/11/17 17:48:38  singhki
**	Oops, totally forgot about OS/2
**	
**	Revision 1.8  1999/11/16 21:25:31  singhki
**	Use GetProcessTimes on NT
**	
**	Revision 1.7  1999/09/01 15:29:35  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.6  1999/08/18 18:08:51  singhki
**	spit out profile info for non terminals too
**	
**	Revision 1.5  1999/08/12 14:14:57  singhki
**	Use SEEK_END to get size of file
**	
**	Revision 1.4  1999/08/10 15:43:45  singhki
**	Write profile info to a log file
**	
**	Revision 1.3  1999/07/27 20:44:26  singhki
**	include sys/time.h too
**	
**	Revision 1.2  1999/07/27 20:32:29  singhki
**	use gettimeofday
**	
**	Revision 1.1  1999/07/27 16:07:34  singhki
**	secdb node profiling
**	
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_FILEIO
#include <portable.h>
#include <kool_ade.h>
#include <hash.h>
#include <datatype.h>
#include <secdb.h>
#include <secnode.h>
#include <secnodei.h>
#include <sdbnprof.h>
#include <time.h>
#if defined( __unix )
#include <sys/time.h>
#include <sys/times.h>
#elif defined( WIN32 )
#include <windows.h>
#endif
#include <sys/types.h>
#include <sys/timeb.h>
#include <fcntl.h>

/*
**	private typedefs
*/

#if defined( WIN32 )

#define WRITE_MODE "w+b"
#define OPEN_FLAGS O_BINARY
typedef __int64 TimeVal;

#elif defined( __unix )

#define WRITE_MODE "w+"
#define OPEN_FLAGS 0
typedef long long TimeVal;

#else

#define WRITE_MODE "w+"
#define OPEN_FLAGS 0
typedef long TimeVal;

#endif



/****************************************************************
**	Routine: GetUserTime
**	Returns: 
**	Action : 
****************************************************************/

inline TimeVal GetUserTime(
)
{
#if defined( __unix )
	struct timeval
			tp;

	gettimeofday( &tp, NULL );

	return TimeVal( tp.tv_sec ) * 1000000 + tp.tv_usec;
#elif defined( WIN32 )
	// The clock() function does not work as advertised.
	static HANDLE Process = GetCurrentProcess();
	FILETIME CreationTime, ExitTime, KernelTime, UserTime;

	GetProcessTimes( Process, &CreationTime, &ExitTime, &KernelTime, &UserTime );

	TimeVal time = ( ((TimeVal) UserTime.dwHighDateTime << 32) + UserTime.dwLowDateTime
					 + ((TimeVal) KernelTime.dwHighDateTime << 32) + KernelTime.dwLowDateTime ) / 10;

	return time;
#else
	return clock();
#endif
}

struct ProfileLogEntry
{
	unsigned
			NodeId	: 24,
			Msg		: 8;

	unsigned
			Elapsed;
};

class ProfileLog
{
public:
	ProfileLog( const char *ProfileName, int BufferSize = 1048576 )
	{
		this->LogFileName 	= strdup( ProfileName );
		this->MapFileName 	= StrPaste( ProfileName, ".map", NULL );
		this->Log 			= open( this->LogFileName, O_RDWR | O_CREAT | OPEN_FLAGS, 0666 );
		this->MapFile 		= fopen( this->MapFileName, WRITE_MODE );

		this->Buffer 		= new ProfileLogEntry[ BufferSize ];
		this->BufferPtr		= this->Buffer;
		this->BufferMax		= this->Buffer + BufferSize;

		NodeIdHash			= HashCreate( "ProfileNodeIds", NULL, NULL, 0, NULL );
		NextNodeId			= 1;
		TimeStamp			= GetUserTime();

		int MapSize = 0;
		if( Ok() )
			write( this->Log, &MapSize, sizeof( MapSize ));
	}
	~ProfileLog()
	{
		free( LogFileName );

		if( MapFile )
		{
			fflush( MapFile );
			FlushLog();

			ProfileLogEntry LastEntry;
			LastEntry.NodeId = 0;
			LastEntry.Msg = SDB_TRACE_LAST_MESSAGE_NUM;
			write( Log, &LastEntry, sizeof( LastEntry ));
		
			int MapPos = lseek( Log, 0, SEEK_END );
			lseek( Log, 0, SEEK_SET );
			write( Log, &MapPos, sizeof( MapPos ));
			lseek( Log, MapPos, SEEK_SET );

			rewind( MapFile );
			char Buf[ 16384 ];
			int Num;
			while( ( Num = fread( Buf, 1, 16384, MapFile )))
				write( Log, Buf, Num );

			close( Log );		
			fclose( MapFile );
			unlink( MapFileName );
		}

		free( MapFileName );
		HashDestroy( NodeIdHash );
	}

	void FlushLog()
	{
		if( BufferPtr != Buffer )
		{
			write( Log, Buffer, sizeof( ProfileLogEntry ) * ( BufferPtr - Buffer ));
			BufferPtr = Buffer;
		}
	}

	void Append( SDB_NODE *Node, int Msg )
	{
		TimeVal Elapsed = GetUserTime() - TimeStamp;

		unsigned NodeId = (unsigned long) HashLookup( NodeIdHash, Node );
		if( !NodeId )
			NodeId = MapNode( Node );

		BufferPtr->NodeId	= NodeId;
		BufferPtr->Msg		= Msg;
		BufferPtr->Elapsed	= (unsigned) Elapsed;
		if( ++BufferPtr == BufferMax )
		{
			FlushLog();
			BufferPtr = Buffer;
		}
		TimeStamp = GetUserTime();
	}

	void DestroyNode( SDB_NODE *Node )
	{
		HashDelete( NodeIdHash, (HASH_KEY) Node );
	}

	bool Ok()
	{
		return MapFile != NULL && Log != -1;
	}

public:
	char	*MapFileName,
			*LogFileName;

private:
	FILE	*MapFile;

	int		Log,
			NextNodeId;

	ProfileLogEntry
			*Buffer,
			*BufferPtr,
			*BufferMax;

	char	NodeName[ 512 ];

	TimeVal	TimeStamp;

	HASH	*NodeIdHash;

	int MapNode( SDB_NODE *Node )
	{
		int NodeId = NextNodeId++;

		HashInsert( NodeIdHash, (HASH_KEY) Node, (HASH_VALUE) NodeId );

		if( !SDB_NODE::IsNode( Node ))
			return NodeId;

		char Type;
		switch( Node->NodeTypeGet() )
		{
			case SDB_NODE_TYPE_REF_COUNTED_LITERAL:
			case SDB_NODE_TYPE_PARENTED_LITERAL:
			{
				DT_VALUE
						TmpVal;

				TmpVal.DataType = DtString;
				if( DTM_TO( &TmpVal, Node->AccessValue() ))
				{
					fwrite( &NodeId, sizeof( NodeId ), 1, MapFile );
					Type = 'L';
					fputc( Type, MapFile );

					int size = strlen( (const char *) TmpVal.Data.Pointer );
					fwrite( &size, sizeof( size ), 1, MapFile );
					fwrite( TmpVal.Data.Pointer, size, 1, MapFile );
					DTM_FREE( &TmpVal );
				}
				break;
			}

			case SDB_NODE_TYPE_EACH:
			case SDB_NODE_TYPE_TERMINAL:
			case SDB_NODE_TYPE_ARRAY:
			default:
			{
				int		ObjectId, VtId, ArgId, Arg;

				if( !( ObjectId = (unsigned long) HashLookup( NodeIdHash, Node->ObjectGet() )))
					ObjectId = MapNode( Node->ObjectGet() );
				if( !( VtId = (unsigned long) HashLookup( NodeIdHash, Node->ValueTypeGet() )))
					VtId = MapNode( Node->ValueTypeGet() );

				for( Arg = 0; Arg < Node->ArgcGet(); ++Arg )
					if( !( HashLookup( NodeIdHash, Node->ArgGet( Arg ))))
						MapNode( Node->ArgGet( Arg ));

				fwrite( &NodeId, sizeof( NodeId ), 1, MapFile );
				Type = 'T';
				fputc( Type, MapFile );

				Arg = Node->ArgcGet();
				fwrite( &Arg, sizeof( Arg ), 1, MapFile );
				fwrite( &ObjectId, sizeof( ObjectId ), 1, MapFile );
				fwrite( &VtId, sizeof( VtId ), 1, MapFile );

				for( Arg = 0; Arg < Node->ArgcGet(); ++Arg )
				{
					ArgId = (unsigned long) HashLookup( NodeIdHash, Node->ArgGet( Arg ));
					fwrite( &ArgId, sizeof( ArgId ), 1, MapFile );
				}
				break;
			}
		}
		return NodeId;
	}
};

/*
**	private variables
*/

static int
		ProfileMessage[ SDB_TRACE_LAST_MESSAGE_NUM ];

static ProfileLog
		*Log;


/****************************************************************
**	Routine: TraceFunc
**	Returns: 
**	Action : 
****************************************************************/

static int TraceFunc(
	SDB_TRACE_MSG	Msg,
	...
)
{
	va_list	Marker;

	SDB_NODE
			*Node;

	if( !ProfileMessage[ Msg ])
		return TRUE;

	va_start( Marker, Msg );
	Node = va_arg( Marker, SDB_NODE * );
	va_end( Marker );

	if( Msg == SDB_TRACE_DESTROY )
	{
		Log->DestroyNode( Node );
		return TRUE;
	}

	Log->Append( Node, Msg );
	return TRUE;
}



/****************************************************************
**	Routine: SecDbProfileStart
**	Returns: TRUE / Err
**	Action : Turns on trace profiling
****************************************************************/

int SecDbProfileStart(
	const char	*ProfileName
)
{
	if( Log )
		return Err( ERR_UNSUPPORTED_OPERATION, "Profiling is already on" );

	if( SecDbTraceFunc )
		return Err( ERR_UNSUPPORTED_OPERATION, "Trace is on, cannot profile at the same time" );

	Log = new ProfileLog( ProfileName );
	if( !Log->Ok() )
	{
		delete Log;
		Log = NULL;
		return ErrMore( "ProfileStart failed!" );
	}

	for( int i = 0; i < SDB_TRACE_LAST_MESSAGE_NUM; ++i )
		ProfileMessage[ i ] = 0;

	ProfileMessage[ SDB_TRACE_INVALIDATE ] =
			ProfileMessage[ SDB_TRACE_DONE_INVALIDATE ] =

			ProfileMessage[ SDB_TRACE_INVALIDATE_CHILDREN ] =
			ProfileMessage[ SDB_TRACE_DONE_INVALIDATE_CHILDREN ] =

			ProfileMessage[ SDB_TRACE_BUILD_CHILDREN ] =
			ProfileMessage[ SDB_TRACE_DONE_BUILD_CHILDREN ] =

			ProfileMessage[ SDB_TRACE_GET ] =
			ProfileMessage[ SDB_TRACE_VALID ] =
			ProfileMessage[ SDB_TRACE_CALC ] =
			ProfileMessage[ SDB_TRACE_EVAL_END ] =

			ProfileMessage[ SDB_TRACE_NODE_SPLIT ] =
			ProfileMessage[ SDB_TRACE_NODE_SPLIT_DONE ] =

			ProfileMessage[ SDB_TRACE_DESTROY ] =

			TRUE;

	SecDbTraceFunc = TraceFunc;
	return TRUE;
}



/****************************************************************
**	Routine: SecDbProfileEnd
**	Returns: TRUE / Err
**	Action : ends profiling, and throws away all data
****************************************************************/

int SecDbProfileEnd(
)
{
	if( !SecDbTraceFunc || SecDbTraceFunc != TraceFunc || !Log )
		return Err( ERR_UNSUPPORTED_OPERATION, "Profiling is not on" );

	delete Log;
	Log = NULL;
	SecDbTraceFunc = NULL;

	return TRUE;
}
