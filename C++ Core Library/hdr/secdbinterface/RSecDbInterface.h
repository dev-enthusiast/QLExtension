/***********************************************************//**
**
** $Header: /home/cvs/src/rglue/src/RSecDbInterface.h,v 1.3 2012/07/27 13:13:46 khodod Exp $
**
** @file RSecDbInterface.h
**
** Simplistic R interface to SecDb.
** Based on mtglue code.
**
** @author John Scillieri
** 
***************************************************************/

#ifndef _RSECDBINTERFACE_H_INCLUDED
#define _RSECDBINTERFACE_H_INCLUDED


#include "RSecDbConverter.h"


int Rwin_fpset();


/*****************************************************************************
* These are the functions exported by the library.
* They are the functions called by the actual R code itself.
*/
extern "C"
{

__declspec(dllexport) SEXP SetSecDbDatabase( SEXP dbString ); 

__declspec(dllexport) SEXP TSDBRead(SEXP curveName, SEXP startDate, SEXP endDate) ;

__declspec(dllexport) SEXP GetSymbolInfo(SEXP symbolName) ;

__declspec(dllexport) SEXP GetValueType(SEXP securityName, SEXP valueTypeName) ;

__declspec(dllexport) SEXP EvaluateSlang(SEXP slangCode) ;

__declspec(dllexport) SEXP CallFunction( SEXP libraryName, SEXP functionName,
										 SEXP argumentList ) ;
}



/*****************************************************************************
* Utility functions for debugging the library without running through R.
*/
dt_value_var cGetValueType( char* securityName, char* valueTypeName ) ;
dt_value_var cEvaluateSlang( char* slangCode );
int cPrintElements( DT_VALUE *Value ) ;
int main() ;



/*****************************************************************************
* Utility Class
* It currently only stores the database name, but could be used in the future 
* to store and retrieve other info that may be needed by the exported 
* library functions.
*/
class RSecDbInterface
{

public:

	static void setDBName( const char *dbName ) ;
        
	static const char* getDBName() ;

	static SEXP CallFunction(char* library, char* function, DT_VALUE* argArray) ;

private:

    static const char *dbString_ ;
};

#endif
