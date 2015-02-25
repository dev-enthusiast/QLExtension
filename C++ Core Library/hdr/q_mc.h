/****************************************************************
**
**	Q_MC.H
**
**	Copyright 1993 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.6 $
**
****************************************************************/

#if !defined( IN_Q_MC_H )
#define IN_Q_MC_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif


// 1-d grid data type.  f is used for option f(v) to save computation
typedef struct {
    int l;       // length of axis
    double *v;   // values
} axis;

// This typedef is to permit toggleing paths between doubles and floats.  IT IS NOT WORKING YET FOR FLOATS!
typedef double pathdatum;
// points on rate and gold paths correspnd to nodes on the time axis.
typedef struct {
    axis *t;     // time axis
    pathdatum *r;   // rate 
    pathdatum *s;   // gold price
    pathdatum *l;   // path lease rates
} path;

typedef struct {
// Inputs
    axis *dateAxis;
    axis *timeAxis;
    double *zp;     // zero prices
    double *rtv;    // rate term volatility
    double *gf;     // gold forward prices
    double *gtv;    // gold term volatility
    double corr;    // rate/gold spot correlation
    double beta;    // cointegration factor
    double shift;   // shifted log shift
    double boundary;// shifted log boundary
// Cache
    int npaths;
    path *paths;
// Calibration outputs
    pathdatum *rdr; // rate drift adjustment
    pathdatum *rlv; // rate local volatility
    pathdatum *rmr; // rate mean reversion
    pathdatum *gdr; // gold drift adjustment
    pathdatum *glv; // gold local volalitity
} pathobject;


EXPORT_C_Q3FAC Q_VECTOR *newQVector( int size );

EXPORT_C_Q3FAC void freeQVector( Q_VECTOR *v );

EXPORT_C_Q3FAC int Destroy2FPaths( void );

EXPORT_C_Q3FAC int Create2FPaths(
    int npaths,    // Number of paths to generate
    Q_VECTOR *time,// Relative times to nodes
    double corr    // Instanateous correlation of rate and gold paths
);


EXPORT_C_Q3FAC int FitRatePaths(
    Q_VECTOR *S_df,  // Discount factors on the same grid
    Q_VECTOR *S_rlv, // Rate term vols on the same grid
    Q_VECTOR *S_mr,  // Rate mean reversion on the same grid.
    double shift,    // shift parameter
    double boundary  // minimum rate
);


EXPORT_C_Q3FAC int FitGoldTermvols(
    Q_VECTOR *S_gf,   // Gold forwards on the same grid
    Q_VECTOR *S_stv,  // Gold term vols on the same grid
    double beta   // Beta parameter for co-moving leases and rates
);


EXPORT_C_Q3FAC int FitGoldLocalvols(
    Q_VECTOR *S_gf,   // Gold forwards on the same grid
    Q_VECTOR *S_stv,  // Gold term vols on the same grid
    double beta   // Beta parameter for co-moving leases and rates
);


EXPORT_C_Q3FAC int FitGoldForwards(
    Q_VECTOR *S_gf   // gold forwards
);


EXPORT_C_Q3FAC int GetPathParameters(
    Q_VECTOR **rdr,  // rate drifts
    Q_VECTOR **rmr,  // rate drifts
    Q_VECTOR **rlv,  // rate drifts
    Q_VECTOR **gdr,  // rate drifts
    Q_VECTOR **glv   // rate drifts
);


EXPORT_C_Q3FAC int GetPath(
    int ii,          // path number to retrieve
    Q_VECTOR **rate,  // rate buffer to fill
    Q_VECTOR **spot, // spot buffer to fill
    Q_VECTOR **lease // lease buffer to fill
);

EXPORT_C_Q3FAC int GetCrossSection(
    int tt,          // time slice to return
    Q_VECTOR **rate, // rate buffer to fill
    Q_VECTOR **spot, // spot buffer to fill
    Q_VECTOR **lease // lease buffer to fill
);

