/* $Header: /home/cvs/src/kool_ade/src/round.h,v 1.12 2007/10/03 19:23:11 khodod Exp $ */
/****************************************************************
**
**	ROUND.H	
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined( IN_ROUND_H )
#define IN_ROUND_H

#include	<math.h>
#include	<float.h>

/*
**	Rounding macros
*/

#ifdef WIN32
#define nextafter _nextafter
#endif

#ifdef __cplusplus
inline double ROUND0( double d )
{
	double n = nextafter( d, DBL_MAX );
	return n - d < 0.5 ? floor( n + 0.5 ) : floor( d + 0.5 );
}
#else
#define ROUND0(d)					( nextafter( (d), DBL_MAX ) - (d) < 0.5 ? floor( nextafter( (d), DBL_MAX ) + 0.5 ) : floor( (d) + 0.5 ) )
#endif

#define ROUNDV(d,v)					( ROUND0( d * v ) / v )
#define ROUND2(d)					ROUNDV(d,100)
#define ROUND3(d)					ROUNDV(d,1000)
#define ROUNDN(d,n)					ROUNDV(d,pow(10.,n))


DLLEXPORT double Round(
	double	Number,		// Number to round or truncate
	double	Fraction,	// Smallest valid fraction, e.g. 0.001
	int		Truncate	// if !Truncate, then round
);


#endif
