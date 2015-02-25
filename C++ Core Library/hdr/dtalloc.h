/* $Header: /home/cvs/src/trade/src/dtalloc.h,v 1.16 2000/06/14 15:51:37 goodfj Exp $ */
/****************************************************************
**
**	DTALLOC.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.16 $
**
****************************************************************/

#if !defined( IN_DTALLOC_H )
	#define IN_DTALLOC_H


#ifndef IN_SECDB_H
#include <secdb.h>
#endif
#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

/*
**	Allocation Data Structure
*/

typedef struct TradeAllocationStructure		DT_ALLOCATION;

struct TradeAllocationStructure
{
	DT_ALLOCATION
			*Next;
			
	double	QuantityOrFactor;			  // allocation Qty
	
	char	Book[ SDB_SEC_NAME_SIZE ];	  // Customer Book
	
	char	Entity[ SDB_SEC_NAME_SIZE ];  // The Entity the Customer
										  // Book belongs to
										
	int		TidBit;						  // Typically zero, but increases for multiple allocations for the same book
};


DLLEXPORT DT_DATA_TYPE 	DtAllocation;	  // Initialized by Trade STARTUP message
		
 
DLLEXPORT int DtFuncAllocation(
	int	   		Msg,		// Datatype message
	DT_VALUE	*r,			// Returned value
	DT_VALUE	*a,			// Allocations value to act on
	DT_VALUE	*b			// Argument value
);

DLLEXPORT double DtAllocationsSum( DT_VALUE *Allocations, char *Entity );


#endif
