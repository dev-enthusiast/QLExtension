/****************************************************************
**
**	SOCKMSG.H	- General Client/Server send/receive based on
**				  BSTREAMs
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/socklib/src/sockmsg.h,v 1.28 2004/11/29 21:08:30 khodod Exp $
**
****************************************************************/

#ifndef IN_SOCKMSG_H
#define IN_SOCKMSG_H


/*
**	PART I: SockMsgBuf
*/

#include	<tcpip.h>
#include	<bstream.h>


/*
**	Buffer size for send/recv of message part header
**		LONG	MessageSize
**		SHORT	MessageType
**		SHORT	MessageMore
*/

#define SOCK_MSG_PART_HEADER_SIZE	8


enum SOCK_MSG_STATUS
{
	SOCKMSG_ERROR,		// SockMsgBufSend/Recv return values
	SOCKMSG_PARTIAL,
	SOCKMSG_DONE

};

enum SOCK_MSG_STATE		// SockMsgBuf state for Send/Recv
{
	SR_START,		// Nothing done yet
	SR_RESTART,		// Rewind to send again
	SR_S_HEADER,	// Start   on header    for PartCurr
	SR_I_HEADER,	// Working on header    for PartCurr
	SR_S_PART,		// Start   on part data for PartCurr
	SR_I_PART,		// Working on part data for PartCurr
	SR_DONE			// Done sending/recieving data

};
	
	
/*
**	Primitive types
*/

typedef int16_t				SOCK_MSG;



/*
**	Message types are 16 bit values.  -32768..0 are reserved for SockMsg
**	library.  1..32767 are available for users.
*/

#define SOCK_MSG_ANY			-1	// Allow any message
#define SOCK_MSG_ERROR_ENCODED	-2	// This response encodes an error
#define	SOCK_MSG_UNDEFINED		 0	// Undefined message received


/*
**	Message structures
*/

struct SOCK_MSG_PART
{
	SOCK_MSG_PART
			*Next;

	int		MemSize,		// Used size of Mem
			MemAlloc;		// Allocated size of Mem

	void	*Mem;
};

struct SOCK_MSG_BUFFER
{
	BSTREAM	BStream;		// BStream used to Get/Put

	SOCKET	Socket;			// Socket to read/write

	SOCK_MSG_PART
			*PartHead,		// Head of all parts
			*PartTail,		// Tail (used for building list)
			*PartCurr;		// Current part (used for send/recv)

	SOCK_MSG
			Type;

	int		SizeEstimate,	// Total bytes estimated at start
			SizeDone;		// Bytes sent/recieved

	char	PartHeader[ SOCK_MSG_PART_HEADER_SIZE ];

	SOCK_MSG_STATE
			State;

	short	FinalPart;		// Recieving final part?

	bool	DeleteMe;		// TRUE if MsgBufDelete() should actually delete
							// this buffer.  FALSE if buffer will be deleted
							// later.  Used by auto-reconnect logic.
};


/*
**	Access macros
*/

#define SockMsgBufBusy( MsgBuf )	((MsgBuf)->State != SR_DONE)


/*
**	Message Functions
*/

DLLEXPORT SOCK_MSG_BUFFER
		*SockMsgBufNew(			SOCK_MSG_BUFFER *MsgBuf, SOCKET Socket );

DLLEXPORT void
		SockMsgBufDelete(		SOCK_MSG_BUFFER *MsgBuf ),
		SockMsgBufFreeData(		SOCK_MSG_BUFFER *MsgBuf );

DLLEXPORT int
		SockMsgBufPrepareRecv(	SOCK_MSG_BUFFER *MsgBuf ),
		SockMsgBufPrepareSend(	SOCK_MSG_BUFFER *MsgBuf, SOCK_MSG Type, 
                                int Estimate );

DLLEXPORT SOCK_MSG_STATUS
		SockMsgBufRecv(			SOCK_MSG_BUFFER *MsgBuf ),
		SockMsgBufSend(			SOCK_MSG_BUFFER *MsgBuf );


/*
**	PART II: SockMsg client/server
*/

#include	<datatype.h>


/*
**	Data structures
*/

struct SOCK_MSG_CONNECTION;
struct SOCK_MSG_Q_ELEM;
struct SOCK_MSG_MASTER;
struct SOCK_MSG_ERR_ELEM;

enum SOCK_MSG_EVENT	// MUST match EventCaseLabels in SlangXSocketMsgServerLoopSwitch().
{
	SOCK_MSG_NEW			= 0,
	SOCK_MSG_READ			= 1,
	SOCK_MSG_ERROR			= 2,
	SOCK_MSG_TIMER			= 3,
	SOCK_MSG_MASTER_ERROR	= 4
	
};

enum SOCK_MSG_CALLBACK_EVENT
{
	SOCK_MSG_CALLBACK_DESTROY	= 0

};


