#define GSCVSID "$Header: /home/cvs/src/rglue/src/RSecDbInterface.cpp,v 1.2 2012/07/27 13:13:46 khodod Exp $"
/***********************************************************//**
** @file RSecDbInterface.cpp
**
** Simplistic R interface to SecDb.
** Based on mtglue code.
**
** @author John Scillieri
** 
****************************************************************
**
** $Log: RSecDbInterface.cpp,v $
** Revision 1.2  2012/07/27 13:13:46  khodod
** Fix up the header and add GSCVSID.
** AWR: #274292
**
**
***************************************************************/

#include "RSecDbInterface.h"
#include "Rcpp.hpp"


const char *RSecDbInterface::dbString_ ;


/*******************************************************************************
* This is used for debugging purposes.  Use it to test the SecDb library apart
* from the R calling routines
*/
int main()
{
	char *dbString = "!Bal Prod 1;ReadOnly{!Bal Prod 2}";
	RSecDbInterface::setDBName(dbString);	

	char *security = "Bal Close FrtMarks Config";
	char *valType = "Contents";

    dt_value_var returnVal = cGetValueType(security, valType);
	DT_VALUE *value = returnVal.get();
	
	cPrintElements(value);


	char *slang ="TsdbSymbolInfo(\"PRC_NG_EXCHANGE_0706\")";
	printf("Evaluating slang code: %s\n", slang);
	
	dt_value_var returnVal2 = cEvaluateSlang(slang);
	DT_VALUE *value2 = returnVal2.get();

	char *badSlang = "TsdbSymbolInfo(\"PRC_NG_EXCHANGE_0706\"";
	printf("Evaluating slang code: %s\n", badSlang);
	
	dt_value_var returnVal3 = cEvaluateSlang(badSlang);
	DT_VALUE *value3 = returnVal3.get();
	
	cPrintElements(value3);

	
	DT_VALUE prDate ;
	prDate.DataType = DtDate;
    long timeVal ;
    DateFromString( "28Jan03" , &timeVal );
    prDate.Data.Number = timeVal;
	
	DT_VALUE startDate ;
	startDate.DataType = DtTime;
	DATE startDateVal = DateFromMdy(7, 24, 2007);
	startDate.Data.Number = startDateVal;
	
	DT_VALUE endDate ;
	endDate.DataType = DtTime;
	long timeVal2 ;
    DateFromString( "01Oct07" , &timeVal2 );
    endDate.Data.Number = timeVal2;

	DT_VALUE currency ;
	currency.DataType = DtString;
    currency.Data.Pointer = "USD";

	DT_VALUE symbol;
	symbol.DataType = DtString;
	symbol.Data.Pointer = "PRC_NG_EXCHANGE";


	DT_VALUE argArray = RSecDbConverter::createArgumentArray(&symbol, &startDate, &endDate, NULL);
	SEXP returnedVal = RSecDbInterface::CallFunction("_Lib ELEC Matlab Dispatch Fns", 
		"TSDB::read",&argArray);

    /*Gs::SecDbInterface *secDb = RSecDbConverter::getSecDbInterface(dbString);
    Gs::SlangInvoker_ptr invoker = secDb->getInvoker("RWrapper");

    dt_value_var returnValue = invoker->Invoke ("RWrapper::TsdbSymbolInfo", argArray2);
    DT_VALUE *value4 = returnValue.get();*/

	//cPrintElements(value4);


/*	char *library = "_Lib MATLAB Functions";
	char *function = "MATLAB::getDatedDF";
	invoker = secDb->getInvoker(library);
    returnValue = invoker->Invoke (function, argArray);
    DT_VALUE *value5 = returnValue.get();

	//cPrintElements(value5);



	// Should NOT crash the program
	char *library = "_L MATLAB Functions";
	char *function = "MATLAB::getDated";
	invoker = secDb->getInvoker(library);
    returnValue = invoker->Invoke (function, argArray);
    DT_VALUE *value6 = returnValue.get();

	cPrintElements(value6);
*/

	char *slang2 ="DateRuleApply(Date(\"27Jul07\"), \"-1b\")";
	printf("Evaluating slang code: %s\n", slang2);
	
	dt_value_var returnVal7 = cEvaluateSlang(slang2);
	DT_VALUE *value7 = returnVal7.get();
	cPrintElements(value7);


	char *security2 = "Commod PWX 2x16 AGC Physical";
	char *valType2 = "Time Created";

    returnVal2 = cGetValueType(security2, valType2);
	value2 = returnVal2.get();
	
	cPrintElements(value2);

	
	// TEST FOR GCURVE FUNCTIONALITY
	printf("\n\nRunning GCURVE TEST:\n");
	DT_VALUE asOfDate ;
	asOfDate.DataType = DtDate;
    DateFromString( "09Jan08" , &timeVal );
    asOfDate.Data.Number = timeVal;

	DT_VALUE portfolioBook ;
	portfolioBook.DataType = DtString;
    portfolioBook.Data.Pointer = "NYTRADE1";

	DT_VALUE percentage;
	percentage.DataType = DtDouble;
	percentage.Data.Number = 1;

	DT_VALUE portArray;
	DTM_ALLOC(&portArray, DtArray);
	DtAppend(&portArray, &portfolioBook);

	DT_VALUE percentageArray;
	DTM_ALLOC(&percentageArray, DtArray);
	DtAppend(&percentageArray, &percentage);



	DT_VALUE rdArgArray = RSecDbConverter::createArgumentArray(&portArray, &asOfDate, &percentageArray, NULL);
	printf("\nArgument array created:\n");	

	Gs::SecDbInterface *secDb = RSecDbConverter::getSecDbInterface(dbString);
    Gs::SlangInvoker_ptr invoker = secDb->getInvoker("_lib elec deltagamma report");
	printf("\nInvoker created. Running ReadDeltas:\n");	

    dt_value_var returnValue = invoker->Invoke ("DeltaGamma::ReadDeltas", rdArgArray);
    DT_VALUE *value8 = returnValue.get();	
	
	cPrintElements(value8);

	returnedVal = RSecDbInterface::CallFunction("_lib elec deltagamma report", 
		"DeltaGamma::ReadDeltas",&rdArgArray);

	printf("Done.\n");
    return 0;   
}


