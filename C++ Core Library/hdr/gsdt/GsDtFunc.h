/****************************************************************
**
**	GSDTFUNC.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdt/src/gsdt/GsDtFunc.h,v 1.5 2001/11/27 22:43:58 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTFUNC_H )
#define IN_GSDT_GSDTFUNC_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsDtFunc 
** Description : A class representing an aribrary type-checked
**				 function for double to double.  This is actually
**				 an abstract base class for more than one kind
**				 of function.  The first kind of derived class
**				 is one containing a GsFuncHandle< D, R >, which
**				 type-checks its arg to make sure it is a D.
**				 The second is the composition of two other
**				 GsDtFuncs.
****************************************************************/

class EXPORT_CLASS_GSDT GsDtFunc : public GsDt
{
	GSDT_DECLARE_ABSTRACT_TYPE( GsDtFunc )
public:
	GsDtFunc( const GsType *DomainType, const GsType *RangeType );
	GsDtFunc( const GsDtFunc & );

	virtual ~GsDtFunc();

	GsDtFunc &operator=( const GsDtFunc &rhs );

	// Virtual functions from GsDt

	virtual GsDt *Evaluate( const GsDtArray &Args ) const;

	virtual GsString	toString() const;

	// Stuff relevant to this actual class.

	virtual GsDt*	operator()( const GsDt * ) const = 0;			

protected:

	// Types we expect the arg and return to be.

	const GsType
			*m_DomainType,
			*m_RangeType;
};



/****************************************************************
** Class	   : GsDtFunc2 
** Description : A two arg version of GsDtFunc
****************************************************************/

class EXPORT_CLASS_GSDT GsDtFunc2 : public GsDt
{
	GSDT_DECLARE_ABSTRACT_TYPE( GsDtFunc2 )
public:
	GsDtFunc2( const GsType *Domain1Type, const GsType *Domain2Type, const GsType *RangeType );
	GsDtFunc2( const GsDtFunc2 & );

	virtual ~GsDtFunc2();

	GsDtFunc2 &operator=( const GsDtFunc2 &rhs );

	// Virtual functions from GsDt

	virtual GsDt *Evaluate( const GsDtArray &Args ) const;

	virtual GsString toString() const;

	// Stuff relevant to this actual class.

	virtual GsDt*	operator()( const GsDt *, const GsDt * ) const = 0;			

protected:

	// Types we expect the arg and return to be.

	const GsType
			*m_Domain1Type,
			*m_Domain2Type,
			*m_RangeType;
};




CC_END_NAMESPACE

#endif 
