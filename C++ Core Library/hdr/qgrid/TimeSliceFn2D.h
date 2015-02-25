/****************************************************************
**
**	TimeSlice2DFn.h	- definition of class TimeSliceFn2D
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_SPACESLICEFN2D_H )
#define IN_SPACESLICEFN2D_H

#include	<qgrid/base.h>
#include	<gscore/types.h>
#include	<gscore/GsVector.h>
#include	<qenums.h>
#include	<gscore/GsHandle.h>
#include	<gsdt/GsDtGeneric.h>
#include	<qgrid/enums.h>

CC_BEGIN_NAMESPACE( Gs )

void TrivialTimeSliceFn2D( double s, double t, double minx, double miny,
                            double maxx, double maxy, int nGridx,
                            int nGridy, GsVector& retvals,
                            GsRefCount* context );

class TimeSliceFn2DRep : public GsRefCount
{
	public:
		virtual void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const = 0;
		virtual ~TimeSliceFn2DRep() {}
};

class ConstantTimeSliceFn2D : public TimeSliceFn2DRep
{
	public:
		ConstantTimeSliceFn2D( double a = 0 ) : m_a( a ) {}
		~ConstantTimeSliceFn2D() {}
		void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const;
	private:
		double	m_a;
};

class ConstantCoeff2D : public TimeSliceFn2DRep
{
	public:
		ConstantCoeff2D( double a = 1. ) : m_a( a ) {}
		void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const;

	private:
		double	m_a;
};

class TimeSliceFn2D : public GsHandleReadOnly< TimeSliceFn2DRep >
{
	public:
		inline TimeSliceFn2D( const TimeSliceFn2DRep* f = 0 ) : GsHandleReadOnly< TimeSliceFn2DRep >( f ) {}
		inline void NewRep( const TimeSliceFn2DRep* f ) { GsHandleReadOnly< TimeSliceFn2DRep >::operator=( f ); }
		void operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy, GsVector& retvals ) const
			{ m_rep->operator()( s, t, minx, miny, maxx, maxy, nGridx, nGridy, retvals ); }
		GsMatrix operator()( double s, double t, double minx, double miny, double maxx, double maxy, int nGridx, int nGridy ) const;
};

CC_END_NAMESPACE

#endif

