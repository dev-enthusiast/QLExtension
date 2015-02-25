/****************************************************************
**
**	qgrid/AndersenPricer.h	- Pricer based on local vols plus jumps
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/qgrid/src/qgrid/AndersenPricer.h,v 1.7 2001/09/19 14:13:25 lowthg Exp $
**
****************************************************************/

#if !defined( IN_QGRID_ANDERSENPRICER_H )
#define IN_QGRID_ANDERSENPRICER_H

#include	<qgrid/base.h>
#include    <qgrid/DVPricer.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_QGRID AndersenPricer : public DVPricer
{
	public:
		// GsDt support:
		typedef	GsDtGeneric<AndersenPricer> GsDtType;
		GsString		toString() const;
		inline static const char * typeName(){ return "AndersenPricer"; }

		inline AndersenPricer() {}

		inline AndersenPricer( const DVPricer& pricer,
						const GsFuncPiecewiseLinear& cumlambda,
						const GsFuncPiecewiseLinear& jumpcumvar, 
						const GsFuncPiecewiseLinear& jumpcummean ):
			DVPricer( pricer ),
			m_cumlambda( cumlambda ),
			m_jumpcumvar( jumpcumvar ),
			m_jumpcummean( jumpcummean )
		{}

		inline AndersenPricer( const AndersenPricer& rhs ):
			DVPricer( rhs ),
			m_cumlambda( rhs.m_cumlambda ),
			m_jumpcumvar( rhs.m_jumpcumvar ),
			m_jumpcummean( rhs.m_jumpcummean )
		{}

		inline AndersenPricer( const GsDtDictionary& dict ) { FromDictionary( dict ); }

		~AndersenPricer(){}

		// const member functions
		GsDtDictionary* ToDictionary() const;
		void ToDictionary( GsDtDictionary& dict ) const;

		// non-const member functions
		void FromDictionary( const GsDtDictionary& dict );

		// necessary virtual member functions
		BasePricer< SimpleVec >* copy() const { return new AndersenPricer( *this ); }

		// classes inheriting from Pricer1DGrid should give their own implementations of this
		void StepBack( double* Values, int nValues, double LogMinGrid, double LogMaxGrid,
                       double s, double t, double LowBarrier, double HighBarrier,
                       double LowRebate, double HighRebate, double RebateTime ) const;

		GsString ModelDescription() const;

	private:
		GsFuncPiecewiseLinear m_cumlambda;
		GsFuncPiecewiseLinear m_jumpcumvar;
		GsFuncPiecewiseLinear m_jumpcummean;
};


/****************************************************************
**	Adlib Name:	QGridAndersenPricerCreate
**	Summary:		
**	References:		
**	Description:   Creates Andersen Pricer
**	Example:		
****************************************************************/

EXPORT_CPP_QGRID AndersenPricer* QGridAndersenPricerCreate(
    const DVPricer& pricer,					  // inherits from DVPricer
   	const GsFuncPiecewiseLinear& cumlambda,	  // integral of jump frequency * dt
   	const GsFuncPiecewiseLinear& jumpcumvar,  // integral of jump variance * dt
   	const GsFuncPiecewiseLinear& jumpcummean  // integral of jump mean * dt
	);

/****************************************************************
**	Adlib Name:	QGridAndersenPricer_CreatePricer
**	Summary:		
**	References:		
**	Description:   Creates generic pricer from Andersen Pricer
**	Example:		
****************************************************************/

EXPORT_CPP_QGRID Pricer* QGridAndersenPricer_CreatePricer(
	const AndersenPricer& pricer              // Andersen pricer
	);

/****************************************************************
**	Adlib Name:	QGridAndersenPricerFromDict
**	Summary:		
**	References:		
**	Description:   Creates Andersen Pricer from GsDtDictionary
**	Example:		
****************************************************************/

EXPORT_CPP_QGRID AndersenPricer* QGridAndersenPricerFromDict(
	const GsDtDictionary& dict	// the dictionary to convert to pricer
);

CC_END_NAMESPACE

#endif



