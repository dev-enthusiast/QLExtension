/****************************************************************
**
**	GSRATEANNUITY.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsRateAnnuity.h,v 1.1 2000/03/13 11:46:53 thompcl Exp $
**
****************************************************************/

#if !defined( IN_FIINSTR_GSRATEANNUITY_H )
#define IN_FIINSTR_GSRATEANNUITY_H

#include <fiinstr/base.h>
#include <fiinstr/GsPriceAnnuity.h>
#include <fiinstr/GsAnnuityRate.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRateAnnuity 
** Description : GsRateAnnuity class
****************************************************************/

class EXPORT_CLASS_FIINSTR GsRateAnnuity : public GsPriceAnnuity
{
protected:

	GsAnnuityRate
			m_AnnuityRate;

public:
	GsRateAnnuity();
	GsRateAnnuity( const GsRDate& AnnuityDates, double Notional, const GsAnnuityRate& AnnuityRate, 
				   bool CalcVales = true );
	GsRateAnnuity( const GsRateAnnuity & );

	~GsRateAnnuity();

	GsRateAnnuity &operator=( const GsRateAnnuity &rhs );

	virtual double price( size_t Index ) const;
	virtual double term ( size_t Index ) const;
	virtual double rate ( size_t Index ) const;

	void setAnnuityRate( const GsAnnuityRate& AnnuityRate );

	typedef GsDtGeneric< GsRateAnnuity > GsDtType;

	static const char* typeName() { return "GsRateAnnuity"; }
	GsString toString() const { return "GsRateAnnuity"; }

	static const GsAnnuityRate& defaultRate();	

};


CC_END_NAMESPACE

#endif 
