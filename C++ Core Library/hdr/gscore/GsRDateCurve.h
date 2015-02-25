/****************************************************************
**
**	GSRDATECURVE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gscurve/src/gscore/GsRDateCurve.h,v 1.3 2001/11/27 22:42:14 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCURVE_GSRDATECURVE_H )
#define IN_GSCURVE_GSRDATECURVE_H

#include <gscore/base.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDoubleVector.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRDateCurve 
** Description : GsRDateCurve class
****************************************************************/

class EXPORT_CLASS_GSCURVE GsRDateCurve
{
public:
	GsRDateCurve( const GsRDate& = GsRDate() );
	GsRDateCurve( const GsRDateCurve & );

	virtual ~GsRDateCurve();

	GsRDateCurve &operator=( const GsRDateCurve &rhs );

	const GsRDate& indexDates() const { return m_RDate; }

	virtual double value( size_t ) const;

	size_t size() const { return m_Values.size(); }

protected:

	GsDoubleVector m_Values;

private:

	GsRDate m_RDate;
};


CC_END_NAMESPACE

#endif 
