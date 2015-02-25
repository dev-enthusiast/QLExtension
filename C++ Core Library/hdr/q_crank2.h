/****************************************************************
**
**	Q_CRANK2.H	- 2-Dimensional Crank Nicolson routines
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.30 $
**
****************************************************************/

#if !defined( IN_Q_CRANK2_H )
#define IN_Q_CRANK2_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

#include <num.h>
#include <n_matrix.h>

typedef struct 
{
	Q_VECTOR          *mu;          	// vector of drift terms, incorporates deltat
	Q_MATRIX          *cov;          	// covariance matrix, incorporates deltat
	int               stateflag;        // 1 if state dependent parameters, 0 otherwise
    int               direction;        // 0 if direction is backward, 1 if forward
	double            *ccRd;            // discount rate for this step, including deltat
	Q_VECTOR          *deltax;          // grid spacing in each dimension
	int               *nGrid;           // number of gridpoints in each dimension
}	Q_FDARGS;

typedef struct 
{
	Q_VECTOR          *mu;          	// vector of vector of drift terms, each vector corresponds to state space
	Q_VECTOR          **cov;          	// matrix of covariance vectors, vector corresponds to state space
    int               direction;        // 0 if direction is backward, 1 if forward
	double            *ccRd;            // discount rate for this step, including deltat
	Q_VECTOR          *deltax;          // grid spacing in each dimension
	int               *nGrid;           // number of gridpoints in each dimension
}	Q_FDARGS_GEN;

EXPORT_C_QUANT double Q_CN2D (
	Q_VECTOR          *Values,      	// option values, to be revised
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
	Q_VECTOR          *B,           	// right side work vector
	Q_VECTOR          *D,           	// work vector needed for conjugate gradient
	Q_VECTOR          *X,           	// work vector needed for conjugate gradient
	Q_VECTOR          *R1,           	// work vector needed for conjugate gradient 
	Q_VECTOR          *R2,           	// work vector needed for conjugate gradient 
	double            *lowboundary1, 	// array of low boundary values in dim 1
	double            *highboundary1, 	// array of low boundary values in dim 1
	double            *lowboundary2, 	// array of low boundary values in dim 2
	double            *highboundary2, 	// array of low boundary values in dim 2
	double            tolerance         // error tolerance for conjugate gradient
	);

EXPORT_C_QUANT int Q_ExplicitStep (
	Q_VECTOR          *Values,      	// option values, to be revised
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
	double            *A,               // nine coefficients determining the diagonal bands
	Q_VECTOR          *B,           	// right side work vector
	double            *lowboundary1, 	// array of low boundary values in dim 1
	double            *highboundary1, 	// array of low boundary values in dim 1
	double            *lowboundary2, 	// array of low boundary values in dim 2
	double            *highboundary2 	// array of low boundary values in dim 2
	);

EXPORT_C_QUANT int Q_ConjugateGradient ( 
	double           *a,         // fixed array of 9 doubles	
	Q_VECTOR         *B,         // input N x M vector of constants
	Q_VECTOR         *D,         // N x M vector for calculation
	Q_VECTOR         *R1,        // N x M vector for calculation
	Q_VECTOR         *R2,        // N x M vector for calculation
	Q_VECTOR         *X,  	     // initial guess for solution vector, modified then returned
	int              n,          // first dimension
	int              m,          // second dimension
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);
	
EXPORT_C_QUANT int Q_ConjugateGradientSQ ( 
	Q_OPERATOR       A,          // operator on N-long Q_VECTOR
	void             *args,      // arguments needed for the operator
	Q_VECTOR         *B,         // input N vector of constants
	Q_VECTOR         *D,         // N vector for calculation
	Q_VECTOR         *R1,        // N vector for calculation
	Q_VECTOR         *P,         // N vector for calculation
	Q_VECTOR         *Q,         // N vector for calculation
	Q_VECTOR         *U,         // N vector for calculation
	Q_VECTOR         *V,         // N vector for calculation
	Q_VECTOR         *X,  	     // initial guess for solution vector, modified then returned
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);

