/****************************************************************
**
**	Q_CRANK.H	headers for Crank-Nicholson style programs
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.13 $
**
****************************************************************/

#if !defined( IN_Q_CRANK_H )
#define IN_Q_CRANK_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT int Q_MeanRevertCN(
	Q_OPT_TYPE      PutOrCall,	// option type
	Q_UP_DOWN       UpOrDown,	// direction of knockout
	Q_OPT_STYLE     AmerEuro,	// option style
	double     		spot,		// current spot
	double     		strike,		// current strike
	Q_VECTOR   		*outval,	// vector of knockout levels
	double     		rebate,		// rebate
	Q_REBATE_TYPE   rebateType,	// rebate type
	int        		nSteps,		// number of steps
	double     		range,		// ratio of highest price to middle
	int        		fineness,   // number of gridpts from middle to end
	Q_VECTOR   		*termVol,	// vector of term volatilities
	double     		exerTime,   // time at which we can start to exercise, if American
	Q_VECTOR   		*timesteps,	// vector of time steps
	Q_VECTOR   		*rate,		// vector of domestic rates
	Q_VECTOR   		*carry,		// vector of carry terms
	Q_VECTOR   		*beta,		// vector of betas
	Q_VECTOR   		*OptionVec,	// vector of option prices
	Q_VECTOR   		*SpotVec	// vector of spot prices
	);

EXPORT_C_QUANT double Q_CrankNicolson2D (
	Q_MATRIX          *Values,      	// option values, to be revised
	double            mu1,          	// drift term in first variable, weighted by deltat
	double            mu2,          	// drift term in second variable 
	double            var1,          	// variance term of first variable
	double            var2,          	// variance term of second variable
	double            corr,             // correlation of 2 variables
	double            ccRd,             // discount rate
	double            deltax,          	// grid spacing in dimension 1
	double            deltay,          	// grid spacing in dimension 1
	int               nGrid1,           // number of grid spacings in dimension 1
	int               nGrid2,           // number of grid spacings in dimension 2
	Q_MATRIX          *B,           	// right side work matrix
	Q_MATRIX          *D,           	// work matrix needed for conjugate gradient
	Q_MATRIX          *X,           	// work matrix needed for conjugate gradient
	Q_MATRIX          *R1,           	// work matrix needed for conjugate gradient 
	Q_MATRIX          *R2,           	// work matrix needed for conjugate gradient 
	double            *lowboundary1, 	// array of low boundary values in dim 1
	double            *highboundary1, 	// array of low boundary values in dim 1
	double            *lowboundary2, 	// array of low boundary values in dim 2
	double            *highboundary2, 	// array of low boundary values in dim 2
	double            tolerance         // error tolerance for conjugate gradient
	);
	
#endif
