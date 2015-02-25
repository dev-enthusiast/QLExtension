/* $Header: /home/cvs/src/num/src/n_crank.h,v 1.59 2000/12/05 16:08:22 lowthg Exp $ */
/****************************************************************
**
**	n_crank.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.59 $
**
****************************************************************/

#if !defined( IN_N_CRANK_H )
#define IN_N_CRANK_H

#include	<qtypes.h>
#include	<n_aiarr.h>

typedef enum {
	DISCRETIZATION_REGULAR,
	DISCRETIZATION_IRREGULAR,
	DISCRETIZATION_INVALID
} DISCRETIZATION;


typedef enum {
	INTERPOLATION_METHOD_LINEAR,
	INTERPOLATION_METHOD_SPLINE
} INTERPOLATION_METHOD;

typedef enum {
	INTEGRATION_METHOD_FD,
	INTEGRATION_METHOD_ANALYTIC
} INTEGRATION_METHOD;

typedef enum
{
	N_OPT_EURO	  =		1,
	N_OPT_AMER	  =		2
} N_OPT_STYLE;

typedef enum
{
	N_FADEOUT_NON_RESURRECTING		= 1,
	N_FADEOUT_NON_RESURRECTING_ALT	= 2,
	N_FADEOUT_RESURRECTING	  		= 3,
	N_FADEOUT_NON_R_TO_VANILLA		= 4,
	N_FADEOUT_NON_R_TO_VANILLA_ALT	= 5,
	N_FADEOUT_R_TO_VANILLA			= 6,
	N_FADEOUT_NONE_DISCRETE_KO 		= 7
} N_FADE_OUT_STYLE;

typedef struct 
{
	AIA		valueTracked;
	DYNAMIC_ARRAY
			time,
			result;
} TRACKED_DATA;

typedef struct CN__ CRANK_NICHOLSON;

typedef struct {
	double	tStart,
			tEnd;
		
	int		nT,
			Nminus,
			Nplus,
			optStyle,
			FadeOutStyle;

	double	low,
			mid,
			high;

	double	(*boundary_up)( double x, double t, CRANK_NICHOLSON *pCN ),
			(*boundary_down)( double x, double t, CRANK_NICHOLSON *pCN ),
			(*boundary_initial)( double x, CRANK_NICHOLSON *pCN ),
			(*rebate_discount_factor)( double t, CRANK_NICHOLSON *pCN );

	void	*ExtraParams; // For Slang

	void	*Extra;

	TRACKED_DATA
			*td;
/*
** used for fadeout-knockout
*/
	AIA		FadeOutTimes,
			FadeOutCoeffs;

	double	FadeOutLowerLevel,	//may be the log.
			FadeOutHigherLevel;

	int		nFO;
/*
** Used for Discrete KO.
*/
	double	downRebate,
			upRebate;

} CRANK_NICHOLSON_INPUT;

typedef struct 
{
	AIA		uValues;
	double	(*boundary_up)( double x, double t, CRANK_NICHOLSON *pCN ),
			(*boundary_down)( double x, double t, CRANK_NICHOLSON *pCN ),
			(*boundary_initial)( double x, CRANK_NICHOLSON *pCN );
	double	uLow,
			uHigh;
} EXTRA_U_VALUES;

/*
** CRANK NICHOLSON DATA STRUCTURE
*/

struct CN__	 //CRANK_NICHOLSON
{
    
	double	T;

	AIA		u,
			z,
			a1,
			b1,
			c1,
			a2,
			b2,
			c2,
			ex,
			workSpace;

	AIA		tGrid,
			xGrid,
			uValues,
			alphaPlus,
			betaPlus,
			alpha,
			beta,
			alphaMinus,
			betaMinus;
	

	double	( *coeff_d2u )( double x, double t, double tp, CRANK_NICHOLSON *pCN ),
			( *coeff_du )( double x, double t, double tp, CRANK_NICHOLSON *pCN ),
			( *coeff_u )( double x, double t, double tp, CRANK_NICHOLSON *pCN ),
			( *fromChangedToNatural )( double x ),
			( *fromNaturalToChanged )( double x );

