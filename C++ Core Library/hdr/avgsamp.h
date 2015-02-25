/* $Header: /home/cvs/src/datatype/src/avgsamp.h,v 1.2 2005/11/29 18:03:22 e45019 Exp $ */
/****************************************************************
**
**	AVGSAMP.H	
**
**	Copyright 2004 - Constellation Power Source, Inc.
**
**	$Revision: 1.2 $
**
****************************************************************/

#if !defined( IN_AVGSAMP_H )
#define IN_AVGSAMP_H

#if !defined( IN_DATE_H )
#	include	<date.h>
#endif

#if !defined( IN_DATATYPE_H )
#	include	<datatype.h>
#endif

DLLEXPORT char *SettlementFrequencyArray[];
DLLEXPORT DT_CURVE *SQ_MakeFrequencyCurve(
	char		*Denominated,
	char		*QuantityUnit,
	char		*SamplingRate,
	DATE		StartDate,
	DATE		LastDate,
	DT_CURVE	*ExclusionCurve
);

DLLEXPORT DT_CURVE *SQ_MakeSettlementCurve(
	char		*Denominated,
	char		*QuantityUnit,
	char		*SettlementFreq,
	RDATE		SettleTenor,
	DATE		StartDate,
	DATE		LastDate
);

DLLEXPORT DT_CURVE	*SQ_MissingDataCurve(
	DT_CURVE		*HistCurve,			//	First Curve
	DT_CURVE		*SampleCurve,		//	Second Curve
	DATE		    StartDate,			//  Range of interest start
	DATE		    EndDate				//  Range of interest end
);

DLLEXPORT DT_CURVE	*SQ_FilterCurve(
	DT_CURVE		*DataCurve,			//	First Curve
	DT_CURVE		*ExclusionCurve,	//	Second Curve
	DATE		    StartDate,			//  Range of interest start
	DATE		    EndDate				//  Range of interest end
);


DLLEXPORT int
        SQ_MakeDailyAllDaysCurveTZ(		DT_CURVE *Curve, DATE From, DATE To, DT_CURVE *ExcludeDates, char *TZFilt ),
        SQ_MakeDailyAllDaysCurveMonths( DT_CURVE *Curve, DATE From, DATE To, DT_CURVE *ExcludeDates, char *Months );

#endif

