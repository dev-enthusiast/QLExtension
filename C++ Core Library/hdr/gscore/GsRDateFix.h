/****************************************************************
**
**	GSRDATEFIX.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateFix.h,v 1.5 2000/03/22 11:51:12 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEFIX_H )
#define IN_GSDATE_GSRDATEFIX_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsRDate.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsRDateFix : public GsRDateImp
{
   	GsRDate m_RDate;

public:
	
	GsRDateFix();
	GsRDateFix( const GsRDate& RDate );
	GsRDateFix( const GsRDateFix& Other );

	~GsRDateFix();

	GsRDateFix& operator=( const GsRDateFix& Rhs );
  
	virtual GsRDateImp* copy() const { return new GsRDateFix( *this ); }

	virtual int cardinality() const { return m_RDate.cardinality(); }
 
	virtual GsRDate elem( size_t Index ) const { return m_RDate.elem( Index ); }
	virtual GsDate addToDate(const GsDate& Date ) const { return m_RDate.addToDate( Date ); }
	virtual GsDate toDate() const { return m_RDate.toDate(); }
	virtual GsDateVector toVec() const { return m_RDate.toVec(); }

	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* DateGen );

	virtual GsString toString() const;
};

CC_END_NAMESPACE

#endif 
