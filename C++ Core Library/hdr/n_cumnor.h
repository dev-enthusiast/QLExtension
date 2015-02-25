/* $Header: /home/cvs/src/num/src/n_cumnor.h,v 1.18 2001/09/10 21:39:45 demeor Exp $ */
/****************************************************************
**
**	n_cumnor.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.18 $
**
****************************************************************/

#if !defined( IN_N_CUMNOR_H )
#define IN_N_CUMNOR_H

#if !defined( IN_N_BASE_H )
#include <n_base.h>
#endif

#ifndef IN_QTYPES_H
#include <qtypes.h>
#endif

typedef enum
{
	BS_FORWARD_CALL			= 1,
	BS_FORWARD_PUT			= 2,
	BS_FORWARD_DIGITAL_CALL	= 3,
	BS_FORWARD_DIGITAL_PUT	= 4
} BS_OPTION_TYPE;

EXPORT_C_NUM double N_BlackScholesPrice(
	BS_OPTION_TYPE
			ot,
	double	T,
	double	vol,
	double	Fwd,
	double	K
);

EXPORT_C_NUM double N_CondMeanLN3(
    double          fwd1,    // mean of first log normal				   
	double          fwd2,    // mean of second log normal				   
	double          fwd3,    // mean of third log normal				   
    double          sig1,    // std dev of first normal				   
	double          sig2,    // std dev of second normal				   
	double          sig3,    // std dev of third normal				   
	double          rho12,   // correlation of first 2 normals
	double          rho13,   // correlation of first and third normal
	double          rho23,   // correlation of last 2 normals
	double          X1,      // value of first log normal
	double          X2       // value of second log normal
	);

EXPORT_C_NUM double N_CondSD3(
	double          sig3,    // std dev of third normal				   
	double          rho12,   // correlation of first 2 normals
	double          rho13,   // correlation of first and third normal
	double          rho23    // correlation of last 2 normals
);

EXPORT_C_NUM double N_CondSDN(
	double          sig,     // standard deviation of nth normal
	Q_MATRIX        *rho     // correlation matrix
	);

EXPORT_C_NUM double N_CondMeanN(
	Q_VECTOR        *mu,     // means
	Q_VECTOR        *sd,     // standard deviations
	Q_MATRIX        *rho,    // correlation matrix
	Q_VECTOR        *X       // first n-1 values
	);

EXPORT_C_NUM double N_CondFwdN(
	Q_VECTOR        *fwds,   // forwards
	Q_MATRIX        *cov,    // covariance matrix
	Q_VECTOR        *X       // first n-1 values
	);

EXPORT_C_NUM double N_CondCov(
	double          sig1,    // standard deviation of n-1st normal
	double          sig2,    // standard deviation of nth normal
	Q_MATRIX        *rho     // correlation matrix of all n variables
	);

EXPORT_C_NUM int N_CondCovMatrix(
	Q_MATRIX        *cov,      // covariance matrix of all n variables
	Q_MATRIX        *condcov   // conditional covariance matrix of last m variables
	);

EXPORT_C_NUM int N_GetSubMatrix (
	Q_MATRIX        *mat,      // whole matrix
	int             m,         // size of basic matrix
	int             i1,        // first index greater than m-1
	int             i2,        // second index greater than m-1, different
	Q_MATRIX        *mat2      // resulting matrix of size m+2 or m+1
	);

#endif
