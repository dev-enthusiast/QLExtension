/****************************************************************
**
**	GSFUNCDATETOCONTINUOUS.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncDateToContinuous.h,v 1.3 2000/06/20 13:21:46 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCDATETOCONTINUOUS_H )
#define IN_GSQUANT_GSFUNCDATETOCONTINUOUS_H

#include <gsquant/base.h>
#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>
#include <gscore/GsDate.h>
#include <gscore/GsCTime.h>
#include <gsdt/GsDtFuncHandle.h>
#include <map>

CC_BEGIN_NAMESPACE( Gs )

// The class below is used to map dates to doubles.  Once can
// imagine actually having a whole slew of such classes to
// deal with the different ways one might like to do this.

/****************************************************************
** Class	   : GsFuncDateToContinuous 
** Description : Function to convert dates to continous times.
****************************************************************/

class EXPORT_CLASS_GSQUANT GsFuncDateToContinuous : public GsFunc< GsDate, double >
{
public:
	GsFuncDateToContinuous( GsDate BaseDate );
	GsFuncDateToContinuous( const GsFuncDateToContinuous &Other );

	virtual ~GsFuncDateToContinuous();

	GsFuncDateToContinuous &operator=( const GsFuncDateToContinuous &rhs );

	virtual double operator()( GsDate Date ) const;
	
private:	
	GsDate	m_BaseDate;			
};



typedef GsFuncHandle< GsDate, double > GsFuncDateToDouble;

EXPORT_CPP_GSQUANT GsFuncDateToDouble * GsFuncDateToCTimeCreate( GsDate BaseDate );


CC_END_NAMESPACE

#endif 
