/****************************************************************
**
**	GSSTATS.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsStats.h,v 1.11 2001/03/13 12:01:27 goodfj Exp $
**
****************************************************************/

#if !defined( IN_GSSTATS_H )
#define IN_GSSTATS_H

#include <gsquant/base.h>
#include <gscore/GsVector.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gsdt/GsDtDictionary.h>


CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsStats 
** Description : 
****************************************************************/

class EXPORT_CLASS_GSQUANT GsStats
{
public:
	GsStats( const GsVector &Data );

	GsStats( const GsStats &Stats ); 

	~GsStats();

	double Sum() const; 
	double Mean() const;
	
	double Variance() const; 

	double StandardDeviation() const; 

	double NumElements() const; 

	double Min() const; 
	double Max() const; 

	// GsDt Support.
	typedef GsDtGeneric<GsStats> GsDtType;
	GsString toString() const { return "GsStats"; }
	inline static const char * typeName() { return "GsStats"; }

private:

	int		m_NumElements;

	double	m_SumX;
	
	double	m_SumX2;

	double	m_Min;
	double	m_Max;
};


EXPORT_CPP_GSQUANT GsStats * GsStatsCreate( const GsVector &Data );

EXPORT_CPP_GSQUANT double GsStatsMin( const GsStats &Stats );
EXPORT_CPP_GSQUANT double GsStatsMax( const GsStats &Stats );

EXPORT_CPP_GSQUANT double GsStatsMean( const GsStats &Stats );
EXPORT_CPP_GSQUANT double GsStatsVariance( const GsStats &Stats );
EXPORT_CPP_GSQUANT double GsStatsStandardDeviation( const GsStats &Stats );

EXPORT_CPP_GSQUANT double GsDoubleTimeSeriesCov( const GsDoubleTimeSeries& TS1, const GsDoubleTimeSeries& TS2 );

EXPORT_CPP_GSQUANT GsDtDictionary *GsStatsDictionary( const GsStats &Stats );

EXPORT_CPP_GSQUANT GsVector * GsQuantiles( const GsVector &Data, unsigned Denom );

CC_END_NAMESPACE

#endif 
