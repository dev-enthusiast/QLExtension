/****************************************************************
**
**	GSRDATEDEFAULTDATEGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateDefaultDateGen.h,v 1.6 2001/11/27 22:43:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEDEFAULTDATEGEN_H )
#define IN_GSDATE_GSRDATEDEFAULTDATEGEN_H

#include <gscore/base.h>
#include <gscore/GsRDateDateGen.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRDateDefaultDateGen 
** Description : GsRDateDefaultDateGen
****************************************************************/

class GsDateGen;
class GsPeriod;

class EXPORT_CLASS_GSDATE GsRDateDefaultDateGen : public GsRDateDateGen 
{
	GsRDateDefaultDateGen();

	GsDateGen* m_DateGen;

	GsRDateDefaultDateGen &operator=( const GsRDateDefaultDateGen &rhs ) { return *this; }
	GsRDateDefaultDateGen( const GsRDateDefaultDateGen & ) {}

	~GsRDateDefaultDateGen();

public:
	GsRDateDefaultDateGen( const GsString& );
	GsRDateDefaultDateGen( const GsDateGen& );
	
	virtual GsDate addToDate( const GsDate& Date, const GsPeriod& ) const;
	inline virtual GsDate addToDate( const GsDate& Date , RDATE rDate) const;
	virtual GsDate rounding( const GsDate& ) const;

	virtual GsRDateImp* createPeriodImp( int Offset, char Method );

	virtual GsString description() const ;
};

inline GsDate GsRDateDefaultDateGen::addToDate( const GsDate& Date , RDATE rDate) const 
{ 
  return GsRDateDateGen::addToDate(Date,rDate); 
}

CC_END_NAMESPACE

#endif 





