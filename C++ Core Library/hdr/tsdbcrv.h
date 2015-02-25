/****************************************************************
**
**	TSDBCRV.H	
**
**	Copyright 1996 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/tsdb/src/tsdbcrv.h,v 1.8 2013/10/01 21:36:14 khodod Exp $
**
****************************************************************/

#if !defined( IN_TSDBCRV_H )
#define IN_TSDBCRV_H

typedef struct
{
	const char 	*Commodity;
	const char 	*Owner;
	const char 	*IntRateName;
	const char 	*IntRateOwner;
	const char 	*Holiday1;
	const char 	*Holiday2;
	int	 	     NumKnots;
	int	 	     NumSimpleIR;		// for interest rates unused for Futures
	const char 	*DayCountMethod;
	double	     CouponPeriod;
		
} TSDB_CURVE_INFO;

typedef struct
{
	int		NumKnots;
	TSDB_POINT	*Pts;
	DATE	Date;
	
} TSDB_CURVE;

#define		TSDB_CURVE_TYPE_UNDEFINED		((long) 0x00000000)
#define		TSDB_CURVE_TYPE_FUT				((long) 0x00000001)
#define		TSDB_CURVE_TYPE_INTRATE			((long) 0x00000002)

#define		TSDB_CURVE_MAX_KNOTS			12*50
#define		TSDB_CURVE_MAX_SIZE				sizeof(TSDB_CURVE_HEADER)+TSDB_CURVE_MAX_KNOTS*sizeof(TSDB_POINT)

#define		AS_TSDB_CURVE_HEADER(RecBuf)	((TSDB_CURVE_HEADER *) RecBuf)
#define		TSDB_CURVE_GET_NUMKNOTS( Sym )	(( Sym->PackSize - sizeof( TSDB_CURVE_HEADER ))/ sizeof( TSDB_POINT ))

typedef struct
{
	DATE	Date;
	long	Type;
	
}	TSDB_CURVE_HEADER;


// in fi_curve.c
int 	TsdbGetCurveInfo( 	 SYMBOL *Sym, TSDB_CURVE_INFO *Info );
int 	TsdbGetInterestInfo( SYMBOL *Sym, TSDB_CURVE_INFO *Info );

#endif

