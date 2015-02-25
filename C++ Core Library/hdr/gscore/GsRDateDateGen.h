/****************************************************************
**
**	GSRDATEDATEGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateDateGen.h,v 1.3 2000/03/22 11:51:11 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEDATEGEN_H )
#define IN_GSDATE_GSRDATEDATEGEN_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDate.h>
#include <gscore/GsString.h>

#include <date.h>

CC_BEGIN_NAMESPACE( Gs )

class GsPeriod;
class GsRDateImp;

class EXPORT_CLASS_GSDATE GsRDateDateGen
{
	size_t m_Count;

protected:
	
	GsRDateDateGen();
	GsRDateDateGen( const GsRDateDateGen& );
	GsRDateDateGen& operator=( const GsRDateDateGen& );

	virtual ~GsRDateDateGen();
	
public:
	
	virtual GsDate addToDate( const GsDate& Date, const GsPeriod& ) const;
	virtual GsDate addToDate( const GsDate& Date, RDATE ) const;
	virtual GsDate rounding( const GsDate& ) const = 0;

	virtual GsRDateImp* createPeriodImp( int Offset, char Method ) = 0;

	virtual GsString description() const = 0;

	inline void ref() { m_Count++;}
	inline void unRef() { if( --m_Count == 0 ) delete this; }
	inline size_t refCount() const { return m_Count; }
};


CC_END_NAMESPACE

#endif 
