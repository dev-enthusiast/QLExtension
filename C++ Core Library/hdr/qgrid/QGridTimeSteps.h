/****************************************************************
**
**	QGridTimeSteps.h	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.9 $
**
****************************************************************/

#if !defined( IN_QGRIDTIMESTEPS_H )
#define IN_QGRIDTIMESTEPS_H

#include	<qgrid/base.h>
#include	<qenums.h>
#include	<qgrid/enums.h>

CC_BEGIN_NAMESPACE( Gs )
												   
class QGridTimeSteps;

EXPORT_CPP_QGRID double dt_linear( const QGridTimeSteps* This, double s, double t );
EXPORT_CPP_QGRID double dt_sqrt( const QGridTimeSteps* This, double s, double t );
EXPORT_CPP_QGRID double dt_linearnum( const QGridTimeSteps* This, double s, double t );
EXPORT_CPP_QGRID double dt_sqrtnum( const QGridTimeSteps* This, double s, double t );

class QGridTimeSteps
{
	public:
		typedef double (*Method)( const QGridTimeSteps* This, double s, double t ); 

		QGridTimeSteps()
		:
			m_Accuracy( 0.01 ),
			m_dt( &dt_linear )
		{}

		double Accuracy() const { return m_Accuracy; }
		void   SetAccuracy( double a ) { m_Accuracy = a; }
		double dt( double s, double t ) const { return (*m_dt)( this, s, t ); }
		Method MethodUsed() const { return m_dt; }

		void UseLinear()    { m_dt = &dt_linear; }
		void UseSqrt()      { m_dt = &dt_sqrt; }
		void UseLinearNum() { m_dt = &dt_linearnum; }
		void UseSqrtNum()   { m_dt = &dt_sqrtnum; }

	private:
		double m_Accuracy;
		double (*m_dt)( const QGridTimeSteps* This, double s, double t );
};

CC_END_NAMESPACE

#endif

