/* $Header: /home/cvs/src/slang/src/x_output.h,v 1.2 1999/06/29 12:57:59 schlae Exp $ */
/****************************************************************
**
**	X_OUTPUT.H	
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_X_OUTPUT_H )
#define IN_X_OUTPUT_H


/*
**	Define types
*/

typedef int		(*PF_OUTPUT_BUFFER_NEW)(		void *Handle, char *Name );
typedef int		(*PF_OUTPUT_BUFFER_BROWSE)(		void *Handle );
typedef int		(*PF_OUTPUT_BUFFER_SAVE)(		void *Handle );
typedef int		(*PF_OUTPUT_BUFFER_OLD)(		void *Handle );
typedef int		(*PF_OUTPUT_BUFFER_SETCOLOR)(	void *Handle, long Color );

#endif


