/****************************************************************
**
**	q_energy.h	Energy Two Factor pricing routines
**
**	Copyright 1999 - Goldman Sachs & Company - New York
**
**	$Log: q_energy.h,v $
**	Revision 1.6  2001/11/27 23:13:48  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.4  2000/04/11 17:19:40  goodfj
**	q_energy moved to qmrm.dll
**	
**	Revision 1.3  1999/12/15 11:45:04  goodfj
**	Fix annoying link warnings
**	
**	Revision 1.2  1999/08/27 21:36:18  terraa
**	Blech
**	
**	Revision 1.1  1999/08/27 21:29:00  terraa
**	Initial version
**	
****************************************************************/

#if !defined( IN_Q_ENERGY_H )
#define IN_Q_ENERGY_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QMRM double Q_OptPortOptNAssetNTenorNaiveMC(
    DATE         Today,            // Pricing Date
	Q_OPT_TYPE   callPut,          // option type of compound option
	double       strike,           // strike of compound option
	DATE         CmpExpDate,       // compound option date
    DT_CURVE     *FwdPoints,       // Forward points for each of the Q forwards underlying the N baskets
    Q_MATRIX     *BasketContents,  // Expresses contents of each of the N baskets as a weighted average of Q forwards
 	double       RdCmpExp,         // Domestic rate to compound exp
    Q_VECTOR     *RdUndExpSet,     // For each of the N baskets, domestic rate to under settle
 	double       RfCmpExp,         // Foreign rate to compound exp
    Q_VECTOR     *RfUndExpSet,     // For each of the N baskets, foreign rate to under settle
    Q_VECTOR     *DateUndExp,      // For each of the N baskets, date of underlying exp
    Q_VECTOR     *DateUndExpSet,   // For each of the N baskets, date of underlying settle
	Q_MATRIX     *Quantity,        // For each of the N baskets, M underlying option qtys
	Q_MATRIX     *Types,           // For each of the N baskets, M underlying option types
	Q_MATRIX     *Strikes,         // For each of the N baskets, M underlying strikes
	Q_MATRIX     *FwdFwdVols,	   // For each of the N baskets, M fwd fwd vols between cmpd exp and und exps
    DT_CURVE     *ShortVolC,       // Svol for the market process underlying the N baskets
    double       LongVol,          // LVol for the market process underlying the N baskets 
    double       CorrLS,           // Rho  for the market process underlying the N baskets
    double       Beta,             // Beta for the market process underlying the N baskets
    int          npaths            // number of monte carlo paths 
);

EXPORT_C_QMRM double Q_OptPortOptNAssetNTenorNaive(
    DATE         Today,            // Pricing Date
	Q_OPT_TYPE   callPut,          // option type of compound option
	double       strike,           // strike of compound option
	DATE         CmpExpDate,       // compound option date
    DT_CURVE     *FwdPoints,       // Forward points for each of the Q forwards underlying the N baskets
    Q_MATRIX     *BasketContents,  // Expresses contents of each of the N baskets as a weighted average of Q forwards
 	double       RdCmpExp,         // Domestic rate to compound exp
    Q_VECTOR     *RdUndExpSet,     // For each of the N baskets, domestic rate to under settle
 	double       RfCmpExp,         // Foreign rate to compound exp
    Q_VECTOR     *RfUndExpSet,     // For each of the N baskets, foreign rate to under settle
    Q_VECTOR     *DateUndExp,      // For each of the N baskets, date of underlying exp
    Q_VECTOR     *DateUndExpSet,   // For each of the N baskets, date of underlying settle
	Q_MATRIX     *Quantity,        // For each of the N baskets, M underlying option qtys
	Q_MATRIX     *Types,           // For each of the N baskets, M underlying option types
	Q_MATRIX     *Strikes,         // For each of the N baskets, M underlying strikes
	Q_MATRIX     *FwdFwdVols,	   // For each of the N baskets, M fwd fwd vols between cmpd exp and und exps
    DT_CURVE     *ShortVolC,       // Svol for the market process underlying the N baskets
    double       LongVol,          // LVol for the market process underlying the N baskets 
    double       CorrLS,           // Rho  for the market process underlying the N baskets
    double       Beta,             // Beta for the market process underlying the N baskets
    int          npts              // number of noDesx for integration       
);

