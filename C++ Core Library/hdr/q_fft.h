/****************************************************************
**
**	q_fft.h	- Option pricing using the fast fourier transform 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/quant/src/q_fft.h,v 1.17 2001/11/27 23:13:49 procmon Exp $
**
****************************************************************/

#if !defined( IN_Q_FFT_H )
#define IN_Q_FFT_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QUANT int Q_FourierStep  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	double          deltax,		// spacing of log(spot) grid
 	double          drift,		// total drift during time step
	double          var,        // total variance during time step
	double          ccRd        // total rate*t during time step
	);

EXPORT_C_QUANT int Q_FourierStepAdj  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	double          deltax,		// spacing of log(spot) grid
 	double          drift,		// total drift during time step
	double          var,        // total variance during time step
	double          ccRd        // total rate*t during time step
	);

EXPORT_C_QUANT int Q_FourierStepND  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes      // number of gridpoints in each dimension
	);

EXPORT_C_QUANT int Q_FourierStepC  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes      // number of gridpoints in each dimension
	);

EXPORT_C_QUANT int Q_FourierStepJ ( 
	Q_VECTOR        *Values,    // input values of option
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	Q_VECTOR        *jumpMean,  // vector of jump means
	Q_MATRIX        *jumpCov,   // covariance matrix of jumps
	double          lambda,     // frequency of jumps, multiplied by time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes,     // number of gridpoints in each dimension
	Q_VECTOR        *NewValues  // output values of option
	);

EXPORT_C_QUANT int Q_FourierStepCJ  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	Q_VECTOR        *jumpMean,  // vector of jump means
	Q_MATRIX        *jumpCov,   // covariance matrix of jumps
	double          lambda,     // frequency of jumps, multiplied by time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes,     // number of gridpoints in each dimension
	Q_VECTOR        *NewValues  // output values of option
	);

EXPORT_C_QUANT int Q_FourierStepCC  ( 
	DT_COMPLEX      *Data,      // values of option, to be modified
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes      // number of gridpoints in each dimension
	);

EXPORT_C_QUANT int Q_Convolution (
	Q_VECTOR          *Values1,    // first function
	Q_VECTOR          *Values2,    // second function
	Q_VECTOR          *Values      // the convolution
	);

EXPORT_C_QUANT int Q_FourierStepR  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	double          deltax,		// spacing of log(spot) grid
 	double          drift,		// total drift during time step
	double          var,        // total variance during time step
	double          ccRd        // total rate*t during time step
	);

EXPORT_C_QUANT int Q_FourierStepCR  ( 
	Q_VECTOR        *Values,    // values of option, to be modified
	Q_VECTOR        *deltax,	// spacing of log(spot) grid
 	Q_VECTOR        *drift,		// total drift during time step
	Q_MATRIX        *cov,       // total covariance during time step
	double          ccRd,       // total rate*t during time step
	Q_VECTOR        *Sizes      // number of gridpoints in each dimension
	);
				  
#endif

