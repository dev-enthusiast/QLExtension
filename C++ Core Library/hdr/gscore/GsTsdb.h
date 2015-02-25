/****************************************************************
**
**	GSTSDB.H	- GsTsdb Addin Functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsTsdb.h,v 1.8 2000/12/10 18:24:26 wur Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSTSDB_H )
#define IN_GSDATE_GSTSDB_H

#include <gscore/base.h>
#include <gscore/GsMultiDoubleTimeSeries.h>

CC_BEGIN_NAMESPACE( Gs )


EXPORT_CPP_GSTSDB GsDoubleTimeSeries*
		GsTsdbReadSeries( const GsString& Symbol, const GsDate& DateFrom, const GsDate& DateTo );

EXPORT_CPP_GSTSDB double
		GsTsdbRead( const GsString& Symbol, const GsDate& Date );

EXPORT_CPP_GSTSDB GsDs*
		GsMTSFromTsdb(const GsStringVector& Symbol, 
							   const GsDate& DateFrom, 
							   const GsDate& DateTo,
							   int sampleFreq,
							   int aligntype,
							   int ithSeries );

EXPORT_CPP_GSTSDB void
		mergeGsDs( 
			const GsDs& ts1, 
			const GsDs& ts2,
			int alignType,
			int ithSeries,
			GsDs* ts );
		
CC_END_NAMESPACE

#endif 



