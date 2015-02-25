/****************************************************************
**
**	GSFUNCSOLVERCFR.H	- Generic constraint solver
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncSolverCFR.h,v 1.8 2001/11/27 22:46:45 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCSOLVERCFR_H )
#define IN_GSQUANT_GSFUNCSOLVERCFR_H

#include <gsquant/base.h>
#include <vector>
#include <algorithm>
#include <ccstl.h>
#include <gsquant/GsFuncConstraint.h>
#include <gscore/GsFuncPiecewise.h>
#include <gsquant/GsFuncSpecial.h>

CC_BEGIN_NAMESPACE( Gs )

typedef GsFuncConstraint< double, double > GsFuncConstraintDouble;

typedef CC_STL_VECTOR( GsFuncConstraintDouble ) GsFuncConstraintVector;

class EXPORT_CLASS_GSQUANT GsFuncSolverCFR : public GsFunc< double, double >
{
public:
	GsFuncSolverCFR( const GsFuncConstraintVector& constraints , 
					 double origin = 0.0, double guess = 0.0, double tolerance = 1e-10
				   );
	GsFuncSolverCFR( const GsFuncSolverCFR& other );
	GsFuncSolverCFR& operator=( const GsFuncSolverCFR& other );
	virtual ~GsFuncSolverCFR();

	const GsFuncConstraintVector& getConstraints() const { return m_constraints; }
	const GsVector& knots() const; 
	const GsVector& coefficients() const; 

	double origin() const { return m_origin; }
	double guess() const { return m_guess; }
	double tolerance() const { return m_tolerance; }
	
	double operator() ( double ) const;
	virtual GsFunc< double, double >* Integrate( double initialValue ) const;

	// GsDt Support.
	typedef GsDtGeneric<GsFuncSolverCFR> GsDtType;
	GsString toString() const;
	inline static const char * typeName() { return "GsFuncSolverCFR"; }
	// GsSt Support
  	typedef GsStreamMapT< GsFuncSolverCFR > GsStType;
  	virtual GsStreamMap* buildStreamMapper() const;

	friend class GsFuncConstraintRoot;
protected:
	GsFuncSolverCFR() {}
	
	virtual void fitConstraints() const;
	GsFuncConstraintVector m_constraints;
	
	mutable GsFuncHandle< double, double > m_func;
	double m_guess;
	double m_tolerance;
	double m_origin;
	mutable int m_fitDone;

	mutable GsVector m_knots;
	mutable GsVector m_coefficients;
};


EXPORT_CPP_GSQUANT GsFuncScFunc GsFuncSolverCFRCreate(const GsDtArray& array , double origin, double guess, double tolerance );
EXPORT_CPP_GSQUANT GsFuncConstraintDouble GsFuncConstraintCFRCreate( double bound, double target, const GsFuncScFunctional& func );
EXPORT_CPP_GSQUANT GsFuncScFunc GsFuncIntegrate( const GsFuncScFunc& func, double initialValue );


CC_END_NAMESPACE

#endif 
