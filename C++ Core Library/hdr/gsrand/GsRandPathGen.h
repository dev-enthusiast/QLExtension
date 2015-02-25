/****************************************************************
**
**	GSRANDPATHGEN.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsrand/src/gsrand/GsRandPathGen.h,v 1.1 2000/08/17 14:02:57 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSRAND_GSRANDPATHGEN_H )
#define IN_GSRAND_GSRANDPATHGEN_H

#include <gsrand/base.h>
#include <gscore/GsMatrix.h>
#include <gsrand/GsRandGaussCorrelated.h>
#include <gsdt/GsDtGeneric.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsRandPathGen 
** Description : A path generator.
****************************************************************/

class EXPORT_CLASS_GSRAND GsRandPathGen
{
public:
	GsRandPathGen( const GsMatrix& Mat, size_t PathLen, INT32 Seed, bool Antithetic );
	GsRandPathGen( const GsRandPathGen & );

	~GsRandPathGen();

	GsRandPathGen &operator=( const GsRandPathGen &rhs );

	// Return the current path
	GsMatrix CurrentPath() const { return m_CurrentPath; }

	// Generate the next path.
	void NextPath();

	// GsDt support
	typedef GsDtGeneric< GsRandPathGen > GsDtType;
	GsString toString() const { return "GsRandPathGen"; }
	inline static const char * typeName() { return "GsRandPathGen"; }

private:

	size_t	m_PathLen;

	GsRandGaussCorrelated
			m_RandGaussCorrelated;
		
	bool	m_Antithetic;

	bool	m_AntitheticParity;

	GsMatrix
			m_CurrentPath;

};


// Addins:

EXPORT_CPP_GSRAND GsRandPathGen * GsRandPathGenCreate(
	const GsMatrix& Mat, 	// Correlation matrix
	size_t PathLen, 		// Length of the path
	size_t Seed,			// Seed
	bool Antithetic			// true if antithetic
);


EXPORT_CPP_GSRAND GsMatrix GsRandPathGenGetPath(
	const GsRandPathGen &PathGen	// The path generator
);


EXPORT_CPP_GSRAND GsRandPathGen * GsRandPathGenNextPath(
	const GsRandPathGen &PathGen	// The path generator
);


CC_END_NAMESPACE

#endif 
