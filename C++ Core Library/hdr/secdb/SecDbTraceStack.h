/****************************************************************
**
**	SecDbTraceStack.h	
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: SecDbTraceStack.h,v $
**	Revision 1.3  2001/07/09 17:58:36  shahia
**	use containment for the vector rather than inheritance
**
**	Revision 1.2  2001/07/09 17:00:00  shahia
**	take inline funcs out TRACE_BREAK, to reduce header dependency
**	
**	Revision 1.1  2001/07/05 13:20:49  shahia
**	Generalized SecDb trace func and moved it to secdb
**	

****************************************************************/

#if !defined( IN_SECDB_TRACE_STACK_H )
#define IN_SECDB_TRACE_STACK_H



#include    <secdb/base.h>
#include    <secdb/SecDbTraceContext.h>
#include    <vector>
#include    <ccstl.h>

class SDB_NODE;
struct SDB_OBJECT;


struct EXPORT_CLASS_SECDB TRACE_BREAK
{
	TRACE_BREAK( SDB_NODE *Node, int Message, SDB_OBJECT *Object = NULL );
	
	TRACE_BREAK();
	
	SDB_NODE
			*m_Object,
			*m_ValueType;

	int		m_Message,
			m_Argc;

	SDB_NODE
			**m_Args;
};




class EXPORT_CLASS_SECDB SecDbTraceStack
{
public: 
	typedef	SecDbTraceContext* value_type;

	SecDbTraceStack(SecDbTraceContext* pSampleContext, int TraceStackSize );
	void Init(	bool	TraceVerbose,
			bool	TraceStep,
			bool	TraceStopOnErr,
			bool	TraceRefresh
			);
	bool GetVerbose() const		{ return m_TraceVerbose; }
	bool GetStep() const		{ return m_TraceStep; }
	bool GetStopOnErr() const	{ return m_TraceStopOnErr; }
	bool GetRefresh() const		{ return m_TraceRefresh; }
	int GetStackSize() const	{ return m_TraceStackSize; }
	int	size() const			{ return m_Stack.size() ; }
	SecDbTraceContext*	GetSampleContext() { return m_pSampleContext; }
	TRACE_BREAK * GetTraceBreakErr() { return m_TraceBreakErr; }

	bool IsFull() const;

	SecDbTraceContext* Top();

	SecDbTraceContext* PushNewContext();

	SecDbTraceContext* PopContext();

	~SecDbTraceStack();
private:

		CC_STL_VECTOR(SecDbTraceContext*) m_Stack;

		bool	m_TraceVerbose;
		bool	m_TraceStep;
		bool	m_TraceStopOnErr;
		bool	m_TraceRefresh;
		int		m_TraceStackSize;
		SecDbTraceContext*	m_pSampleContext;
		TRACE_BREAK *m_TraceBreakErr;
	

};

#endif
