/* $Header: /home/cvs/src/gridpricer/src/q_prop.h,v 1.11 2001/10/24 19:14:58 demeor Exp $ */
/****************************************************************
**
**	q_prop.h	Generic pricing engine
**
**	Copyright 1999 - Goldman Sachs & Company - New York
**
**	$Revision: 1.11 $
**	
****************************************************************/

#if !defined( IN_Q_PROP_H )
#define IN_Q_PROP_H

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

#ifndef IN_DATATYPE_H
#include <datatype.h>
#endif

#ifndef IN_QENUMS_H
#include <qenums.h>
#endif

enum Q_FD_METHOD
{
    Q_FD_EXPLICIT = 0,
    Q_FD_IMPLICIT = 1,
    Q_FD_CRANK    = 2,
    Q_FD_FOURIER  = 3
};


struct Q_GRID_DOMAIN {
    int              NumPoints;         // number of elements
    double           min;               // f[0] = f[ X=min ]
    double           max;               // f[len - 1] = f[ X=max ]
    double           dx;                // (max - min)/(NumPoints-1)
    double           Solve;             // Solve for this node on the grid
};


EXPORT_C_GRIDPRICER int G_SparseDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_TridDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_GeneralDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_CrankNicholsonDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);


EXPORT_C_GRIDPRICER int G_SparseDiffuseF(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SparseDiffuseFC(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SparseDiffuseFCC(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_FourierDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SparseDiffuseI(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SparseDiffuseFI(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_SparseDiffuseFJ(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_IntegralDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_CrossCCYDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_HullWhiteExplicit(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_HullWhiteExplicit2(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
);

EXPORT_C_GRIDPRICER int G_GridReduce(
	DT_VECTOR     *fv,         // values on old (larger) grid, which are modified
	double        *deltax,     // grid intervals on larger grid
	double        *deltay,     // grid intervals on smaller grid
	double        *nGrid,      // number of gridpoints in each dimension, both grids
	int           D            // number of dimensions
	);

EXPORT_C_GRIDPRICER void DtMatrixTimesVectorInPlace(
    DT_MATRIX        *m,                // Matrix
    DT_VECTOR        *v,                // Vector
    double           *bf                // Buffer
);

EXPORT_C_GRIDPRICER void SparseMatrixStep(
    int NumSteps,                       // Number of micro timesteps
    Q_GRID_DOMAIN *FactorSpace,         // grid definition
    DT_MATRIX *CovMatrix,               // term covariance
    DT_VECTOR *Drifts,                  // term drift
    DT_VECTOR *Betas,                   // term mean reversion
    int D,                              // number of dimensions
    double dt,                          // micro timestep size
    double OmFac,                       // covarance adjustment factor for damped variables
    DT_VECTOR *fv,                      // incoming & outgoing option value
    double *bf                          // temp buffer
);

EXPORT_C_GRIDPRICER int G_CrankNicolsonND (
	double           deltaT,       // total time step
	double           dt,           // micro timestep size
	int              tt,           // micro timestep counter
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
    DT_VECTOR 		 *Betas,	   // mean reversions, now required to be zero
	DT_VECTOR        *fv,          // incoming and outgoing option value
	double           *b,           // an extra work array
	double           *d,           // an extra work array
	double           *r1,          // an extra work array
	double           *r2,          // an extra work array
	double           ccRd,         // discount rate during this period
	double           tolerance     // error tolerance in conjugate gradient
	);

EXPORT_C_GRIDPRICER int G_CrankNicolsonGen (
	double           deltaT,       // total time step
	double           dt,           // micro timestep size
	int              tt,           // micro timestep counter
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
    DT_VECTOR 		 *Betas,	   // mean reversions, now required to be zero
	DT_VECTOR        *fv,          // incoming and outgoing option value
	double           *b,           // an extra work array
	double           *d,           // an extra work array
	double           *r1,          // an extra work array
	double           *r2,          // an extra work array
	double           *ccRd,        // discount rate during this period
	double           tolerance     // error tolerance in conjugate gradient
	);

EXPORT_C_GRIDPRICER int G_FourierStepND (
	double           deltaT,       // total time step
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
    DT_VECTOR 		 *Betas,	   // mean reversions, now required to be zero
	DT_VECTOR        *fv,          // incoming and outgoing option value
	double           ccRd          // discount rate during this period
	);

EXPORT_C_GRIDPRICER int G_FourierStepC (
	double           deltaT,       // total time step
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
    DT_VECTOR 		 *Betas,	   // mean reversions, now required to be zero
	DT_VECTOR        *fv,          // incoming and outgoing option value
	double           ccRd          // discount rate during this period
	);

EXPORT_C_GRIDPRICER int G_FourierStepCC (
	double           deltaT,       // total time step
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
    DT_VECTOR 		 *Betas,	   // mean reversions which are used only for grid reduction purposes
	DT_VECTOR        *fv1,         // first layer
	DT_VECTOR        *fv2,         // second layer
	double           ccRd          // discount rate during this period
	);

EXPORT_C_GRIDPRICER int G_FourierStepCJ (
	double           deltaT,       // total time step
	int              D,            // number of dimensions
	Q_GRID_DOMAIN    *FactorSpace, // grid dimension
	DT_MATRIX        *CovMatrix,   // term covariance matrix
	DT_VECTOR        *Drifts,      // term drifts
	DT_MATRIX        *JumpCov,     // jump covariance matrix
	DT_VECTOR        *JumpMean,    // means of jumps
	double           lambda,       // expected number of jumps in interval
    DT_VECTOR 		 *Betas,	   // mean reversions which are used only for grid reduction purposes
	DT_VECTOR        *fv,          // incoming and outgoing option value
	double           ccRd          // discount rate during this period
	);

EXPORT_C_GRIDPRICER int G_NormalDiscount (                                       
	int            D,             // # Factors                                                    
	double         deltaT,        // Finite time interval to diffuse                              
	int            TimeStep,      // Index for this interval                                      
	DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval                     
	DT_VECTOR      *Drifts,       // dim D drift vector for this interval                         
	DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval                
	Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors                             
	DT_VECTOR      *LayerGrid,    // Domain specification for layer                               
	DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
	DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);                                                              

EXPORT_C_GRIDPRICER int G_NoDiscount (                                       
	int            D,             // # Factors                                                    
	double         deltaT,        // Finite time interval to diffuse                              
	int            TimeStep,      // Index for this interval                                      
	DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval                     
	DT_VECTOR      *Drifts,       // dim D drift vector for this interval                         
	DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval                
	Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors                             
	DT_VECTOR      *LayerGrid,    // Domain specification for layer                               
	DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
	DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);                                                              

EXPORT_C_GRIDPRICER int G_AndersenDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion    
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_DualAndersenDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval, used for jumps here
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_ADIDiffuse(
	int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Number of diffusion timesteps for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_SparseDiffuseNoBeta(
	int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this interval
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpec,   // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_HestonDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_HestonExplicit(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int G_HigginsDiffuse(
    int            D,             // # Factors
    double         deltaT,        // Finite time interval to diffuse
    int            TimeStep,      // Index for this time step
    DT_MATRIX      *CovMatrix,    // D x D covarince matrix for this interval
    DT_VECTOR      *Drifts,       // dim D drift vector for this interval
    DT_VECTOR      *Betas,        // dim D mean reversion vector for this interval
    Q_GRID_DOMAIN  *FactorSpace,  // Domain specification for factors
    DT_VECTOR      *LayerGrid,    // Domain specification for layer
    DT_VALUE       *GridSpecs,    // Structure containing information needed for general diffusion
    DT_MATRIX      *Values        // X-Layered grid values -- will be overwritten by this function
	);

EXPORT_C_GRIDPRICER int RescaleGrid(
	Q_GRID_DOMAIN *OldSpec,
	Q_GRID_DOMAIN *NewSpec,
	int D,
	double *Grid,
	double *NewGrid
);

EXPORT_C_GRIDPRICER int G_RescaleGrid( 
	Q_GRID_DOMAIN      *OldSpec, 		 // specifications for larger grid
	Q_GRID_DOMAIN      *FactorSpec,      // specifications for smaller grid
	int                D,                // dimension
	double             *Values,			 // vector of values
	double             *bf               // pointer to new values
	);

EXPORT_C_GRIDPRICER int G_HestonRescale(
	Q_GRID_DOMAIN      *OldSpec, 		 // specifications for larger grid
	Q_GRID_DOMAIN      *FactorSpec,      // specifications for smaller grid
	int                D,                // dimension
	double             *Values,			 // vector of values
	double             *bf               // pointer to new values, pre-allocated
	);

EXPORT_C_GRIDPRICER int G_ExplicitTimeSteps (
    int              D,             // # Factors
	Q_GRID_DOMAIN    *FactorSpec,   // Domain specification for factors
    DT_MATRIX        *cov           // D x D covarince matrix for this interval
    );

void pval(
    DT_MATRIX *m,
    Q_GRID_DOMAIN *fd,
    int D
);

void parr(
    double *m,
    Q_GRID_DOMAIN *fd,
    int D
);

void pm(
    DT_MATRIX *m
);

#endif
