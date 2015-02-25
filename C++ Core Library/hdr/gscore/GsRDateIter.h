/****************************************************************
**
**	GSRDATEITER.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateIter.h,v 1.5 2000/03/22 11:51:13 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEITER_H )
#define IN_GSDATE_GSRDATEITER_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDate.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateIter : public GsRDateImp
{
	GsRDate m_RDate;
	int m_NoIters;

public:
	
	GsRDateIter();
	GsRDateIter( const GsRDate& RDate );
	GsRDateIter( const GsRDateIter& Other );

	~GsRDateIter();

	GsRDateIter& operator=( const GsRDateIter& Rhs );

	virtual GsRDateImp* copy() const { return new GsRDateIter( *this ); }

	virtual int cardinality() const { return m_RDate.cardinality() ; }

	virtual GsRDate elem( size_t Index ) const { return m_RDate.elem( Index ); }

	virtual GsDate addToDate(const GsDate& Date ) const;
	virtual GsDate toDate() const { return m_RDate.toDate(); }
	virtual GsDateVector toVec() const { return m_RDate.toVec(); }

	virtual GsRDateImp* multiply( int M );

	virtual GsRDateImp*  setDefaultDateGen( GsRDateDateGen* DateGen );

	virtual GsString toString() const;
};



CC_END_NAMESPACE

#endif 
