/* $Header: /home/cvs/src/quant/src/q_trivar.h,v 1.7 2000/03/03 17:54:37 goodfj Exp $ */
/****************************************************************
**
**	Q_TRIVAR.H	
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_Q_TRIVAR_H )
#define IN_Q_TRIVAR_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QBASIC double Q_TrivariateNormal (
	double            X, 
	double            Y, 
	double            Z, 
	double            RHOXY,
	double            RHOXZ,
	double            RHOYZ,
	double            width
	);

EXPORT_C_QBASIC double Q_TrivariateNormal2 (
	double            X, 
	double            Y, 
	double            Z, 
	double            RHOXY,
	double            RHOXZ,
	double            RHOYZ
	);

EXPORT_C_QBASIC double Q_BinTri(
	Q_OPT_TYPE      cp1,  	    // option type on first asset
	Q_OPT_TYPE      cp2,   		// option type on first asset
	Q_OPT_TYPE      cp3,   		// option type on first asset
	Q_VECTOR        *spot,      // 3-long vector of spot prices
	Q_VECTOR        *strike,    // 3-long vector of strike prices
	Q_MATRIX        *vol,       // 3 by 3 vol and cross vol matrix
	double          time,       // time to expiration
	double          Rd,         // domestic rate
	Q_VECTOR        *Rf,        // 3-long vector of foreign rates 
	double          payoff		// payoff upon being in the money
    );
	
EXPORT_C_QBASIC double Q_TriBinary(
	Q_OPT_TYPE      *callPut,   // array of call/put switches
	Q_VECTOR        *spot,      // 3-long vector of spot prices
	Q_VECTOR        *strike,    // 3-long vector of strike prices
	Q_MATRIX        *vol,       // 3 by 3 vol and cross vol matrix
	double          time,       // time to expiration
	double          ccRd,       // domestic rate
	Q_VECTOR        *ccRf,      // 3-long vector of foreign rates 
	double          payoff		// payoff upon being in the money
    );
		
#endif

