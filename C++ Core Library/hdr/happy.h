/* $Header: /home/cvs/src/portable/src/happy.h,v 1.7 2012/09/04 14:21:02 e19351 Exp $ */
/****************************************************************
**
**	HAPPY.H
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_HAPPY_H )
#define IN_HAPPY_H

#define	HAPPY_ALLOC_SIZE_PTR(Ptr)	(*(((size_t *)Ptr) - 1))

#endif

