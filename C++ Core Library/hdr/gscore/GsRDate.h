/****************************************************************
**
**	GsRDate.h	-
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDate.h,v 1.41 2001/11/27 22:43:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSRDATE_H )
#define IN_GSRDATE_H

#include <gscore/GsException.h>
#include <gscore/types.h>
#include <gscore/GsString.h>
#include <gscore/GsDate.h>
#include <gscore/GsPeriod.h>
#include <gscore/GsDateGen.h>
#include <gscore/GsRDateDateGen.h>
#include <gscore/GsDateEnums.h>
#include <gscore/GsMemPoolPerClass.h>

#include <ccstl.h>
#include <vector>

CC_BEGIN_NAMESPACE( Gs )

class GsRDateImp;
class GsRDateVector;

class EXPORT_CLASS_GSDATE GsRDate
{
public: 
	typedef GsDtRDate GsDtType;
	typedef CC_STL_PAIR( int, int ) quotient_type;

	GsRDate();
	GsRDate( const GsRDate& RDate );
	GsRDate( const GsDate& Date );
	GsRDate( const GsDateVector& Vec );	
	explicit GsRDate( GsRDateImp* RDateImp );
	GsRDate( const GsPeriod& Period );
	GsRDate( const GsPeriod& Period, const GsDateGen& DateGen );
	GsRDate( const GsPeriod& Period, const GsString& DateGenName );

	// For GsRDate( const GsString& ) use GsRDateParser::fromString( const GsString& )

	~GsRDate();

	GsRDate& operator=(const GsRDate& rdate);

	GsRDate& operator*=( int M );
	GsRDate& operator+=( const GsRDate& RDate );
	GsRDate& operator-=( const GsRDate& RDate );

	GsRDate& setDefaultDateGen( GsRDateDateGen* );
	GsRDate& elemSet( size_t Index, const GsRDate& RDate );
	GsRDate& append( const GsRDate& RDate );

	GsRDate operator-() const;
	GsRDate invert() const;
	GsDate toDate() const;
 	GsDate addToDate(const GsDate& Date) const;
	GsDateVector toVec() const;
	GsRDate elem( size_t Index ) const;

	GsString toString() const;

	int cardinality() const;// Number of embbeded GsRDates
	size_t size() const; // max( cardinality(), 0 );

	quotient_type findQuotient( const GsDate& Date ) const;

	GsRDate resolve() const;

private:
	GsRDateImp* m_Data;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};

inline bool operator==( const GsRDate& R1, const GsRDate& R2 )
{
	return R1.toString() == R2.toString();
}

inline bool operator!=( const GsRDate& R1, const GsRDate& R2 )
{
	return R1.toString() != R2.toString();
}


inline GsRDate operator*( int M, const GsRDate& RDate )
{
	GsRDate Ret( RDate );
	Ret *= M;
	return Ret;
}

inline GsRDate operator*( const GsRDate& RDate, int M )
{
	return M * RDate;
}


inline GsRDate operator+( const GsRDate& RDate1, const GsRDate& RDate2 )
{
	GsRDate Ret( RDate1 );
	Ret += RDate2;
	return Ret;
}

inline GsRDate operator-( const GsRDate& RDate1, const GsRDate& RDate2 )
{
	GsRDate Ret( RDate1 );
	Ret -= RDate2;
	return Ret;
}

inline GsDate& operator+=( GsDate& Date, const GsRDate& RDate )
{
	Date = RDate.addToDate( Date );
 	return Date;
}

inline GsDate& operator-=( GsDate& Date, const GsRDate& RDate )
{
 	Date = ( RDate * -1 ).addToDate( Date );
	return Date;
}

inline GsRDate operator+( const GsDate& Date, const GsRDate& RDate )
{
	GsRDate Result( Date );
	return Result += RDate;
}

inline GsRDate operator-( const GsDate& Date, const GsRDate& RDate )
{
	GsRDate Result( Date );
	return Result -= RDate;
}

inline GsDate operator+( const GsRDate&, const GsDate& Date )
{
	return Date;
}	

inline GsDate operator-( const GsRDate&, const GsDate& Date )
{
	return Date;
}	

inline bool operator < ( const GsRDate& rd1, const GsRDate& rd2 ) 
{ 
	try	{	return rd1.toDate() < rd2.toDate(); 
	} 		catch ( GsXInvalidOperation ) {}

	try {	return rd1.toDate() < rd2.toVec().front();
	} 		catch ( GsXInvalidOperation ) {}

	try {	return rd1.toVec().front() < rd2.toDate();
	} 		catch ( GsXInvalidOperation ) {}

	try {	return rd1.toVec() < rd2.toVec();
	} 		catch ( GsXInvalidOperation ) {}

	GSX_THROW( GsXInvalidOperation, "< RDates are not convertible" );
	return false;						// make gcc happy we should never get here because the above line will raise an exception
}

EXPORT_CPP_GSDATE CC_OSTREAM& 
		operator << ( CC_OSTREAM& s, const GsRDate& rd );
EXPORT_CPP_GSDATE GsString
		GsRDateToString( const GsRDate& );
EXPORT_CPP_GSDATE GsDate
		GsRDateToGsDate( const GsRDate& );
EXPORT_CPP_GSDATE GsDateVector
		GsRDateToGsDateVector( const GsRDate& );
EXPORT_CPP_GSDATE GsDateVector
		GsRDateToSortedUniqueGsDateVector( const GsRDate& RDate );
EXPORT_CPP_GSDATE GsDateTimeSeries*
		GsRDateToGsDateTimeSeries( const GsRDate& );
EXPORT_CPP_GSDATE GsRDate 
		GsRDateSetDefaultDateGen( const GsRDate& Rdate, const GsDateGen& DG ),
		GsRDateSet( const GsRDateVector& Vec ),
		GsRDateFixed( const GsRDate& Rdate ),
		GsRDateIterator( const GsRDate& Rdate ),
		GsRDateRangeFunc( const GsRDate& Increment, const GsRDate& WindowStart, const GsRDate& WindowEnd,
						  bool IncStart, bool IncEnd ),
		GsRDateRangeStubFunc( const GsRDate& Increment, const GsRDate& WindowStart, const GsRDate& WindowEnd, 
							  enum LT_INCLUDE_END IncludeFirst,	
							  enum LT_INCLUDE_END IncludeLast,	
							  enum LT_STUB_TYPE StartStub,					
							  enum LT_STUB_TYPE EndStub, 
							  int CompoundingPeriodsPerPayment ),						
		GsRDateAt( const GsRDate& RDate, int Index ),
		GsRDateSetAt( const GsRDate& RDate, int Index, const GsRDate& Elem ),
		GsRDateResolve( const GsRDate& RDate );

EXPORT_CPP_GSDATE int
		GsRDateFindQuotientFirst( const GsRDate& RDate, const GsDate& Date ),
		GsRDateFindQuotientSecond( const GsRDate& RDate, const GsDate& Date );

EXPORT_CPP_GSDATE GsRDate 
		GsRDateSeriesCreate( const GsRDateVector &VectorOfRDates );


#define GS_RDATE_RANGE_MAX_ITER 1000

#define GSX_RANGE_ITER_THROW( _RDATE_ ) \
CC_OSTRSTREAM Stream; \
Stream << "GsRDateRange::toVec date failed to increment after " << GS_RDATE_RANGE_MAX_ITER << " iterations of \n"; \
Stream << _RDATE_ << "\n"; \
Stream << "Usage : RANGE( Increment, Window Begin, Window End )"; \
GSX_THROW( GsXInvalidOperation, CC_OSTRSTREAM_STR( Stream ) );


CC_END_NAMESPACE
#endif


