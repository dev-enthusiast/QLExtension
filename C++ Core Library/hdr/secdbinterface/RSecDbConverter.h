/***********************************************************//**
**
** $Header: /home/cvs/src/rglue/src/RSecDbConverter.h,v 1.2 2012/07/27 13:13:46 khodod Exp $
**
** @file RSecDbConverter.h
**
** Utility class to convert between R and SecDB DT_VALUES.
** Based on mtglue.
**
** @author John Scillieri
** 
***************************************************************/

#ifndef _RSECDBCONVERTER_H_INCLUDED
#define _RSECDBCONVERTER_H_INCLUDED

#include "sdbInterface.h"
#include <R.h>
#include <Rdefines.h>
#include "Rcpp.hpp"
#include <date.h>

#include <iostream>
#include <string>
#include <sstream>

class RSecDbTracer;

/*****************************************************************************
*
*/
class RSecDbConverter
{
public:
    
    // SecDb to R Conversion Functions
    static SEXP convertDataTypeToR( DT_VALUE *value ) ;
	static SEXP convertArrayToRList( DT_VALUE *value ) ;
    static SEXP convertDateToRNumeric( DT_VALUE *value ) ;
    static SEXP convertDoubleToRNumeric( DT_VALUE *value ) ;
    static SEXP convertCurveToRList( DT_VALUE *curveData ) ;
	static SEXP convertCurveToRDataFrame( DT_VALUE *curveData );
	static SEXP convertGCurveToRDataFrame( DT_VALUE *curveData );
	static SEXP convertMatrixToRMatrix( DT_VALUE *value ) ;
	static SEXP convertSecurityToRString( DT_VALUE *value ) ;
    static SEXP convertStringToRString( DT_VALUE *value ) ;
    static SEXP convertStructureToRList( DT_VALUE *value ) ;
	static SEXP convertTimeToRTime(DT_VALUE *value);
	static SEXP convertVectorToRVector( DT_VALUE *value ) ;
    
    // R to SecDb Conversion Functions
    static DT_VALUE convertRToDataType( SEXP element );
	static DT_VALUE convertRString( SEXP charValue, int index=0 ) ;
    static DT_VALUE convertRTime( SEXP timeValue ) ; 
	static DT_VALUE convertRDate( SEXP dateValue ) ;
	static DT_VALUE convertRNumeric( SEXP number, int index=0) ;
	static DT_VALUE convertRList( SEXP listValue );
	static void readCharacterValue( char** destination, SEXP charValue, int index=0) ;
	static void readIntegerValue( int* destination, SEXP intValue ) ;

    // Utility Functions
	static Gs::SecDbInterface* getSecDbInterface(const char *dbString) ; 
    static DT_VALUE createArgumentArray( DT_VALUE *val1, ... ) ;
    static int printElements( DT_VALUE *Value ) ;
    
    
private:
        
    /***************************************************************************
    * The tracer object that displays all SecDb Output
	*/
    class RSecDbTracer : public Gs::SecDbTracer
    {
        public:
			virtual void vtrace(const char *msg, va_list argp) const
			{
				char *message = (char*)msg;
#ifdef _DEBUG
				printf("SecDb Output: ");
            	vprintf( message, argp);
#else
				Rprintf("SecDb Output: ");
				Rvprintf( message, argp );
#endif
			}
			
			virtual void trace (const char * msg) const
            {
                char *message = (char*)msg;
#ifdef _DEBUG
            	printf( "SecDb Output: %s\n", message );
#else
				Rprintf( "SecDb Output: %s\n", message );
#endif
            } 
			
		private:

    };
    
    
};


#endif
