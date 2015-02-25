/* $Header: /home/cvs/src/quant/src/q_mulcom.h,v 1.14 2000/07/10 11:45:15 goodfj Exp $ */
/****************************************************************
**
**	q_mulcom.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.14 $
**
****************************************************************/

#if !defined( IN_Q_MULCOM_H )
#define IN_Q_MULCOM_H

#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif
#if !defined( IN_N_CRANK_H )
#include <n_crank.h>
#endif

typedef	double (*CRANK_FUNC)( double x, double t, CRANK_NICHOLSON *pCN );
typedef	double (*CRANK_FUNC_INIT)( double x, CRANK_NICHOLSON *pCN );

double Accumulated_MultiDKO_BlackScholes( 
	double				x, 
	double 				t, 
	CRANK_NICHOLSON     *pCN
);

double Accumulated_MultiDKO_Up( 
	double				x, 
	double 				t, 
	CRANK_NICHOLSON     *pCN
);

double Accumulated_MultiDKO_Down( 
	double 			x, 
	double 			t, 
	CRANK_NICHOLSON *pCN 
);

double Accumulated_MultiDKO_Payoff( 
	double 			x, 
	CRANK_NICHOLSON *pCN 
);

double MultiDKO_BlackScholes( 
	double				x, 
	double 				t, 
	CRANK_NICHOLSON     *pCN
);

double MultiDKO_Up( 
	double				x, 
	double 				t, 
	CRANK_NICHOLSON     *pCN
);

double MultiDKO_Down( 
	double 			x, 
	double 			t, 
	CRANK_NICHOLSON *pCN 
);

double MultiDKO_Payoff( 
	double 			x, 
	CRANK_NICHOLSON *pCN 
);

double identity( 
	double 		x 
);

int	isExpirationNow
(
   	Q_OPT_TYPE		isCall,
    Q_REBATE_TYPE 	rebType,
	double			Texp,
	double			spot,
	double			strike,
	AIA				*lowOutVal,
	AIA				*highOutVal,
	AIA				*lowRebate,
	AIA				*highRebate,
	CRANK_CURVE 	*cc,
	double			*p
);

int	isBeyondKO
(
   	Q_OPT_TYPE		isCall,
    Q_REBATE_TYPE 	rebType,
	double			Texp,
	double			spot,
	double			strike,
	AIA				*lowOutVal,
	AIA				*highOutVal,
	AIA				*lowRebate,
	AIA				*highRebate,
	CRANK_CURVE 	*cc,
	double			*p
);

int	setUpBlocks(
   	Q_OPT_TYPE		isCall,
    Q_REBATE_TYPE 	rebType,
	double			Texp,
	double			spot,
	double			strike,
	AIA				*lowOutVal,
	AIA				*highOutVal,
	AIA				*lowRebate,
	AIA				*highRebate,
	CRANK_CURVE 	*cc,
	int				nBlock,
	CRANK_NICHOLSON_INPUT *block,
	PARAMETERS_LIST_MULTI_DKO *plmd,
	CRANK_FUNC		boundary_up,
	CRANK_FUNC		boundary_down,
	CRANK_FUNC_INIT boundary_initial, 
	double			AssetDiscretizationFactor,
	double			tDiscretizationFactor,
	int				min_time_discretization,
	int				time_discretization_factor,
	int				min_n_minus_plus,
	int				max_n_minus_plus,
	int				asset_discretization_factor,
	int				min_time_steps,
	int				max_time_steps,
	int				nstd_dev,
	double			rangeStdDev
);

void blockAlloc(
	AIA		*endPeriod,
	int		*nBlock,
	CRANK_NICHOLSON_INPUT
			**block
);

int DiscretizationMultiplier(
	int				nBlock,
	CRANK_NICHOLSON_INPUT	*block,
	double			AssetDiscretizationFactor,
	double			tDiscretizationFactor
);


#endif

