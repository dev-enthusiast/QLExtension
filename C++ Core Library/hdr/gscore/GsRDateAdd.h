/****************************************************************
**
**	GSRDATEADD.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsRDateAdd.h,v 1.4 2000/04/11 13:49:58 thompcl Exp $
**
****************************************************************/

#if !defined( IN_GSDATE_GSRDATEADD_H )
#define IN_GSDATE_GSRDATEADD_H

#include <gscore/base.h>
#include <gscore/GsRDateImp.h>
#include <gscore/GsMemPoolPerClass.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRDateAdd 
** Description : GsRDateAdd class
****************************************************************/

class EXPORT_CLASS_GSDATE GsRDateAdd : public GsRDateImp
{
	GsRDateImp
			*m_Lhs,
			*m_Rhs;

	GsRDateAdd();

public:
	GsRDateAdd( GsRDateImp* Lhs, GsRDateImp* Rhs );
	GsRDateAdd( const GsRDateAdd & );

	~GsRDateAdd();

	GsRDateAdd &operator=( const GsRDateAdd &rhs );

	virtual GsRDateImp* copy() const;

	virtual int cardinality() const;

	virtual GsRDate::quotient_type findQuotient( const GsDate& Date ) const;

  	virtual GsRDate elem( size_t Index ) const;

	virtual GsDate addToDate(const GsDate& Date ) const;
	virtual GsDate toDate() const;
  	virtual GsDateVector toVec() const;

	virtual GsRDateImp* multiply( int M )
	{
		GsRDateAdd* Imp = static_cast< GsRDateAdd* >( uniuqeImp() );

		Imp->m_Lhs = Imp->m_Lhs->multiply( M );
		Imp->m_Rhs = Imp->m_Rhs->multiply( M );
	
		return Imp;
	}

  	virtual GsRDateImp* setDefaultDateGen( GsRDateDateGen* DateGen )
	{
		GsRDateAdd* Imp = static_cast< GsRDateAdd* >( uniuqeImp() );

		Imp->m_Lhs = Imp->m_Lhs->setDefaultDateGen( DateGen );
		Imp->m_Rhs = Imp->m_Rhs->setDefaultDateGen( DateGen );

		return Imp;
	}

	virtual GsString toString() const;

	// We manage our own memory in a mempool.

	GS_MEMPOOL_PER_CLASS_DECLARE_MEMBERS
};


CC_END_NAMESPACE

#endif 
