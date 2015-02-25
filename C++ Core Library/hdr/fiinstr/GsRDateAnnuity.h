/****************************************************************
**
**	GSRDATEANNUITY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsRDateAnnuity.h,v 1.1 2000/03/13 11:46:53 thompcl Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_GSRDATEANNUITY_H )
#define IN_FIINSTR_GSRDATEANNUITY_H

#include <fiinstr/base.h>
#include <gscore/types.h>
#include <gscore/GsRDateCurve.h>


CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRDateAnnuity 
** Description : GsRDateAnnuity class
****************************************************************/

class EXPORT_CLASS_FIINSTR GsRDateAnnuity : public GsRDateCurve
{
public:

	GsRDateAnnuity( const GsRDate& RDate );
	GsRDateAnnuity( const GsRDateAnnuity& Other );

	~GsRDateAnnuity();

	GsRDateAnnuity &operator=( const GsRDateAnnuity &rhs );

	virtual void calcValues() {};

	virtual double payment( size_t Index) const; 

	typedef GsDtGeneric< GsRDateAnnuity > GsDtType;

	static const char* typeName() { return "GsRDateAnnuity"; }
	GsString toString() const { return "GsRDateAnnuity"; }

};

EXPORT_CPP_FIINSTR double
		GsRDateAnnuityPayment( const GsRDateAnnuity& RDateAnnuity, int Index );

CC_END_NAMESPACE

#endif 

