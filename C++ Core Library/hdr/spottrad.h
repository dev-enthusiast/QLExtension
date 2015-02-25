/* $Header: /home/cvs/src/spot/src/spottrad.h,v 1.12 2001/08/14 18:39:21 simpsk Exp $ */
/****************************************************************
**
**	SPOTTRAD.H	
**
**	$Revision: 1.12 $
**
****************************************************************/

#ifndef IN_SPOTTRAD_H
#define IN_SPOTTRAD_H

#ifndef IN_GOB_H
#include <gob.h>
#endif


#define		INVERT_CHILD(Tag)		ChildData->Tag = ChildData->Tag == 0.0 ? 0.0 : 1.0 / ChildData->Tag


//											TradeType	EffQuantFactor	ValidatePrice			Securities				USDECrossValueFunc				PreserveEffQuant	ZeroUnitPrice

#define		SPOT_TRADE_BUY_OPT			{	"Buy",		 1.0,			SpotValidatePrice,		NULL,					NULL,							FALSE,				FALSE	}
#define		SPOT_TRADE_SELL_OPT			{	"Sell",		-1.0,			SpotValidatePrice,		NULL,					NULL,							FALSE,				FALSE	}
#define		SPOT_TRADE_EXPIRE			{	"Expire",	-1.0,			SpotValidateExerPrice,	NULL,					NULL,							TRUE,				TRUE	}
#define		SPOT_TRADE_EXERCISE_MF		{	"Exercise",	-1.0,			SpotValidateExerPrice,	"Exercise Securities",	SpotValueFuncUSDECrossOptExr,	TRUE,				TRUE	}
#define		SPOT_TRADE_ASSIGN_MF		{	"Assign",	 1.0,			SpotValidateExerPrice,	"Assign Securities",	SpotValueFuncUSDECrossOptExr,	TRUE,				TRUE	}
#define		SPOT_TRADE_EXERCISE_NO_MF	{	"Exercise",	-1.0,			SpotValidateExerPrice,	"Exercise Securities",	NULL,							TRUE,				TRUE	}
#define		SPOT_TRADE_ASSIGN_NO_MF		{	"Assign",	 1.0,			SpotValidateExerPrice,	"Assign Securities",	NULL,							TRUE,				TRUE	}
#define		SPOT_TRADE_FIX				{	"Fix",		-1.0,			SpotValidateExerPrice,	"Fixing Securities",	NULL,							TRUE,				TRUE	}
			

/*
**	Trade types for options
*/
struct DT_BINARY;

extern DT_BINARY
		SpotTradeTypeBinaryOptMf,
		SpotTradeTypeBinaryXOpt;

DLLEXPORT DT_BINARY
		SpotTradeTypeBinaryOpt;


/*
**	Prototypes
*/
	
DLLEXPORT int		SpotValidatePrice( 				SDB_OBJECT *SecPtr, SDB_M_DATA *MsgData, SDB_VALUE_TYPE ValueType, double Price ),
		SpotValueFuncUSDECrossOptExr(	GOB_VALUE_FUNC_ARGS );

#endif
