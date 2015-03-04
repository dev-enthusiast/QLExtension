#define GSCVSID "$Header: /home/cvs/src/secdb/src/SecDbTraceStack.cpp,v 1.9 2012/06/12 16:59:47 e19351 Exp $"

/****************************************************************
**
**	SecDbTraceContext.cpp	
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: SecDbTraceStack.cpp,v $
**	Revision 1.9  2012/06/12 16:59:47  e19351
**	Replace a few longs that should be time_t.
**	AWR: #177555
**
**	Revision 1.8  2001/10/13 22:49:55  singhki
**	oops, cannot get object/vt if its a literal
**
**	Revision 1.7  2001/10/02 15:33:26  singhki
**	move object,vt and vti into children. allows us to write better
**	iterators, literals take less space much cooler in general.
**	
**	Revision 1.6  2001/07/18 14:26:28  shahia
**	removed unused headers
**	
**	Revision 1.5  2001/07/17 17:49:42  singhki
**	split children and parents into separate arrays in SDB_NODE. allows us
**	to build cycles because we can add parents while a node is
**	locked. code is much cleaner too.
**	
**	Revision 1.4  2001/07/09 17:58:35  shahia
**	use containment for the vector rather than inheritance
**	
**	Revision 1.3  2001/07/09 17:00:00  shahia
**	take inline funcs out TRACE_BREAK, to reduce header dependency
**	
**	Revision 1.2  2001/07/05 14:42:51  shahia
**	keep linux happy, get rid of warnings
**	
**	Revision 1.1  2001/07/05 13:20:46  shahia
**	Generalized SecDb trace func and moved it to secdb
**	

****************************************************************/

#define BUILDING_SECDB

#include	<portable.h>
#include	<stdio.h>
#include	<string.h>
#include	<vector>
#include	<limits>
#include	<kool_ade.h>
#include	<hash.h>
#include	<secdb.h>
#include	<secexpr.h>
#include	<secobj.h>
#include	<sectrace.h>
#include	<secnode.h>
#include	<secnodei.h>
#include	<secdrv.h>
#include	<secdb/SecDbTraceStack.h>
#include	<secdb/SecDbGenericTraceFunc.h>


TRACE_BREAK::TRACE_BREAK( SDB_NODE *Node, int Message, SDB_OBJECT *Object )
{
	m_Object = NULL;
	if( Object )
	{
		DT_VALUE
				TmpValue;

		TmpValue.DataType = DtSecurity;
		TmpValue.Data.Pointer = Object;
		m_Object = SDB_NODE::FindLiteral( &TmpValue, SecDbRootDb );
	}
	m_ValueType = NULL;
	m_Args = NULL;
	m_Argc = 0;
	if( Node && !Node->IsLiteralQuick() )
	{
		m_Object	= Node->ObjectGet();
		m_ValueType	= Node->ValueTypeGet();
		if(( m_Argc	= Node->ArgcGet() ))
			m_Args	= Node->ArgGetAddr( 0 );
	}
	m_Message = Message;
}

TRACE_BREAK::TRACE_BREAK()
{
	m_Object = m_ValueType = NULL;
	m_Args = NULL;
	m_Message = -1;
	m_Argc = 0;
}


SecDbTraceStack::SecDbTraceStack( SecDbTraceContext* pSampleContext, int TraceStackSize) : 
			m_TraceStackSize( TraceStackSize ),
			m_pSampleContext( pSampleContext )
{ 	
	m_TraceBreakErr = new TRACE_BREAK; 
	
}

bool SecDbTraceStack::IsFull() const	
{ 
	return (int)m_Stack.size() > m_TraceStackSize; 
 
}

SecDbTraceContext* SecDbTraceStack:: Top()
{
	if ( m_Stack.size() == 0 )
		return 0;

	return *(m_Stack.end()-1);
}

SecDbTraceContext* SecDbTraceStack::PushNewContext()
{
	
	SecDbTraceContext* pContext = m_pSampleContext->Dup() ;

	m_Stack.push_back( pContext );
	return pContext;
}

SecDbTraceContext*  SecDbTraceStack::PopContext()
{
	if ( m_Stack.size() == 0 )
		return 0;
	delete *(m_Stack.end()-1);
	m_Stack.pop_back();
	if ( m_Stack.size() == 0 )
		return 0;
	else
		return *(m_Stack.end()-1);

}

void SecDbTraceStack::Init(	
		bool	TraceVerbose,
		bool	TraceStep,
		bool	TraceStopOnErr,
		bool	TraceRefresh )
{
	m_TraceVerbose = TraceVerbose;
	m_TraceStep = TraceStep;
	m_TraceStopOnErr = TraceStopOnErr;
	m_TraceRefresh = TraceRefresh;
}

SecDbTraceStack::~SecDbTraceStack()
{
	delete m_TraceBreakErr;
	for (CC_STL_VECTOR(SecDbTraceContext*)::iterator iter = m_Stack.begin();  iter != m_Stack.end(); ++iter )
		delete *iter;
}

time_t SecDbTraceContext::LastAbortCheck = std::numeric_limits<time_t>::max();


/////////////////////////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////////////////////////

SecDbTraceContext::SecDbTraceContext() :
					m_Level(0),
					m_Aborted(false),
					m_Suspended(false),
					m_Verbose(false),
					m_Step(0),
					m_Refresh(true),
					m_StartTime(0)
{
}

SecDbTraceContext::SecDbTraceContext( const SecDbTraceContext& TraceContext)
{
	SecDbTraceContext::Init(	
			TraceContext.m_Level,
			TraceContext.m_Aborted,
			TraceContext.m_Suspended,
			TraceContext.m_Verbose,
			TraceContext.m_Refresh,
			TraceContext.m_Step,
			TraceContext.m_StartTime
		);
}
	
const SecDbTraceContext& SecDbTraceContext::operator= ( const SecDbTraceContext& TraceContext)
{
	SecDbTraceContext::Init(	
		TraceContext.m_Level,
		TraceContext.m_Aborted,
		TraceContext.m_Suspended,
		TraceContext.m_Verbose,
		TraceContext.m_Refresh,
		TraceContext.m_Step,
		TraceContext.m_StartTime
	);

	return *this;
}

void SecDbTraceContext::Init(	
	int		Level,
	bool	Aborted,
	bool	Suspended,
	bool	Verbose,
	bool	Refresh,
	int		Step,
	double	StartTime
	)
{
	m_Level = Level;
	m_Aborted = Aborted;
	m_Suspended = Suspended;
	m_Verbose = Verbose;
	m_Refresh = Refresh;
	m_Step = Step;
	m_StartTime = StartTime;
}

void SecDbTraceContext::AppendPointerInfo( void* ptr)
{
	char Buffer[256];

	if( ptr )
	{
		sprintf( Buffer, "->%-10p ", ptr );
		AppendText( Buffer );
	}
	else
		AppendText( "             " );
}

