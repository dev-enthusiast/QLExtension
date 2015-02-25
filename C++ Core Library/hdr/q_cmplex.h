/* $Header: /home/cvs/src/quant/src/q_cmplex.h,v 1.4 2000/03/03 17:54:27 goodfj Exp $ */
/****************************************************************
**
**	Q_CMPLEX.H	- routines to handle complex arithmetic
**
**	Copyright 1995 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.4 $
**
****************************************************************/

#if !defined( IN_Q_CMPLEX_H )
#define IN_Q_CMPLEX_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif

EXPORT_C_QBASIC int Q_MultComplex (
	double         re1,    // real part of first number
	double         im1,    // imaginary part of first number
	double         re2,    // real part of first number
	double         im2,    // imaginary part of first number
	double         *re,    // returned real part
	double         *im     // returned imaginary part
	);

EXPORT_C_QBASIC int Q_DivComplex (
	double         re1,    // real part of first number
	double         im1,    // imaginary part of first number
	double         re2,    // real part of first number
	double         im2,    // imaginary part of first number
	double         *re,    // returned real part
	double         *im     // returned imaginary part
	);

EXPORT_C_QBASIC int Q_SqrtComplex (
	double         re1,    // real part of input number
	double         im1,    // imaginary part of input number
	double         *re,    // returned real part
	double         *im     // returned imaginary part
	);

EXPORT_C_QBASIC int Q_ExpComplex (
	double         re1,    // real part of input number
	double         im1,    // imaginary part of input number
	double         *re,    // returned real part
	double         *im     // returned imaginary part
	);

EXPORT_C_QBASIC int Q_LogComplex (
	double         re1,    // real part of input number
	double         im1,    // imaginary part of input number
	double         *re,    // returned real part
	double         *im     // returned imaginary part
	);

#endif