EXPORT_C_QUANT double Q_MRMCN2D (
	Q_VECTOR          *Values,      	// option values, to be revised
	double            mu1,          	// drift term in first variable, weighted by deltat
	double            mu2,          	// drift term in second variable 
	double            var1,          	// variance term of first variable
	double            var2,          	// variance term of second variable
	double            corr,             // correlation of 2 variables
	double            beta,          	// mean reversion, weighted by deltat
	double            ccRd,             // discount rate
	double            deltax,          	// grid spacing in dimension 1
	double            deltay,          	// grid spacing in dimension 1
	double            lowlimit1,        // lower limit of first variable
	double            lowlimit2,        // lower limit of second variable
	int               nGrid1,           // number of grid spacings in dimension 1
	int               nGrid2,           // number of grid spacings in dimension 2
	Q_VECTOR          *B,           	// right side work vector
	Q_VECTOR          *D,           	// work vector needed for conjugate gradient
	Q_VECTOR          *X,           	// work vector needed for conjugate gradient
	Q_VECTOR          *a1,           	// work vector needed for conjugate gradient 
	Q_VECTOR          *a7,           	// work vector needed for conjugate gradient 
	Q_VECTOR          *R1,           	// work vector needed for conjugate gradient 
	Q_VECTOR          *R2,           	// work vector needed for conjugate gradient 
	double            *lowboundary1, 	// array of low boundary values in dim 1
	double            *highboundary1, 	// array of low boundary values in dim 1
	double            *lowboundary2, 	// array of low boundary values in dim 2
	double            *highboundary2, 	// array of low boundary values in dim 2
	double            tolerance         // error tolerance for conjugate gradient
	);
	
EXPORT_C_QUANT int Q_MRMExplicitStep (
	Q_VECTOR          *Values,      	// option values, to be revised
	double            mu1,          	// drift term in first variable, weighted by deltat
	double            mu2,          	// drift term in second variable 
	double            var1,          	// variance term of first variable
	double            var2,          	// variance term of second variable
	double            corr,             // correlation of 2 variables
	double            beta,          	// mean reversion, weighted by deltat
	double            ccRd,             // discount rate
	double            deltax,          	// grid spacing in dimension 1
	double            deltay,          	// grid spacing in dimension 1
	double            lowlimit1,        // lower limit of first variable
	double            lowlimit2,        // lower limit of second variable
	int               nGrid1,           // number of grid spacings in dimension 1
	int               nGrid2,           // number of grid spacings in dimension 2
	double            *A,           	// 9-long vector of coefficients
	Q_VECTOR          *B,           	// right side work vector
	Q_VECTOR          *a1,           	// work vector needed for conjugate gradient 
	Q_VECTOR          *a7,           	// work vector needed for conjugate gradient 
	double            *lowboundary1, 	// array of low boundary values in dim 1
	double            *highboundary1, 	// array of low boundary values in dim 1
	double            *lowboundary2, 	// array of low boundary values in dim 2
	double            *highboundary2 	// array of low boundary values in dim 2
	);

EXPORT_C_QUANT double Q_MRMConjugateGradient ( 
	double           *A,         // 9-long vector of coefficients
	Q_VECTOR         *A1,        // fixed N x M vector of coefficients
	Q_VECTOR         *A7,        // fixed N x M vector of coefficients
	Q_VECTOR         *B,         // input N x M vector of constants
	Q_VECTOR         *D,         // N x M vector for calculation
	Q_VECTOR         *R1,        // N x M vector for calculation
	Q_VECTOR         *R2,        // N x M vector for calculation
	Q_VECTOR         *X,  	     // initial guess for solution vector, modified then returned
	int              N,          // number of rows
	int              M,          // number of columns
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);		 

EXPORT_C_QUANT int Q_ConjugateGradientGen ( 
	Q_OPERATOR       A,          // operator on N-long Q_VECTOR
	void             *args,      // arguments needed for the operator
	Q_VECTOR         *B,         // input N vector of constants
	Q_VECTOR         *D,         // N vector for calculation
	Q_VECTOR         *R1,        // N vector for calculation
	Q_VECTOR         *R2,        // N vector for calculation
	Q_VECTOR         *X,  	     // initial guess for solution vector, modified then returned
	double           tolerance,  // return when error is this small
	int              maxIter     // also return after this many iterations
	);

