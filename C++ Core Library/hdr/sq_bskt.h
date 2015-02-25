/* $Header: /home/cvs/src/squant/src/sq_bskt.h,v 1.7 2000/06/15 15:33:37 goodfj Exp $ */
/****************************************************************
**
**	SQ_BSKT.H	routines to handle baskets
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_SQ_BSKT_H )
#define IN_SQ_BSKT_H

#if !defined( IN_DATE_H )
#include <date.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


/*
**	Snagged from meanrev.h to remove dependency on meanrev proj
*/

typedef struct SQ_BskParams
{
	double	LongVol;		// term vol of long position of basket
	double	ShortVol;		// term vol of short postition of basket
	double	Corr;			// correlation of returns of long and short
	double	Current;		// Current value of basket
	double	LongCurrent;	// Current value of long position
	double	ShortCurrent;	// Current value of short position

} SQ_BSK_PARAMS;

DLLEXPORT int SQ_TimeBasket (
	Q_MATRIX        *fwds,       // matrix of forward prices by dates and asset
	Q_MATRIX        *weights,    // matrix of weights
	Q_CURVE_MATRIX  *vols,       // matrix of vol and cross vol curves
	DATE            prcDate,     // pricing date
	DATE            expDate,     // last date - usually last date of date vector
	Q_DATE_VECTOR   *dates,	     // vector of dates
	SQ_BSK_PARAMS	*basketparms // returned structure of vols and forwards
	);

DLLEXPORT int SQ_TimeBasket2 (
	Q_MATRIX        *fwds,       // matrix of forward prices by dates and asset
	Q_MATRIX        *weights,    // matrix of weights
	Q_CURVE_MATRIX  *vols,       // matrix of vol and cross vol curves
	DATE            prcDate,     // pricing date
	DATE            expDate,     // last date - usually last date of date vector
	Q_DATE_VECTOR   *dates,	     // vector of dates
	SQ_BSK_PARAMS	*basketparms // returned structure of vols and forwards
	);

#endif

