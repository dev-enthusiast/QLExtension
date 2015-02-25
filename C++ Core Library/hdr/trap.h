/****************************************************************
**
**	TRAP.H	- Error, abort and shutdown functions
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/kool_ade/src/trap.h,v 1.11 2014/05/13 23:28:05 c038571 Exp $
**
****************************************************************/

#if !defined( IN_TRAP_H )
#define IN_TRAP_H

/*
**	Define types
*/

typedef	void	(*TRAP_FUNC_MATH)(		const char *ErrText, int Continuable );
typedef	void	(*TRAP_FUNC_ABORT)(		void );
typedef void	(*TRAP_FUNC_SHUTDOWN)(	void );
typedef void	(*TRAP_FUNC_TIMER)(		void );


/*
**	Prototype functions
*/

DLLEXPORT int
		TrapMathError( 	TRAP_FUNC_MATH pfMathTrap ),
		TrapAbort(		TRAP_FUNC_ABORT pfAbortTrap ),
		TrapShutdown(	TRAP_FUNC_SHUTDOWN pfShutdownTrap, const char *Name ),
		TrapTimer(		TRAP_FUNC_TIMER pfTimerTrap, double SecondsUntilTrap ),
		TrapFatalSignals(),
	    UnregisterExceptionFilter();
	
		

#endif
