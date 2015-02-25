/* $Header: /home/cvs/src/gridpricer/src/q_grid.h,v 1.3 2001/11/27 22:34:00 procmon Exp $ */
/***************************************************************
**
**	q_grid.h	Generic grid pricing engine
**
**	Copyright 1999 - Goldman Sachs & Company - New York
**
**	$Revision: 1.3 $
**	
****************************************************************/

#if !defined( IN_Q_GRID_H )
#define IN_Q_GRID_H

#if !defined( IN_Q_PROP_H )
#include <q_prop.h>
#endif

#if !defined( IN_SLANGFNC_H )
#include <slangfnc.h>
#endif

#if !defined( IN_GRIDPRICER_BASE_H )
#include <gridpricer_base.h>
#endif

typedef int (*Q_GRID_FWD_FUNC)(
    double         price,
    DT_VECTOR     *weight,
    DT_MATRIX     *TermCov,
    Q_GRID_DOMAIN *GridSpecs,
    int            D,                   
    double        *future           
);


typedef int (*Q_GRID_EXER_FUNC)(
    Q_GRID_DOMAIN *FactorSpace, 
    DT_VECTOR     *LayerGrid, 
    int            NumFactors, 
    int            TimeStep, 
    DT_VALUE      *DealInfo,
    Q_GRID_FWD_FUNC FwdFuncPtr,
    DT_MATRIX     *Values 
);


typedef int (*Q_GRID_DIFF_FUNC)(
    int            D,             
    double         deltaT,        
    int            TimeStep,      
    DT_MATRIX     *CovMatrix,    
    DT_VECTOR     *Drifts,       
    DT_VECTOR     *Betas,        
    Q_GRID_DOMAIN *FactorSpace,  
    DT_VECTOR     *LayerGrid,    
    DT_VALUE      *GridSpecs,    
    DT_MATRIX     *Values
);

typedef int (*Q_GRID_DISC_FUNC)(
    int            D,             
    double         deltaT,        
    int            TimeStep,      
    DT_MATRIX     *CovMatrix,    
    DT_VECTOR     *Drifts,       
    DT_VECTOR     *Betas,        
    Q_GRID_DOMAIN *FactorSpace,  
    DT_VECTOR     *LayerGrid,    
    DT_VALUE      *GridSpecs,    
    DT_MATRIX     *Values
);


#define GetAndCheckDouble( info, var, name ) \
    if( (var = DtComponentGetNumber( info, name, DtDouble, HUGE_VAL )) == HUGE_VAL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );

#define GetAndCheckInt( info, var, name ) \
    if( (var = (int)DtComponentGetNumber( info, name, DtDouble, HUGE_VAL )) == HUGE_VAL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );

#define GetAndCheckVector( info, var, name ) \
    if( (var = (DT_VECTOR *)DtComponentGetPointer( info, name, DtVector, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name ); 
		
#define GetAndCheckVectorAndSize( info, var, name, sz ) \
    if( (var = (DT_VECTOR *)DtComponentGetPointer( info, name, DtVector, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name ); \
	else if( (int)var->Size != (int)sz )	\
        return( Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #var " has incorrect size of %d (should be %d)", (int)var->Size, sz ) );	

#define GetAndCheckArray( info, var, name ) \
    if( (var = (DT_ARRAY *)DtComponentGetPointer( info, name, DtArray, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );
		
#define GetAndCheckArrayAndSize( info, var, name, sz ) \
    if( (var = (DT_ARRAY *)DtComponentGetPointer( info, name, DtArray, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name ); \
	else if( (int)var->Size != (int)sz ) \
        return( Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": @ - " #var " has incorrect size of %d (should be %d)", var->Size, sz ) );	
		
#define GetAndCheckString( info, var, name ) \
    if( (var = (char *)DtComponentGetPointer( info, name, DtString, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );

#define GetAndCheckMatrix( info, var, name ) \
    if( (var = (DT_MATRIX *)DtComponentGetPointer( info, name, DtMatrix, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );

#define GetAndCheckHash( info, var, name ) \
    if( (var = (HASH *)DtComponentGetPointer( info, name, DtStructure, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );

#define GetAndCheckCurve( info, var, name ) \
    if( (var = (DT_CURVE *)DtComponentGetPointer( info, name, DtCurve, NULL )) == NULL ) \
        return Err( ERR_INVALID_ARGUMENTS,  ARGCHECK_FUNCTION_NAME ": %s", name );


EXPORT_C_GRIDPRICER int Q_GridPricer(
    DT_VALUE        *GridSpecs,        // Structure containing information needed for general diffusion
    DT_VALUE        *DealInfo,         // Structure containing information needed by exercise function
    SLANG_APP_FUNC  *AppFunc,          // Slang exercise function
    Q_GRID_DIFF_FUNC DiffuseFuncPtr,   // Arbitrary diffusion function
    Q_GRID_EXER_FUNC ExerFuncPtr,      // Arbitrary exercise function
    Q_GRID_FWD_FUNC  FwdFuncPtr,       // Arbitrary forward function
    Q_GRID_DISC_FUNC DiscFuncPtr,      // Arbitrary discount function
    DT_VALUE        *FinalRetVal       // To be filled in by this function (DataType is already specified)
);

EXPORT_C_GRIDPRICER int Q_GridDiffuse(
	DT_MATRIX			  *Values,			 // Values to diffuse
	int					  ThisDex,			 // Index of the propagation period
    DT_VALUE              *GridSpecs,        // Structure containing information needed for general diffusion
    Q_GRID_DIFF_FUNC  	  DiffuseFuncPtr,    // Arbitrary diffusion function
    DT_VALUE              *FinalRetVal       // To be filled in by this function (DataType is already specified)
);

EXPORT_C_GRIDPRICER int isValidCovMatrix(
    DT_MATRIX *Cov              // Covariance DT_MATIRX to validate
);

EXPORT_C_GRIDPRICER int isValidGridSpec(
    Q_GRID_DOMAIN *FactorSpace,    // GRID_DOMAINS to validate
    int            D               // Number of domains in array
);

EXPORT_C_GRIDPRICER Q_GRID_DOMAIN *GetFactorDomain(
    DT_VALUE *FactorSpecsValue,    // Factor Specs Structure
    int       N                    // Number of Dimensions
);

#endif
