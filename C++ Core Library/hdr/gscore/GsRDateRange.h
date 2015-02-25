/****************************************************************
**
**	GSRDATERANGEFUNC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateRange.h,v 1.10 2000/03/23 18:35:25 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATERANGE_H )
#define IN_GSDATE_GSRDATERANGE_H

#include <gscore/base.h>
#include <gscore/GsRDate.h>
#include <gscore/GsRDateVecCached.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateRange : public GsRDateVecCached
{
protected:

    GsRDate	m_WindowStart,
			m_WindowEnd,
			m_RDate;		// Increment

	int
			m_Base;			// Lowest N for which N*m_RDate is within the window
	size_t
			m_Size;			// Number of N*m_RDate within the window

	bool	m_IncStart;		// IncludeStart
	bool	m_IncEnd;		// IncludeEnd
	bool	m_IncDateIncN;   // Does date increase when N increases ?
	bool    m_DateGenSet;

	void setLimits();

public:

    GsRDateRange( const GsRDate& RDate, const GsRDate& WindowStart, 
				  const GsRDate& WindowEnd, bool IncludeStart = true, bool IncludeEnd = true );

    GsRDateRange( const GsRDateRange& Other );
    ~GsRDateRange();

    GsRDateRange& operator=( const GsRDateRange& Rhs );

    virtual GsRDateImp* copy() const { return new GsRDateRange( *this ); }

	virtual int cardinality() const { return m_Size; }

	virtual GsRDate elem( size_t Index ) const;

	virtual GsDateVector getVec() const;

    virtual GsString toString() const;

    virtual GsRDateImp* multiply( int M );
    virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* DateGen );

	void rollWindowStart( int NumberOfDates );
	void rollWindowEnd( int NumberOfDates );

};

CC_END_NAMESPACE

#endif 




