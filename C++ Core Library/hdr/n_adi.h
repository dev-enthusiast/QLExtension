/****************************************************************
**
**	n_adi.h	- alternating direction implicit
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_N_ADI_H )
#define IN_N_ADI_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#include <n_tensor.h>

typedef enum
{
	EXPLICIT,
	ADI,
	PREDICTOR_CORRECTOR
}  N_FDMETHOD;

EXPORT_C_NUM int N_1DGenerator(
	const Q_DIFFUSION* Diffusion,		// the diffusion
	int				   Dimension,		// the dimension in which to operate
	double			   DiscountFactor,	// multiplier for the discount rate
	double			   DriftFactor,		// multiplier for the drift
	double			   VarianceFactor,	// multiplier for the variance
	const Q_TENSOR*	   ValuesIn,		// values to operate on
	Q_TENSOR*	   	   ValuesOut		// returned values
);

EXPORT_C_NUM int N_ApplyGenerator(
	const Q_DIFFUSION* Diffusion,		// the diffusion
	double			   dt,				// multiplier for the generator
	const Q_TENSOR*	   ValuesIn,		// values to operate on
	Q_TENSOR*	   	   ValuesOut,		// returned values
	Q_VECTOR*		   WorkSpace		// automatically allocates this if too small
);

EXPORT_C_NUM int N_1DImplicitOperator(
	const Q_DIFFUSION* Diffusion,		// the diffusion
	int				   Dimension,		// the dimension in which to operate
	double			   DiscountFactor,	// multiplier for the discount rate
	double			   DriftFactor,		// multiplier for the drift
	double			   VarianceFactor,	// multiplier for the variance
	const Q_TENSOR*	   ValuesIn,		// values to operate on
	Q_TENSOR*		   ValuesOut,		// returned values
	Q_VECTOR*		   WorkSpace		// automatically allocates this if too small
);

EXPORT_C_NUM int N_ADI(
	const Q_DIFFUSION* Diffusion,		// the diffusion
	double			   dt,				// multiplier for the variance
	Q_TENSOR*	   	   Values,			// values to operate on, done in-place
	Q_VECTOR*		   WorkSpace		// at least 3 * max size of tensor in all dimensions
);

EXPORT_C_NUM int N_SingleStepADI(
	const Q_DIFFUSION* Diffusion,	// the diffusion
	double	           dt,			// time step size
	N_FDMETHOD		   fdmethod,	// fd scheme
	const Q_TENSOR*    ValuesIn, 	// initial values
	double*            ValuesOut	// final values
);

EXPORT_C_NUM int N_PredictorCorrector(
	const Q_OP*	G,			// the generator
	const Q_OP* S,			// the smoother
	const Q_VECTOR*	Vin,	// the inputted values
	Q_VECTOR* Vout,			// the outputted values (returned)
	Q_VECTOR* Vtmp			// temp storage
);

#endif