/*******************************************************************************
* Read the curve information from tsdb.
*   curveName - the curve name ( as a character array from R )
*   startDate - the earliest date from which to retrieve curve information 
*               ( as a date string of the format "%d%b%y" )
*   endDate - the last date from which to retrive curve data ( same as startDate )
*/
SEXP TSDBRead(SEXP curveName, SEXP startDate, SEXP endDate)
{
    
	DT_VALUE dt1 = RSecDbConverter::convertRString(curveName);
	DT_VALUE dt2 = RSecDbConverter::convertRTime(startDate);
	DT_VALUE dt3 = RSecDbConverter::convertRTime(endDate);
	DT_VALUE argArray = 
             RSecDbConverter::createArgumentArray(&dt1, &dt2, &dt3, NULL);
	
	SEXP value = RSecDbInterface::CallFunction("_Lib ELEC Matlab Dispatch Fns", 
		"TSDB::read",&argArray);
	
	return value;

}  


/*******************************************************************************
* 
*/
SEXP GetSymbolInfo(SEXP symbolName)
{
	DT_VALUE dt1 = RSecDbConverter::convertRString(symbolName);	
	DT_VALUE argArray = RSecDbConverter::createArgumentArray(&dt1, NULL);
	SEXP value = RSecDbInterface::CallFunction("RWrapper", 
		"RWrapper::TsdbSymbolInfo",&argArray);
	return value;
}


/*******************************************************************************
* 
*/
SEXP GetValueType(SEXP securityName, SEXP valueTypeName)
{
	int protectCounter = 0;
	DT_VALUE securityValue = RSecDbConverter::convertRString(securityName);
    char *name = (char*)(securityValue.Data.Pointer); 
    DT_VALUE valueTypeValue = RSecDbConverter::convertRString(valueTypeName);
    char *value = (char*)(valueTypeValue.Data.Pointer); 
    
    //Rprintf("Read security: %s and value: %s\n", name, value);
    
    dt_value_var returnVal = cGetValueType(name, value);
	DT_VALUE *dtValueType = returnVal.get();
    
    SEXP valueType = RSecDbConverter::convertDataTypeToR(dtValueType);
    
	UNPROTECT(protectCounter);
    return(valueType);
}


/*******************************************************************************
* 
*/
dt_value_var cGetValueType( char* securityName, char* valueTypeName )
{
	Gs::SecDbInterface *secDb = RSecDbConverter::getSecDbInterface(RSecDbInterface::getDBName());	
	
	dt_value_var returnVal = secDb->getValueType( securityName, valueTypeName );
	return returnVal;

}


/*******************************************************************************
* 
*/
SEXP EvaluateSlang(SEXP slangCode)
{
	int protectCounter = 0;
	char *slangPtr;
	PROTECT(slangCode = AS_CHARACTER(slangCode)); protectCounter++;
	slangPtr = R_alloc( strlen( CHAR( STRING_ELT(slangCode, 0) ) ), sizeof(char) );
	strcpy( slangPtr, CHAR( STRING_ELT(slangCode, 0) ) );
    
    //Rprintf("Read slang code string: %s\n", slangPtr);
    
    dt_value_var returnVal = cEvaluateSlang(slangPtr);
	DT_VALUE *dtValueType = returnVal.get();
    
    SEXP valueType = RSecDbConverter::convertDataTypeToR(dtValueType);

    UNPROTECT(protectCounter);
    return(valueType);
}