EXPORT_C_QUANT int Q_CNImplicit (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS          *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_CNImplicitGen (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS_GEN      *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_CNExplicit (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS          *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_FDOperator(
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS          *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_FDOperatorGen(
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS_GEN      *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_CNIterate (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
    int               stateflag,        // 1 if parameters are state-dependent, 0 otherwise
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *R2,              // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance,        // error tolerance in conjugate gradient
	int               nSteps            // number of time steps executed
	);

EXPORT_C_QUANT int Q_CrankNicolsonND (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
	int               stateflag,        // 1 if state dependent parameters, 0 otherwise
	int               direction,        // 0 if backward equation, 1 if forward equation
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *R2,              // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_CNIterateGen(
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_VECTOR 		  **cov,   			// covariance matrix 
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *R2,              // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance,        // error tolerance in conjugate gradient
	int               nSteps            // number of time steps executed
	);

EXPORT_C_QUANT int Q_CrankNicolsonGen (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_VECTOR 		  **cov,   			// covariance matrix 
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *R2,              // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_ExplicitND (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
    int               stateflag,        // 1 if parameters are state-dependent, 0 otherwise
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *NewValues      	// option values, to be returned
	);

EXPORT_C_QUANT int Q_ExpOperatorND (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
    int               stateflag,        // 1 if state dependent parameters, 0 otherwise
	int               direction,        // 0 if backward equation, 1 if forward equation
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_IndexInv (
	int      index,  // index in the long vector
	int      *ng,    // number of gridpoints in each dimension
	int      dim,    // number of dimensions
	int      *i      // a particular grid point ( i1,i2,...,in ), calculated
	);

EXPORT_C_QUANT int Q_AndersenL (
	double             lRebate,    // lower rebate at lower point of grid
	double             uRebate,    // upper rebate at high point of grid
	Q_VECTOR           *Times,     // times for local volatility
	Q_VECTOR           *Spots,     // times for local volatility
	Q_MATRIX           *volMatrix, // matrix of local vols
	double             jumpMean,   // mean of jump in log space              
	double			   jumpSD,     // standard deviation of jump             
	double			   lambda,     // frequency of jumps
	DATE               prcDate,    // pricing date
	DATE               start,      // start of time interval
	DATE               end,        // end of time interval
	DT_CURVE      	   *disc,      // domestic discount curve
	DT_CURVE           *fdisc,     // foreign discount curve
	int                nSteps,     // number of time steps
	Q_VECTOR           *SpotVec,   // vector of spots    
	Q_VECTOR           *Values     // input vector of payoffs, which is modified
	);

EXPORT_C_QUANT int Q_Andersen (
	double             lRebate,    // lower rebate at lower point of grid
	double             uRebate,    // upper rebate at high point of grid
	N_GENERIC_FUNC_3D  localvol,   // 3-dimensional function for local volatility
	void               *context,   // context for local vol function
	double             jumpMean,   // mean of jump in log space              
	double			   jumpSD,     // standard deviation of jump             
	double			   lambda,     // frequency of jumps
	DATE               prcDate,    // pricing date
	DATE               start,      // start of time interval
	DATE               end,        // end of time interval
	DT_CURVE      	   *disc,      // domestic discount curve
	DT_CURVE           *fdisc,     // foreign discount curve
	int                nSteps,     // number of time steps
	Q_VECTOR           *SpotVec,   // vector of spots    
	Q_VECTOR           *Values     // input vector of payoffs, which is modified
	);

EXPORT_C_QUANT int Q_AndersenL2 (
	double             lRebate,    // lower rebate at lower point of grid
	double             uRebate,    // upper rebate at high point of grid
	Q_VECTOR           *Times,     // times for local volatility
	Q_VECTOR           *Spots,     // spots for local volatility
	Q_MATRIX           *volMatrix, // matrix of local vols
	double             jumpMean,   // mean of jump in log space              
	double			   jumpSD,     // standard deviation of jump             
	double			   lambda,     // frequency of jumps
	double             start,      // start of time interval
	double             end,        // end of time interval
	Q_VECTOR           *discount,  // discount vector
	Q_VECTOR           *discT,     // discount times vector
	Q_VECTOR           *fdiscount, // foreign discount vector
	Q_VECTOR           *fdiscT,    // foreign discount times vector
	double             deltat,     // size of time step
	Q_VECTOR           *SpotVec,   // vector of spots    
	Q_VECTOR           *Values     // input vector of payoffs, which is modified
	);

EXPORT_C_QUANT int Q_Andersen2 (
	double             lRebate,    // lower rebate at lower point of grid
	double             uRebate,    // upper rebate at high point of grid
	N_GENERIC_FUNC_3D  localvol,   // 3-dimensional function for local volatility
	void               *context,   // context for local vol function
	double             jumpMean,   // mean of jump in log space              
	double			   jumpSD,     // standard deviation of jump             
	double			   lambda,     // frequency of jumps
	double             start,      // start of time interval
	double             end,        // end of time interval
	Q_VECTOR           *discount,  // discount vector
	Q_VECTOR           *discT,     // discount times vector
	Q_VECTOR           *fdiscount, // foreign discount vector
	Q_VECTOR           *fdiscT,    // foreign discount times vector
	double             deltat,     // size of time step
	Q_VECTOR           *SpotVec,   // vector of spots    
	Q_VECTOR           *Values     // input vector of payoffs, which is modified
	);

EXPORT_C_QUANT int Q_AndersenStep (
	Q_VECTOR          *Values,    // input vector of payoffs
	Q_VECTOR          *localvar,  // vector of local variances
	double            drift,      // total drift during this time step
	double            ccRd,       // continuous discounting during this step
	double            deltax,     // grid step size in log space
	double            jumpMean,   // mean of jump in log space
	double            jumpSD,     // standard deviation of jump
	double            lambda,     // probability of a jump in this interval
	double            lRebate,    // value at lower edge of grid
	double            uRebate     // value at upper edge of grid
	);

EXPORT_C_QUANT int Q_LocalVol (
	double        time,
	double        time2, 
	double        spot, 
	Q_LOCAL_VOLS  *Context,
	double        *vol
	);

EXPORT_C_QUANT int Q_Index (
	int      *ng,    // number of gridpoints in each dimension
	int      dim,    // number of dimensions
	int      *i      // a particular grid point ( i1,i2,...,in ), as input
	);

EXPORT_C_QUANT int Q_CrankNicolsonND2 (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
    int               stateflag,        // 1 if parameters are state-dependent, 0 otherwise
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *P,               // an extra work vector
	Q_VECTOR          *Q,               // an extra work vector
	Q_VECTOR          *U,               // an extra work vector
	Q_VECTOR          *V,               // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_CrankNicolsonGen2 (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_VECTOR 		  **cov,   			// covariance matrix 
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *P,               // an extra work vector
	Q_VECTOR          *Q,               // an extra work vector
	Q_VECTOR          *U,               // an extra work vector
	Q_VECTOR          *V,               // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_GridReduce (
	Q_VECTOR      *Values,     // values on old (larger) grid
	double        *deltax,     // grid intervals on larger grid
	double        *deltay,     // grid intervals on smaller grid
	double        *lowX,       // minimum grid levels on larger grid
	double        *lowY,       // minimum grid levels on smaller grid
	int           *ngX,        // number of gridpoints in each dimension, larger grid
	int           *ngY,        // number of gridpoints in each dimension, smaller grid
	int           D,           // number of dimensions
	Q_VECTOR      *NewVal      // new values on narrower grid, returned
	);

EXPORT_C_QUANT int Q_CrankNicolsonJ (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_MATRIX 		  *cov,   			// covariance matrix 
	Q_VECTOR          *jumpMean,        // vector of jump means
	Q_MATRIX          *jumpCov,         // covariance matrix of jumps
	double            lambda,           // frequency of jumps
	double   		  ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *R2,              // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_CrankNicolsonJG (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_VECTOR 		  **cov,   			// covariance matrix 
	Q_VECTOR          *jumpMean,        // vector of jump means
	Q_MATRIX          *jumpCov,         // covariance matrix of jumps
	double            lambda,           // frequency of jumps
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *P,               // an extra work vector
	Q_VECTOR          *Q,               // an extra work vector
	Q_VECTOR          *U,               // an extra work vector
	Q_VECTOR          *V,               // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_ArraySum (
	double           *Values,    // output vector 
	double           scalar1,    // coefficient of first vector
	double           scalar2,    // coefficient of second vector
	int              N,          // add first N values of each
	double           *Values1,   // input vector
	double           *Values2    // input vector
	);

EXPORT_C_QUANT int Q_CrankNicolsonRB (
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_VECTOR 		  *mu,    			// vector of drifts in each dimension
	Q_VECTOR 		  **cov,   			// covariance matrix 
	int               direction,        // 0 if backward, 1 if forward
	double   		  *ccRd,   			// discount rate multiplied by deltat
	Q_VECTOR 		  *deltax,			// vector of grid spacings
	Q_VECTOR      	  *nGrid,			// vector of doubles for grid sizes
	Q_VECTOR          *lowerRebate,     // lower boundary values in each dimension
	Q_VECTOR          *upperRebate,     // upper boundary values in each dimension
	Q_VECTOR          *B,               // an extra work vector
	Q_VECTOR          *D,               // an extra work vector
	Q_VECTOR          *R1,              // an extra work vector
	Q_VECTOR          *P,               // an extra work vector
	Q_VECTOR          *Q,               // an extra work vector
	Q_VECTOR          *U,               // an extra work vector
	Q_VECTOR          *V,               // an extra work vector
	Q_VECTOR          *NewValues,      	// option values, to be returned
	double            tolerance         // error tolerance in conjugate gradient
	);

EXPORT_C_QUANT int Q_FDOperatorRB(
	Q_VECTOR          *Values,      	// option values, to be revised
	Q_FDARGS_GEN      *args,            // structure containing drift, covariance matrix, etc
	Q_VECTOR          *Output         	// output vector
	);

EXPORT_C_QUANT int Q_GridReduceExt (
	Q_VECTOR      *Values,     // values on old (larger) grid
	double        *deltax,     // grid intervals on larger grid
	double        *deltay,     // grid intervals on smaller grid
	double        *lowX,       // minimum grid levels on larger grid
	double        *lowY,       // minimum grid levels on smaller grid
	int           *ngX,        // number of gridpoints in each dimension, larger grid
	int           *ngY,        // number of gridpoints in each dimension, smaller grid
	int           D,           // number of dimensions
	Q_VECTOR      *NewVal      // new values on narrower grid, returned
	);
							
#endif

