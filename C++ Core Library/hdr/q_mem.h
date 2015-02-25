/* $Header: /home/cvs/src/quant/src/q_mem.h,v 1.3 1998/10/09 22:22:52 procmon Exp $ */
/****************************************************************
**
**	Q_MEM.H	- Something very silly for use in memory allocation
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.3 $
**
****************************************************************/

#if !defined( IN_Q_MEM_H )
#define IN_Q_MEM_H
#include	<malloc.h>

#ifdef MEMCHECK
void	*memChkCalloc(size_t n, size_t size);
void	*memChkMalloc(size_t n);
void	memChkFree(void *);
long	memChkUsed ();
#define	CALLOC(n,size)	memChkCalloc(n,size)
#define	MALLOC(n)		memChkMalloc(n)
#define	FREEMEM(p)		memChkFree(p)

#else
#define	CALLOC(n,size)	calloc(n,size)
#define	MALLOC(n)		malloc(n)
#define	FREEMEM(p)		free(p)
#endif

/*
void	*hxCalloc (size_t	numItem, size_t size, HX_HND *pHandle);
int	hxFreeMem (HX_HND handle);
*/

#endif