/*
**	Callback to destroy application-specific data in a
**	SOCK_MSG_CONNECTION.     Note that the socket has been closed
**	by the time the function is called.   No socklib functions may
**	be called.
*/
typedef void	(*SOCK_MSG_APP_DATA_CALLBACK)( SOCK_MSG_CONNECTION *Conn, SOCK_MSG_CALLBACK_EVENT Event );


struct SOCK_MSG_CONNECTION
{
	SOCK_MSG_MASTER
			*Master;			// NULL if not managed

	SOCKET	Socket;
	
	SOCK_MSG_Q_ELEM
			*WriteQHead,
			*WriteQTail;

	SOCK_MSG_BUFFER
			*ReadMsgBuf;
			
	SOCK_MSG
			ReadStatus;
			
	unsigned
			NonBlocking:1;

	void	*AppData;			// Application managed data

	SOCK_MSG_APP_DATA_CALLBACK
			AppDataCallback;	// Callback routine for AppData
};

struct SOCK_MSG_MASTER
{
	SOCKET	Socket,
			MinSocket,
			MaxSocket,
			CurSocket,
			NumEvents;
	
	char	*Port;
	
	fd_set	FDescRead,
			FDescWrite,
			FDescTemplate;
			
	double	LastTimer;
	
	SOCK_MSG_ERR_ELEM
			*Errors;
};

struct SOCK_MSG_ERR_ELEM
{
	SOCK_MSG_ERR_ELEM
			*Next;

	SOCKET	Socket;

	int		ErrCode;
	
	char	*ErrText;
};

struct SOCK_MSG_Q_ELEM
{
	SOCK_MSG_Q_ELEM
			*Next;
			
	SOCK_MSG_BUFFER
			*MsgBuf;
};


/*
**	Declare global variable for linger timeout (number of seconds to try
**	flushing pending write buffers for each connection).
*/

DLLEXPORT double	
		SockMsgLingerTimeout;


/*
**	The really simple 3 function client API
*/

DLLEXPORT SOCK_MSG_CONNECTION
		*SockMsgConnect(		const char *Host, const char *Port );

DLLEXPORT int
		SockMsgWriteRead(		SOCK_MSG_CONNECTION *Connection, SOCK_MSG Msg, DT_VALUE *WriteValue, DT_VALUE *ReadValue, double Timeout );

DLLEXPORT void
		SockMsgClose(			SOCK_MSG_CONNECTION *Connection );


/*
**	The rest of the functions
*/

DLLEXPORT SOCK_MSG_CONNECTION 
		*SockMsgLookupSocket(	SOCKET Socket, int CreateFlag );

DLLEXPORT int
		SockMsgEnqueueWrite(	SOCK_MSG_CONNECTION *Connection, SOCK_MSG Msg,     DT_VALUE const *MsgData  ),
		SockMsgDequeueRead(		SOCK_MSG_CONNECTION *Connection, SOCK_MSG *RetMsg, DT_VALUE *RetValue ),
		SockMsgDestroy(			SOCK_MSG_CONNECTION *Connection ),
		SockMsgEnqueueWriteErrStack(	SOCK_MSG_CONNECTION *Connection );

DLLEXPORT SOCK_MSG_STATUS 
		SockMsgWrite(			SOCK_MSG_CONNECTION *Connection, double Timeout ),
		SockMsgRead(			SOCK_MSG_CONNECTION *Connection, double Timeout );

DLLEXPORT SOCK_MSG_BUFFER
		*SockMsgWriteBegin( SOCK_MSG_CONNECTION *Connection, SOCK_MSG Msg, size_t SizeEstimate );

DLLEXPORT GsStatus
		SockMsgWriteComplete(	SOCK_MSG_CONNECTION *Connection, SOCK_MSG_BUFFER *MsgBuf ),
		SockMsgWriteReset(		SOCK_MSG_CONNECTION *Connection, SOCK_MSG_BUFFER *MsgBuf );

DLLEXPORT BSTREAM
		*SockMsgReadBegin( SOCK_MSG_CONNECTION *Connection, SOCK_MSG *RetMsg );

DLLEXPORT int
		SockMsgReadComplete( SOCK_MSG_CONNECTION *Connection );

DLLEXPORT SOCK_MSG_MASTER
		*SockMsgServerStartup(	const char *Port );

DLLEXPORT int
		SockMsgServerShutdown(	SOCK_MSG_MASTER *Master ),
		SockMsgServerAddSocket(	SOCK_MSG_MASTER *Master, SOCKET Socket ),
		SockMsgServerNextEvent(	SOCK_MSG_MASTER *Master, double Timer, SOCK_MSG_EVENT *RetEvent, SOCKET *RetSocket );

DLLEXPORT void
		SockMsgError( SOCK_MSG_MASTER *Master, SOCKET Socket );


/* Routines for managing buffer limits */
DLLEXPORT size_t
		SockMsgBufSetMaxBufferSize( size_t NewValue ),
		SockMsgBufGetMaxBufferSize( void ),
		SockMsgBufSetMaxTotalBufferSize( size_t NewValue ),
		SockMsgBufGetMaxTotalBufferSize( void ),
		SockMsgBufGetCurrentTotalBufferSize( void );

#endif

