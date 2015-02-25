/* $Header: /home/cvs/src/metal/src/metal.h,v 1.5 2000/11/29 22:25:05 abramjo Exp $ */
/****************************************************************
**
**	METAL.H	
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.5 $
**
****************************************************************/

#if !defined( IN_METAL_H )
#define IN_METAL_H

#define METAL_CLASS_IMPLIED_NAME			"%3.3s %s"
#define METAL_FWD_IMPLIED_NAME				"Fwd %3.3s %7.7s %-11.11s %s"
#define METAL_INV_IMPLIED_NAME				"Inv %3.3s %-11.11s %s"
#define METAL_LOCATION_IMPLIED_NAME			"MtlLoc %s"

int	MtlValueFuncMetalClassName( GOB_VALUE_FUNC_ARGS );
int	MtlValueFuncLocationName( GOB_VALUE_FUNC_ARGS );
	
DLLEXPORT int MtlMakeMetalInventory	(char *QuantityUnit, char *MetalClassShortName, char *DeliveryLocation, char *ForwardName);

#endif

