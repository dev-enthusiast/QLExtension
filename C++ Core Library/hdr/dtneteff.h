/****************************************************************
**
**	dtneteff.h - NET_EFFECTS datatype
**
**	Copyright 1997-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/trade/src/dtneteff.h,v 1.17 2000/06/12 23:53:20 kimbet Exp $
**
****************************************************************/

#if !defined( IN_DTNETEFF_H )
#define IN_DTNETEFF_H

#include <trade.h>


#define NETEFF_BOOK_NODES_EQUAL( a, b )    ( (0 == stricmp( (a)->Book, (b)->Book )) \
												&& ( ((a)->TidBit < 0 && (b)->TidBit < 0) || ((a)->TidBit == (b)->TidBit && (a)->AllocationId == (b)->AllocationId)) )
												
/*
**	Net Effects data structures (see trade.h for typedefs)
*/

struct NetEffectsBookStructure
{
	NET_EFFECTS_BOOK
			*Next;
			
	char	Book[ SDB_SEC_NAME_SIZE ];

	double	Factor;			// How much of associated PosEffectsNodes belong to this book
	
	int		TidBit;			// Typically zero, but increases for multiple allocations for the same book
	
	SDB_UNIQUE_ID
			AllocationId;	// Typically the TradeId. Helps in the Addition of Neteffects
							// by collapsing the effects of only those books with same TidBit 
							// and AllocationId. 		
};

struct NetEffectsNodeStructure
{
	NET_EFFECTS_NODE
			*Next;

	double	Quantity;			// Positive for Portfolio1
								// Negative for Portfolio2

	char	Security[ SDB_SEC_NAME_SIZE ];

};

struct NetEffectsStructure
{
	NET_EFFECTS
			*Next;

	NET_EFFECTS_NODE
			*Nodes;

	NET_EFFECTS_BOOK
			*Books;
			
};

DLLEXPORT DT_DATA_TYPE
		DtNetEffects;	 // Initialized by Trade STARTUP message

DLLEXPORT int
		DtNetEffectsSetAllocationId( NET_EFFECTS *NetEffects, SDB_UNIQUE_ID AllocationId ),
		DtNetEffectsGetBooks( 	  	 NET_EFFECTS *NetEffects, DT_VALUE *Books );

DLLEXPORT GsStatus
		RoundNetEffects( NET_EFFECTS **inNetEffects ),
		RoundTradeChildren( DT_VALUE *Struct );

DLLEXPORT double
		GetFactorFromTradeChildren( DT_VALUE TofRNodes, DT_VALUE Trade1Nodes, DT_VALUE Trade2Nodes,	int multiplier );
#endif