EXPORT_C_QMRM double Q_AmortStructEnergyTwoFactorMC(
    DATE         Today,             // Pricing Date
    int          N,                 // Number of amortization periods
    int          M,                 // Number of pricing periods per amortization period
    int          Q,                 // Number of forwards
    int          W,                 // Number of instruments per pricing period 
    Q_VECTOR     *AmortDates,       // For each of the N amortization periods, amortization date (must precede averaging!)
	Q_VECTOR     *AmortLevelA,      // For each of the N amortization periods, amortization level   A
	Q_VECTOR     *AmortLevelB,      // For each of the N amortization periods, amortization level   B
	Q_VECTOR     *AmortPctA,        // For each of the N amortization periods, amortization percent A (Expressed as pct remaining)
	Q_VECTOR     *AmortPctB,        // For each of the N amortization periods, amortization percent B (Expressed as pct remaining)
    Q_VECTOR     *AmortIR,          // For each of the N amortization periods, domestic rate to amort date
    Q_VECTOR     *DateUndExp,       // For each of the N*M pricing periods, underlying expiration date
    Q_VECTOR     *DateUndExpSet,    // For each of the N*M pricing periods, underlying expiration settlement date
	Q_VECTOR     *Quantity,         // For each of the N*M pricing periods, quantity
    Q_VECTOR     *IRUndExpSet,      // For each of the N*M pricing periods, interest rate to expiration settlement dates
    Q_MATRIX     *BasketContents,   // For each of the N*M pricing periods, vector expressing each basket as weighted avg of Q forwards
    Q_MATRIX     *OptionQuantities, // For each of the N*M pricing periods, W instrument quantities
    Q_MATRIX     *OptionTypes,      // For each of the N*M pricing periods, W instrument types
    Q_MATRIX     *OptionStrikes,    // For each of the N*M pricing periods, W instrument strikes
    Q_MATRIX     *OptionFwdFwdVols, // For each of the N*M pricing periods, W instrument fwd fwd vols from each of N amortization dates
    DT_CURVE     *FwdPoints,        // Forward points for each of the Q forwards
    DT_CURVE     *ShortVolC,        // Svol for the underlying market process
    double       LongVol,           // LVol for the underlying market process
    double       Rho,               // Rho  for the underlying market process
    double       Beta,              // Beta for the underlying market process
    int          npaths             // number of monte carlo paths
);

EXPORT_C_QMRM double Q_KOStructEnergyTwoFactorMC(
    DATE         Today,             // Pricing Date
    Q_IN_OUT     InOut,             // In or out
    Q_UP_DOWN    UpDown,            // KO Direction
    double       H,                 // KO Level
    double       KOPremium,         // Premium to be *paid* at KO End for structure
    DT_CURVE     *KOSegment,        // We knockout against these futures during this window (KO against Nrby i+1 between Ti<->Ti+1, first elem is -1)
    DT_CURVE     *FwdPoints,        // Forward points for each of the Q forwards
    Q_MATRIX     *BasketContents,   // For each of the N pricing periods, vector expressing each basket as weighted avg of Q forwards
    double       RdKED,             // Domestic rate to KO End
    Q_VECTOR     *RdUndExpSet,      // Domestic rates   for N pricing periods
    double       RfKED,             // Domestic rate to KO End
    Q_VECTOR     *RfUndExpSet,      // Domestic rates   for N pricing periods
    Q_VECTOR     *DateUndExp,       // Expiration dates for N pricing periods
    Q_VECTOR     *DateUndExpSet,    // Settlement dates for N pricing periods
    Q_MATRIX     *OptionQuantities, // For each of the N pricing periods, W instrument quantities
    Q_MATRIX     *OptionTypes,      // For each of the N pricing periods, W instrument types
    Q_MATRIX     *OptionStrikes,    // For each of the N pricing periods, W instrument strikes
    Q_MATRIX     *OptionFwdFwdVols, // For each of the N pricing periods, W instrument fwd fwd vols from KO End Date
    DT_CURVE     *ShortVolC,        // Svol for the underlying market process
    double       LongVol,           // LVol for the underlying market process
    double       Rho,               // Rho  for the underlying market process
    double       Beta,              // Beta for the underlying market process
    int          npaths             // number of monte carlo paths
);

#endif
