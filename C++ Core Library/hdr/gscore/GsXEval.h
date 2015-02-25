/* $Header: /home/cvs/src/gsbase/src/gscore/GsXEval.h,v 1.6 2001/11/27 22:41:11 procmon Exp $ */
/****************************************************************
**
**	gscore/GsXEval.h	- Evaluation exceptions
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsXEval.h,v 1.6 2001/11/27 22:41:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSXEVAL_H )
#define IN_GSCORE_GSXEVAL_H

#include	<gscore/base.h>
#include	<gscore/GsException.h>
#include	<gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	GsException
**		GsXEval		- Virtual base class for all (Slang) evaluation conditions
**			GsXEvalTerminate- Exceptions that terminate
**				GsXEvalError		- Error evaluating
**				GsXEvalAbort		- Abort() encountered
**				GsXEvalTransAbort	- TransactionAbort() encountered
**			GsXEvalLoop		- Classes for loop conditions
**				GsXEvalBreak		- Break; encountered
**				GsXEvalContinue		- Continue; encountered
**			GsXEvalDebug	- Classes for Debugger events
**				GsXEvalRetry		- Debugger event - KLL?
**				GsXEvalBackup		- Debugger event - KLL?
**			GsXEvalValue	- Virtual base class for all (Slang) evaluation conditions with value
**				GsXEvalReturn	- Return( Value ) encountered
**				GsXEvalExit		- Exit( Value ) encountered
**				GsXEvalArgInfo	- ArgInfo requested
*/


class GsXEval;
class GsXEvalTerminate;
class GsXEvalError;
class GsXEvalAbort;
class GsXEvalTransAbort;
class GsXEvalLoop;
class GsXEvalBreak;
class GsXEvalContinue;
class GsXEvalDebug;
class GsXEvalRetry;
class GsXEvalBackup;
class GsXEvalValue;
class GsXEvalReturn;
class GsXEvalExit;
class GsXEvalArgInfo;


GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEval,			GsException		 );

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalTerminate,	GsXEval			 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalError,		GsXEvalTerminate );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalAbort,		GsXEvalTerminate );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalTransAbort,	GsXEvalTerminate );

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalLoop,		GsXEval			 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalBreak,		GsXEvalLoop		 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalContinue,	GsXEvalLoop		 );

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalDebug,		GsXEval			 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalRetry,		GsXEvalDebug	 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalBackup,		GsXEvalDebug	 );

class EXPORT_CLASS_GSBASE GsXEvalValue : public GsXEval
{
	GSX_DECLARE_EXCEPTION( GsXEvalValue, GsXEval );
public:
	// GsXEvalValue also holds a GsDt* as the return value/exit code
	const GsDt*	getValue() { return m_value; }
	GsDt*		stealValue()
	{
		GsDt*	Value = m_value;
		m_value = NULL;
		return Value;
	}
	void setValue( GsDt* Val ) { m_value = Val; }
private:
	GsDt	*m_value;
};


GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalReturn,		GsXEvalValue	 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalExit,		GsXEvalValue	 );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXEvalArgInfo,		GsXEvalValue	 );


CC_END_NAMESPACE
#endif