/*******************************************************************************
* 
*/
dt_value_var cEvaluateSlang( char* slangCode )
{
	Gs::SecDbInterface *secDb = 
		RSecDbConverter::getSecDbInterface(RSecDbInterface::getDBName());
	if (secDb)
	{
		dt_value_var returnVal = secDb->evalSlang(slangCode);
		return returnVal;
	}
	else
	{
		printf("Invalid SecDb Interface. Unable To Evaluate Slang Code.\n");
		dt_value_var emptyValue;
		return emptyValue;
	}

}


/*******************************************************************************
* 
*/
SEXP CallFunction( SEXP libraryName, SEXP functionName, SEXP argumentList )
{

	char *libraryPtr;
	RSecDbConverter::readCharacterValue( &libraryPtr, libraryName );
	
	char *functionPtr;
	RSecDbConverter::readCharacterValue( &functionPtr, functionName );

	int argListLength = length(argumentList);

	//Rprintf("Read an argument list length = %d\n", argListLength);

	std::vector<DT_VALUE> arguments;
	for (int argIndex = 0; argIndex < argListLength; argIndex++)
	{

		SEXP element = VECTOR_ELT(argumentList, argIndex);

		DT_VALUE dtVal = RSecDbConverter::convertRToDataType(element);
		arguments.push_back(dtVal);
	}


	DT_VALUE sdbArgArray;
	DTM_ALLOC(&sdbArgArray, DtArray);
	for (int sdbArgIndex = 0 ; sdbArgIndex < arguments.size() ; sdbArgIndex++)
	{
		DtAppend(&sdbArgArray, &arguments[sdbArgIndex]);
	}

	SEXP returnValue = RSecDbInterface::CallFunction(libraryPtr, functionPtr, 
		&sdbArgArray);


    return(returnValue); 
}



/*******************************************************************************
* Iterates through the elements of a DT_VALUE pointer and prints out all 
* name->value pairings
*/
int cPrintElements( DT_VALUE *Value )
{
	//Variables to hold the name->value pairing as well as 
	//the converted string output.
	DT_VALUE Enum, ElemValue, StrValue;

	DTM_FOR( &Enum, Value )
	{
		StrValue.DataType = DtString;
		DTM_TO( &StrValue, &Enum );
		printf( "%s:\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
		DTM_FREE( &StrValue );

		DtSubscriptGetCopy( Value, &Enum, &ElemValue );
		StrValue.DataType = DtString;
		DTM_TO( &StrValue, &ElemValue );
		printf( "%s\n\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
		DTM_FREE( &StrValue );
		DTM_FREE( &ElemValue );

	}
	
	return 0;
}


/*******************************************************************************
* Set the SecDb database name for the rest of the session.
*   dbString - a R character array to be used as the database name
*/
SEXP SetSecDbDatabase( SEXP dbString )
{
    int protectCounter = 0;
    PROTECT(dbString = AS_CHARACTER(dbString)); protectCounter++;
	
	RSecDbInterface::setDBName( CHAR(STRING_ELT(dbString,0)) );

	UNPROTECT(1);
	return(R_NilValue);
}


/******************************************************************************/
const char* RSecDbInterface::getDBName()
{
	return dbString_;
}


/******************************************************************************/
void RSecDbInterface::setDBName(const char *dbName)
{
	dbString_ = dbName;
}

/*******************************************************************************
* 
*/
SEXP RSecDbInterface::CallFunction(char* library, char* function, DT_VALUE* argArray)
{
	const char *dbName = RSecDbInterface::getDBName(); 
    Gs::SecDbInterface *secDb = RSecDbConverter::getSecDbInterface(dbName);

    Gs::SlangInvoker_ptr invoker = secDb->getInvoker(library);
	
	if (invoker.get())
	{
      dt_value_var returnValue = invoker->Invoke (function, *argArray);
      DT_VALUE *value = returnValue.get();
	  if (value)
      {
         SEXP rValue = RSecDbConverter::convertDataTypeToR(value);
         return(rValue);
      }
      else
      {
         Rprintf("Error calling function: %s\n", function);
         return(R_NilValue);
      }
    }
    else
    {
      Rprintf("Couldn't call library due to null slang invoker.\n");
      return(R_NilValue); 
    }   
}


