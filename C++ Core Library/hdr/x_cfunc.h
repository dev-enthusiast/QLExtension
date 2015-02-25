/* $Header: /home/cvs/src/slang/src/x_cfunc.h,v 1.3 2011/05/17 20:04:36 khodod Exp $ */
/****************************************************************
**
**	X_CFUNC.H	- 'C' Datatype functions
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_X_CFUNC_H )
#define IN_X_CFUNC_H


/*
**	Structure used to contain CFunc datatype information
*/

typedef struct SecCFunc
{
	int		UseCount;		// to controll freeing
	
	char	*DllPath,
			*FuncName;
	
	void  	*FuncPtr;		

} SEC_C_FUNC;



#endif


