/****************************************************************
**
**	GSFUNCALGEBRA.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsfunc/src/gscore/GsFuncAlgebra.h,v 1.17 2004/10/26 20:44:56 khodod Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCALGEBRA_H )
#define IN_GSFUNCALGEBRA_H

#include <gscore/GsFunc.h>
#include <gscore/GsFuncHandle.h>

CC_BEGIN_NAMESPACE( Gs )

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator+( const GsFuncHandle<Domain, Range> &f, 
				   const GsFuncHandle<Domain, Range> &g );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator+( const GsFuncHandle<Domain, Range> &f, Range m );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator+( Range m, const GsFuncHandle<Domain, Range> &f );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator-( const GsFuncHandle<Domain, Range> &f, 
				   const GsFuncHandle<Domain, Range> &g );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator-( const GsFuncHandle<Domain, Range> &f, Range m );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator-( Range m, const GsFuncHandle<Domain, Range> &f );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator*( const GsFuncHandle<Domain, Range> &f, 
				   const GsFuncHandle<Domain, Range> &g );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator*( const GsFuncHandle<Domain, Range> &f, Range m );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator*( Range m, const GsFuncHandle<Domain, Range> &f );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator/( const GsFuncHandle<Domain, Range> &f, 
				   const GsFuncHandle<Domain, Range> &g );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator/( const GsFuncHandle<Domain, Range> &f, Range m );

template <class Domain, class Range>
		GsFuncHandle<Domain, Range> 
		operator/( Range m, const GsFuncHandle<Domain, Range> &f );


CC_END_NAMESPACE

#endif 



		
