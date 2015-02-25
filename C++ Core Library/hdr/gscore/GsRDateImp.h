/****************************************************************
**
**	GSRDATEFUNC.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateImp.h,v 1.10 2001/11/27 22:43:11 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEIMP_H )
#define IN_GSDATE_GSRDATEIMP_H

#include <gscore/base.h>
#include <gscore/types.h>
#include <gscore/GsDate.h>
#include <gscore/GsRDate.h>
#include <gscore/GsDateVector.h>
#include <gscore/GsRDateDateGen.h>
#include <gscore/GsException.h>

CC_BEGIN_NAMESPACE( Gs )


class EXPORT_CLASS_GSDATE GsRDateImp
{
	size_t m_Count;

protected:
	virtual ~GsRDateImp();
	GsRDateImp();

public:
	
  	virtual GsRDateImp* copy() const = 0;
	virtual GsRDateImp* uniuqeImp();

	bool operator==( const GsRDateImp& Other );

	void ref() { m_Count++;}
	void unRef() { if( --m_Count == 0 ) delete this; }
	size_t refCount() const { return m_Count; }

	virtual int cardinality() const { return -1; }
	virtual GsRDate::quotient_type findQuotient( const GsDate& Date ) const;

	size_t size() const;

	virtual GsRDate elem( size_t Index ) const ;
	virtual GsRDateImp* elemSet( size_t Index, const GsRDate& RDate );
	virtual GsRDateImp* multiply( int M ); 
	virtual GsRDateImp* append( const GsRDate& );
	virtual GsRDateImp* prepend( const GsRDate& );

	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* );

	virtual GsDate addToDate(const GsDate& Date ) const;
	virtual GsDate toDate() const;
	virtual GsDateVector toVec() const;

	virtual GsString toString() const = 0;

};


#define GSRDATE_INVALID_OPERATION( Method ) GSX_THROW( GsXInvalidOperation, ##Method )

CC_END_NAMESPACE

#endif 