EXPORT_C_Q3FAC int MonteAmortSwapPricer(
    double   *Value,    // Price
    double   *Err,      // Error
    Q_OPT_TYPE callput, // fwd/call/put flag
    Q_UP_DOWN upOrDown, // knockout direction
    double   leasebasefloat, // initial floating accrual
    double   liborbasefloat, // initial floating accrual
    Q_VECTOR *qstrikes, // strikes
    Q_VECTOR *qstrikedates,  // strike dates
    Q_VECTOR *Amounts,  // number of ounces per payment
    Q_VECTOR *LowPrice, // low price for 100% amortization
    Q_VECTOR *MidPrice, // mid price for some middle amortization
    Q_VECTOR *HighPrice,// high price for 0% amortization
    Q_VECTOR *LowPct,   // percentage at lower level
    Q_VECTOR *MidPct,   // percentage of amortization middle level
    Q_VECTOR *HighPct,  // percentage at upper level
    DATE     prcDate,   // pricing date
    Q_DATE_VECTOR *AmortDates, // amortization times
    DATE     koDate,    // start of knockout period
    int swapflag,       // swapflag
    int leaseflag,      // leases flag
    int liborflag,      // libor flag
    double fixedAdj,    // fixed adj
    double floatAdj,    // float adj
    double cap,         // cap
    double floor  ,     // floor
    double outValL,     // lower ko
    double outValU      // upper ko
);

EXPORT_C_Q3FAC double Q_NewAmortSwapMC(
    Q_UP_DOWN       upOrDown,       // knockout direction
    double 	        spot,    	    // Underlyer spot price
    DT_CURVE 	    *strike,   	    // strike curve
    Q_VECTOR        *Amounts,       // number of ounces per payment
    double          outVal,         // knockout level
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 	    *vol,	        // volatility curve
    double          rangevol,       // used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,    // amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 	    *RdCurve,  	    // domestic rate curve
    DT_CURVE 	    *fwdCurve,      // one forward price per payment time
    int npaths
);

EXPORT_C_Q3FAC double Q_NewAmortFloatSwapMC(
    Q_UP_DOWN       upOrDown,       // knockout direction
    double 	        spot,	        // Underlyer spot price
    double          basefloat,
    DT_CURVE 	    *strike,   	    // strike curve
    Q_VECTOR        *Amounts,       // number of ounces per payment
    double          outVal,         // knockout level
    Q_VECTOR        *LowPrice,      // low price for 100% amortization
    Q_VECTOR        *MidPrice,      // mid price for some middle amortization
    Q_VECTOR        *HighPrice,     // high price for 0% amortization
    Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 	    *vol,	        // volatility curve
    double          rangevol,       // used to calculate the range of integration
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,    // amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 	    *RdCurve,  	    // domestic rate curve
    DT_CURVE 	    *fwdCurve,      // one forward price per payment time
    int npaths
);


EXPORT_C_Q3FAC double Q_AmortSwapApprox( 
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          lvol,          // volatility at strike == LowPrice[0]
    double          hvol,          // volatility at strike == HighPrice[0]
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve   	// forward curve
);


EXPORT_C_Q3FAC double Q_AmortFloatSwapApprox( 
	Q_UP_DOWN       upOrDown,       // knockout direction
	double 			spot,	 		// Underlyer spot price
    double      	basefloat,      // to-date accumulation of floating payments
	DT_CURVE 		*strike,   		// strike curve
	Q_VECTOR        *Amounts,       // number of ounces per payment
	double          outVal,         // knockout level
	Q_VECTOR        *LowPrice,      // low price for 100% amortization
	Q_VECTOR        *MidPrice,      // mid price for some middle amortization
	Q_VECTOR        *HighPrice,     // high price for 0% amortization
	Q_VECTOR        *LowPct,        // percentage at lower level
    Q_VECTOR        *MidPct,        // percentage of amortization middle level
    Q_VECTOR        *HighPct,       // percentage at upper level
    DT_CURVE 		*vol,			// volatility curve
    double          lvol,          // volatility at strike == LowPrice[0]
    double          hvol,          // volatility at strike == HighPrice[0]
    DATE            prcDate,        // pricing date
    Q_DATE_VECTOR   *AmortDates,  	// amortization times
    DATE            koDate,         // start of knockout period
    DT_CURVE 		*RdCurve,  		// domestic rate curve
    DT_CURVE 		*fwdCurve   	// forward curve
);

		
#endif