	AIA_DOUBLE_2D
			Acoeff_d2u,
			Acoeff_du,
			Acoeff_u;

	CRANK_NICHOLSON_INPUT
			*pCNI;

	void	*Extra;

	DISCRETIZATION
			dscr;

	double	alphaMinusCst,
			betaMinusCst,	
			alphaCst,   
			betaCst,   	
			alphaPlusCst,
			betaPlusCst,
			uHigh,
			uLow;

	int		N_Extra_uValues;
	EXTRA_U_VALUES *Extra_uValues;
	AIA		extra_p;
/*
** used for fadeout-knockout
*/
	AIA_INT	isFadeOutTimes;

	int		indexFOlow,
	        indexFOhigh;

	int		accumulation;
};

//===========================

EXPORT_C_NUM void N_TRACKED_DATA_Initialize(
	TRACKED_DATA	*td
);

EXPORT_C_NUM void N_TRACKED_DATA_Alloc(
	TRACKED_DATA	*td,
	int				n
);

EXPORT_C_NUM void N_TRACKED_DATA_Free(
	TRACKED_DATA	*td
);
//===========================

int CRANK_ComputeOneStepCrankNicholson(
	CRANK_NICHOLSON	
			*pCN,
	int		m,
	double	theta,
    int		coeffDependOnx
);

void CRANK_TransferFinalToInitialValue( 
	CRANK_NICHOLSON 		*target, 
	CRANK_NICHOLSON 		*source,
	INTERPOLATION_METHOD	im
);

void CRANK_Initialize(
	CRANK_NICHOLSON	*pCN
);

void CRANK_Alloc(
	CRANK_NICHOLSON_INPUT
					*pCNInput,
	CRANK_NICHOLSON	*pCN
);

void CRANK_AllocNottxGrid(
	CRANK_NICHOLSON_INPUT
					*pCNInput,
	CRANK_NICHOLSON	*pCN
);

void CRANK_Free( 
	CRANK_NICHOLSON	*pCN
);

int CRANK_Loop(
	CRANK_NICHOLSON	
			*pCN,
	double	theta,
	int 	(*CrankNicholson)( CRANK_NICHOLSON*, int, double, int ),
	int		lowBound,
	int		highBound,
	int		coeffDependOnx
);

int CRANK_ComputeOneStepFIP(
	CRANK_NICHOLSON	
			*pCN,
	int		index,
	double	theta,
    int		coeffDependOnx
);

double CRANK_Evaluate(
	CRANK_NICHOLSON	
			*pCN,
	double	x
);

void CRANK_AllocxGridIrregular(
	CRANK_NICHOLSON_INPUT
					*pCNInput,
	CRANK_NICHOLSON	*pCN
);

void CRANK_AllocxGridRegular(
	CRANK_NICHOLSON_INPUT
					*pCNInput,
	CRANK_NICHOLSON	*pCN
);

typedef double	(*N_CRANK_COMPUTE1)( double x, double t, double tp, CRANK_NICHOLSON *pCN );
typedef double	(*N_CRANK_COMPUTE2)( double x );

EXPORT_C_NUM double N_CRANK_Compute(
   	CRANK_NICHOLSON_INPUT
			*block,
	int		howManyBlock,
	double	x0,
	N_CRANK_COMPUTE1  coeffu,
	N_CRANK_COMPUTE1  coeffdu,
	N_CRANK_COMPUTE1  coeffd2u,
	N_CRANK_COMPUTE2  fromChangedToNatural,
	N_CRANK_COMPUTE2  fromNaturalToChanged,
	void	*Extra,
	INTERPOLATION_METHOD im,
	double	theta,
	int		coeffDependOnx
);

typedef struct {
	AIA_CURVE	logFwd,
				logDF,
				var;
} CRANK_CURVE;

EXPORT_C_NUM double N_CRANK_Compute_BlackScholes(
   	CRANK_NICHOLSON_INPUT
			*block,
	int		howManyBlock,
	double	x0,
	CRANK_CURVE	
			*cc
);

EXPORT_C_NUM void N_CRANK_CURVE_Initialize(
	CRANK_CURVE *ccp
);

