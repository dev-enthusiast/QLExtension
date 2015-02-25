/****************************************************************
**
**	GSMULTIDOUBLETIMESERIES.H	- GsMultiDoubleTimeSeries class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsMultiDoubleTimeSeries.h,v 1.14 2000/07/12 17:10:02 wur Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSMULTIDOUBLETIMESERIES_H )
#define IN_GSDATE_GSMULTIDOUBLETIMESERIES_H

#include <gscore/GsTimeSeries.h>
#include <gscore/GsTsHandle.h>
#include <gscore/GsVector.h>
#include <gscore/GsMatrix.h>

CC_BEGIN_NAMESPACE( Gs )

//class GsDtDictionary;

class EXPORT_CLASS_GSDATE GsDs : public GsTimeSeries< GsTsHandle< GsVector > >
{
public:

	typedef GsTimeSeries< GsTsHandle< GsVector > > BaseClass;
	typedef BaseClass::iterator iterator;
	typedef BaseClass::const_iterator const_iterator;
	typedef BaseClass::size_type size_type;

	typedef GsDtGeneric< GsDs > GsDtType;
	inline static const char* typeName() { return "GsDs"; }
	GsString toString() const { return "GsDs"; }

	GsDs( size_t N = 0);
	GsDs( const GsDateVector& dates );
	GsDs( const GsDs& ts );
	GsDs( const GsDateVector& dates, const GsMatrix& values );
	~GsDs() {};

	GsDs& shflog( double shift=0 );
	void insert( const GsDate& date, const GsTsHandle< GsVector >& value );
	int cols( void ) const { return m_cols; }
	GsVector getColumnData( const int i_col ) const;
	GsMatrix getColumnDataWithDates( const int i_col, bool reverseFlag ) const;
	GsMatrix getData( void ) const ;
	int remove( const GsDateVector& dates );
	GsMatrix expand( bool reverseFlag ) const;

private:
	int m_cols;
};


CC_END_NAMESPACE

#endif 
