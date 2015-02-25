/**************************************************************** -*-c++-*-
**
**	SecDbTraceContext.h	: base class for trace context
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/SecDbTraceContext.h,v 1.4 2012/06/12 16:59:47 e19351 Exp $
**
****************************************************************/

#if !defined( IN_SECDB_TRACE_CONTEXT_H )
#define IN_SECDB_TRACE_CONTEXT_H

#include    <secdb/base.h>
#include	<limits>

class EXPORT_CLASS_SECDB SecDbTraceContext
{

public:

	static time_t LastAbortCheck;
	SecDbTraceContext();
	SecDbTraceContext( const SecDbTraceContext& TraceContext);
	const SecDbTraceContext& operator= ( const SecDbTraceContext& TraceContext);
	virtual ~SecDbTraceContext() {}
	virtual void Init(	
			int			Level,
			bool		Aborted,
			bool		Suspended,
			bool		Verbose,
			bool		Refresh,
			int			Step,
			double		StartTime
		);

	virtual void End() { LastAbortCheck = std::numeric_limits< time_t >::max(); }
	virtual	void AppendText( const char* String ) = 0;
	virtual	void AppendPointerInfo( void* ptr);
	virtual int OnBreak() = 0;
	virtual void CheckForEsc() = 0;
	virtual	SecDbTraceContext* Dup() = 0;

	virtual int GetLevel() const		{ return m_Level; }
	virtual bool GetAborted() const		{ return m_Aborted; }
	virtual bool GetSuspended() const	{ return m_Suspended; }
	virtual bool GetVerbose() const		{ return m_Verbose; }
	virtual bool GetRefresh() const		{ return m_Refresh; }
	virtual int	GetStep() const			{ return m_Step; }
	virtual double GetStartTime() const	{ return m_StartTime; }

	virtual void SetLevel( int Level ) 			{  m_Level = Level; }
	virtual void SetAborted( bool Aborted )		{  m_Aborted = Aborted; }
	virtual void SetSuspended(bool Suspended ) 	{  m_Suspended = Suspended; }
	virtual void SetVerbose( bool Verbose )		{  m_Verbose = Verbose; }
	virtual void SetStep( int Step ) 			{  m_Step = Step; }
	virtual void SetStartTime( double StartTime) {  m_StartTime = StartTime; }


private :
	int		m_Level;
	bool	m_Aborted;
	bool	m_Suspended;
	bool	m_Verbose;
	int		m_Step;
	bool	m_Refresh;
	double	m_StartTime;

};

#endif
