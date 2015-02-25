/****************************************************************
**
**	BasePricer.h	-  Templated base class for option pricing classes
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.19 $
**
****************************************************************/

#if !defined( IN_BASEPRICER_H )
#define IN_BASEPRICER_H

#include <gsquant/GsFuncPiecewiseLinear.h>

CC_BEGIN_NAMESPACE( Gs )

class Pricer;

template< class X > class BasePricer : public GsRefCount
{
	public:
		/***********************************************************************
		** Cast to a generic Pricer object									  **
		***********************************************************************/

		// constructs a Pricer, makes a copy of the object to do this
		virtual Pricer* CreatePricer() const;

		// constructs a Pricer, doesn't copy so object can no longer be used
		Pricer* ConvertToPricer();

		/***********************************************************************
		** These virtual member functions are required of the derived class   **
		** for proper functionality									          **
		***********************************************************************/

		// required for using CreatePricer
		virtual BasePricer< X >* copy() const;

		// required for everything
		virtual void SetPayout( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		// required for everything
		virtual void StepBack( X& Grid, double s, double Time, double LowerKO, double UpperKO,
							  double LowerRebate, double UpperRebate, double RebateTime ) const;

		// required for everything
		virtual double Price( const X& Grid, double LowerKO, double UpperKO, double LowerRebate,
							  double UpperRebate, double RebateTime ) const;
		virtual double Price( const X& Grid ) const;	// can be used instead if knockouts aren't handled

		// required if not vanilla terminal-value style option
		virtual void Multiply( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		// required for Bermudan style options
		virtual void MaxDbl( X& Grid, double a, double Time ) const;

		// required to get a description of which model is in use
		virtual GsString ModelDescription() const;

		virtual GsDt* ToGsDt( const X& Grid, double Time ) const;

		/***********************************************************************
		** These virtual member functions have a default action, which can be **
		** over-ridden if this is not what is required				          **
		************************************************************************/

		virtual void MultiplyDbl( X& Grid, double a, double Time ) const; // Grid *= a;

		virtual void AddDbl( X& Grid, double a, double Time ) const; // Grid += a;

		virtual void AddGrids( X& Grid, const X& Grid2, double Time ) const; // Grid += Grid2

		virtual void SubGrids( X& Grid, const X& Grid2, double Time ) const;

		virtual void AddProduct( X& Grid, const X& RHSGrid, const GsFuncPiecewiseLinear& f,
                                 double Time ) const; // Grid += RHSGrid * f;

		virtual GsDt* Value( const X& Grid, double Time, double LowerKO, double UpperKO,
							 double LowerRebate, double UpperRebate, double RebateTime ) const;

		/***********************************************************************
		** These virtual member functions are defined in terms of previous    **
		** ones. These definitions can be over-ridden for greater efficiency  **
		************************************************************************/

		virtual void SetPayoutDbl( X& Grid, double a, double Time ) const;
        
		virtual void AddPwl( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		virtual void SubPwl( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		virtual void MaxGrids( X& Grid, const X& Grid2, double Time ) const;

		virtual void MaxPwl( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		virtual void MinDbl( X& Grid, double a, double Time ) const;

		virtual void MinGrids( X& Grid, const X& Grid2, double Time ) const;

		virtual void MinPwl( X& Grid, const GsFuncPiecewiseLinear& f, double Time ) const;

		// finally, the destructor
		virtual ~BasePricer();
};

CC_END_NAMESPACE

#endif