EXPORT_C_NUM int N_CRANK_CURVE_Alloc(
	CRANK_CURVE *ccp,        
	double	    Tstart,      
	double	    Tend,      
	AIA		    *DomesticT,   
	AIA		    *DomesticDF,  
	AIA		    *FwdT,        
	AIA		    *Fwd,
	AIA			*volT,
	AIA			*vol
);

int CRANK_CURVE_Alloc_NoVol(
	CRANK_CURVE
			*ccp,
	double	Tstart,
	double	Tend,
	AIA		*DomesticT,	
	AIA		*DomesticDF,	
	AIA		*FwdT,	
	AIA		*Fwd
);

EXPORT_C_NUM void N_CRANK_CURVE_Free(
	CRANK_CURVE *ccp
);

double N_Crank_coeff_u( double x, double t, double tp, CRANK_NICHOLSON *pCN );
double N_Crank_coeff_du( double x, double t, double tp, CRANK_NICHOLSON *pCN );
double N_Crank_coeff_d2u( double x, double t, double tp, CRANK_NICHOLSON *pCN );

EXPORT_C_NUM void N_CRANK_NICHOLSON_INPUT_Initialize(
	CRANK_NICHOLSON_INPUT
					*cni
);

EXPORT_C_NUM void N_CRANK_NICHOLSON_INPUT_Alloc_FadeOut(
	AIA	*FadeOutTimes,	
	AIA	*lowFadeOutVal,	
	AIA	*highFadeOutVal,
	int	nBlock,
	CRANK_NICHOLSON_INPUT
		*block
);

EXPORT_C_NUM int N_NineDiagMultiply ( 
	double           *a,         // fixed array of 9 doubles	
	Q_MATRIX         *Values,    // input N x M matrix
	Q_MATRIX         *NewValues  // output N x M matrix
	);

EXPORT_C_NUM int N_MatrixXScalar ( 
	double           a,          // arbitrary double
	Q_MATRIX         *Values,    // input N x M matrix
	Q_MATRIX         *NewValues  // output N x M matrix
	);

EXPORT_C_NUM int N_MatrixAdd ( 
	Q_MATRIX         *Values1,   // input N x M matrix
	Q_MATRIX         *Values2,   // input N x M matrix
	Q_MATRIX         *NewValues  // output N x M matrix
	);
	
EXPORT_C_NUM double N_MatrixDotProduct ( 
	Q_MATRIX         *Values1,   // input N x M matrix
	Q_MATRIX         *Values2    // input N x M matrix
	);

EXPORT_C_NUM int N_ConjugateGradient ( 
	double           *a,         // fixed array of 9 doubles	
	Q_MATRIX         *B,         // input N x M matrix of constants
	Q_MATRIX         *D,         // N x M matrix for calculation
	Q_MATRIX         *R1,        // N x M matrix for calculation
	Q_MATRIX         *R2,        // N x M matrix for calculation
	Q_MATRIX         *X,  	     // initial guess for solution matrix, modified then returned
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);

EXPORT_C_NUM int N_NineDiagMultiplyV ( 
	Q_MATRIX         *a0,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a1,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a2,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a3,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a4,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a5,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a6,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a7,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a8,        // fixed N x M matrix of coefficients
	Q_MATRIX         *Values,    // input N x M matrix
	Q_MATRIX         *NewValues  // output N x M matrix
	);

EXPORT_C_NUM double N_ConjugateGradientV ( 
	Q_MATRIX         *a0,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a1,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a2,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a3,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a4,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a5,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a6,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a7,        // fixed N x M matrix of coefficients
	Q_MATRIX         *a8,        // fixed N x M matrix of coefficients
	Q_MATRIX         *B,         // input N x M matrix of constants
	Q_MATRIX         *D,         // N x M matrix for calculation
	Q_MATRIX         *R1,        // N x M matrix for calculation
	Q_MATRIX         *R2,        // N x M matrix for calculation
	Q_MATRIX         *X,  	     // initial guess for solution matrix, modified then returned
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);

/*
**
*/
void EXTRA_U_VALUES_Initialize(
	EXTRA_U_VALUES	*pU
);

void EXTRA_U_VALUES_Free(
	EXTRA_U_VALUES	*pU
);


#endif

