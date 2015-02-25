/****************************************************************
**
**	GSFUNCFINDROOT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsquant/src/gsquant/GsFuncFindRoot.h,v 1.2 2000/07/18 17:34:37 vengrd Exp $
**
****************************************************************/

#if !defined( IN_GSQUANT_GSFUNCFINDROOT_H )
#define IN_GSQUANT_GSFUNCFINDROOT_H

#include <gsquant/base.h>
#include <gsquant/GsFuncSpecial.h>
#include <gscore/GsFuncFindRoot.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_GSQUANT
GsFuncScFunctional GsFuncFindRootCreate( 
	double guess,  	 		// initial guess
	double tolerance 		// tolerance
);

EXPORT_CPP_GSQUANT
GsFuncScFunctional GsFuncFindRootConstrainedCreate( 
	double guess,  	 		// initial guess
	double tolerance, 		// tolerance
	double lower,			// range lower bound
	double upper			// range upper bound
);

CC_END_NAMESPACE

#endif 
