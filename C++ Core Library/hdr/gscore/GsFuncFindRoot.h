/* $Header: /home/cvs/src/gsfunc/src/gscore/GsFuncFindRoot.h,v 1.8 2001/11/27 22:45:47 procmon Exp $ */
/****************************************************************
**
**	GSFUNCFINDROOT.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncFindRoot.h,v 1.8 2001/11/27 22:45:47 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCFINDROOT_H )
#define IN_GSFUNCFINDROOT_H

#include <gscore/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

#include <gscore/err.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSFUNC GsFuncFindRoot :
	public GsFunc<GsFuncHandle<double, double>, double>
{
public:
	GsFuncFindRoot( double guess = 0.0, 
				    double tolerance = 1e-10 );
	GsFuncFindRoot(double guess, 
				   double tolerance, 
				   double lower, 
				   double upper );
	virtual ~GsFuncFindRoot() {}

	double operator()( GsFuncHandle< double, double > ) const;

	double getTolerance() const { return m_tolerance; }
	double getGuess() const { return m_guess; }
	static double getMrNumIterations() { return s_mrNumIterations; }

	void setTolerance( double tolerance ) { m_tolerance = tolerance; }
	void setGuess    ( double guess     ) { m_guess = guess;     }

private: // ... member functions ....
	inline static void checkHuge( double x );
	static int s_genfunc( double x, void* Context, double* Result );

private: // .... data members ....
	double m_tolerance;
	double m_guess;

	double m_lower;
	double m_upper;
	bool m_constrain_range;

	static unsigned s_mrNumIterations; 	
	static GsFuncHandle< double, double > s_gf;
};

// .... Inlines ....
void GsFuncFindRoot::checkHuge( double x )
{
	if( x == HUGE_VAL )

		GSX_THROW( GsXInvalidOperation, 
	   		       GsString( "GsFuncFindRoot::() returned HUGE_VAL\n" ) 
				   + 
				   ErrGetString() );
}

CC_END_NAMESPACE

#endif 
