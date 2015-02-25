/****************************************************************
**
**	GSFUNCSOLVERSMOOTH.H	- Smooth constraint solver
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncSolverSmooth.h,v 1.1 2000/06/16 20:55:45 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCSOLVERSMOOTH_H )
#define IN_GSQUANT_GSFUNCSOLVERSMOOTH_H

#include <gsquant/base.h>
#include <gsquant/GsFuncSolverCFR.h>

CC_BEGIN_NAMESPACE( Gs )

class GsFuncSolverSmooth : public GsFuncSolverCFR
{
public:
	GsFuncSolverSmooth( const GsFuncConstraintVector& constraints , 
						double origin = 0.0, double guess = 0.0, double tolerance = 1e-10
					  );
	GsFuncSolverSmooth( const GsFuncSolverSmooth& other );
	GsFuncSolverSmooth& operator=( const GsFuncSolverSmooth& other );
	virtual ~GsFuncSolverSmooth();

	virtual GsFunc< double, double >* Integrate( double initialValue ) const;
	// GsDt Support.
	typedef GsDtGeneric<GsFuncSolverSmooth> GsDtType;
	inline static const char * typeName() { return "GsFuncSolverSmooth"; }

	friend class GsFuncConstraintRootSmooth;
protected:
	GsFuncSolverSmooth();
	virtual void fitConstraints() const;

	mutable GsVector m_values;
	
};

EXPORT_CPP_GSQUANT GsFuncScFunc GsFuncSolverSmoothCreate(const GsDtArray& array , double origin, double guess, double tolerance );
CC_END_NAMESPACE

#endif 
