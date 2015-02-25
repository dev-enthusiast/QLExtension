/****************************************************************
**
**	GSPRICEANNUITY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsPriceAnnuity.h,v 1.1 2000/03/13 11:46:53 thompcl Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_GSPRICEANNUITY_H )
#define IN_FIINSTR_GSPRICEANNUITY_H

#include <fiinstr/base.h>
#include <fiinstr/GsRDateAnnuity.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
** Class	   : GsPriceAnnuity 
** Description : GsPriceAnnuity class
****************************************************************/

class EXPORT_CLASS_FIINSTR GsPriceAnnuity : public GsRDateAnnuity
{
	double m_Notional;

public:
	GsPriceAnnuity();
	GsPriceAnnuity( const GsRDate& AnnuityDates, double Notional, bool CalcValues = true );
	GsPriceAnnuity( const GsPriceAnnuity & );

	~GsPriceAnnuity();

	GsPriceAnnuity &operator=( const GsPriceAnnuity &rhs );

	virtual void calcValues();

	virtual double price( size_t Index ) const;
	virtual double notional( size_t Index ) const;

	typedef GsDtGeneric< GsPriceAnnuity > GsDtType;

	static const char* typeName() { return "GsPriceAnnuity"; }
	GsString toString() const { return "GsPriceAnnuity"; }

};


CC_END_NAMESPACE

#endif 
