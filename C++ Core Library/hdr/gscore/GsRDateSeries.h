/****************************************************************
**
**	GSRDATESERIES.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateSeries.h,v 1.15 2000/06/28 18:15:28 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATESERIES_H )
#define IN_GSDATE_GSRDATESERIES_H

#include <gscore/base.h>
#include <gscore/GsRDate.h>
#include <gscore/GsRDateVecCached.h>
#include <ccstl.h>
#include <vector>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateSeries : public GsRDateVecCached
{
	typedef CC_STL_VECTOR( GsRDate ) Series;

	typedef Series::iterator SeriesIterator;
	typedef Series::const_iterator ConstSeriesIterator;

protected:

	Series 
			m_Series;
	
public:

	GsRDateSeries();
	GsRDateSeries( size_t n );
	GsRDateSeries( const GsRDateVector &VectorOfRDates );
	GsRDateSeries( const GsDateVector  &VectorOfDates  );
	GsRDateSeries( const GsRDateSeries& Other );
	~GsRDateSeries();

	GsRDateSeries& operator=( const GsRDateSeries& Rhs );

	virtual GsRDateImp* copy() const { return new GsRDateSeries( *this ); }

	virtual int cardinality() const { return m_Series.size(); }

	virtual GsRDate elem( size_t Index ) const;
	virtual GsRDateImp* elemSet( size_t Index, const GsRDate& RDate );
	
	virtual GsRDateImp* multiply( int M );
	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* DateGen );

	virtual GsRDateImp* append( const GsRDate& RDate );
	virtual GsRDateImp* prepend( const GsRDate& Rdate );

	virtual GsDateVector getVec() const;
	virtual GsString toString() const;
	
	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};



CC_END_NAMESPACE

#endif 
