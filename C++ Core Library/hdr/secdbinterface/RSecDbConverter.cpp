#define GSCVSID "$Header: /home/cvs/src/rglue/src/RSecDbConverter.cpp,v 1.5 2012/07/27 13:12:34 khodod Exp $"
/***********************************************************//**
** @file RSecDbConverter.cpp
**
** Utility class to convert between R and SecDB DT_VALUES.
** Based on mtglue.
**
** @author John Scillieri
** 
****************************************************************
**
** $Log: RSecDbConverter.cpp,v $
** Revision 1.5  2012/07/27 13:12:34  khodod
** Fix conversion to SecDb Null by performing the check as the very first
** thing we do in the generic converter. isNumeric succeeds for R NULL!
** AWR: #274292
**
**
***************************************************************/

#include "RSecDbConverter.h"

const char *DEFAULT_APP_NAME = "Rsession";

static Gs::SecDbInterface *secDb = NULL;

#ifndef __DEBUG
#define R_log (void)
#else
#define R_log Rprintf
#endif


/******************************************************************************/
/******************** BEGIN SECDB TO R CONVERSION FUNCTIONS *******************/
/******************************************************************************/

/*******************************************************************************
* Convert the DT_VALUE passed in to a comparable R Type.
* 
* Warning: Not all types are implemented, you WILL get a warning though if you
* encounter an unsupported type.
*/
SEXP RSecDbConverter::convertDataTypeToR( DT_VALUE *value)
{
    DT_DATA_TYPE dataType = value->DataType;
    int id = dataType->ID;
    
    SEXP returnValue = R_NilValue;
    
    if (id == DtString->ID)
    {
        // Character strings
        returnValue = RSecDbConverter::convertStringToRString(value);
    }
    else if (id == DtArray->ID)
    {
        // Arrays of anything
        returnValue = RSecDbConverter::convertArrayToRList(value);
    }
    else if (id == DtCurve->ID)
    {
        // Curves (int rates: fwd pnts...)
        //returnValue = RSecDbConverter::convertCurveToRList(value);    
        returnValue = RSecDbConverter::convertCurveToRDataFrame(value);
    }
    else if (id == DtDate->ID)
    {
        // Dates
        returnValue = RSecDbConverter::convertDateToRNumeric(value);     
    }
    else if (id == DtDouble->ID)
    {
        // Doubles (recast longs: etc...)
        returnValue = RSecDbConverter::convertDoubleToRNumeric(value); 
    }
    else if (id == DtGCurve->ID)
    {
        // Curves (int rates: fwd pnts...) 
        returnValue = RSecDbConverter::convertGCurveToRDataFrame(value);
    }
    else if (id == DtMatrix->ID)
    {
        returnValue = RSecDbConverter::convertMatrixToRMatrix(value);
    }
    else if (id == DtNull->ID)
    {
          // NULL (undefined value)
        returnValue = R_NilValue;  
    }
    else if (id == DtSecurity->ID)
    {
        // Nested security type
        returnValue = RSecDbConverter::convertSecurityToRString(value);
    }
    else if (id ==  DtStructure->ID)
    {
        // Structures of anything
        returnValue = RSecDbConverter::convertStructureToRList(value);    
    }
    else if (id ==  DtTCurve->ID)
    {     
        // Time Curves
        returnValue = RSecDbConverter::convertCurveToRDataFrame(value); 
    }
    else if (id == DtTime->ID)
    {
         // Times 
        returnValue = RSecDbConverter::convertTimeToRTime(value);
    }
    else if (id ==  DtVector->ID)
    {     
        // Vector of reals
        returnValue = RSecDbConverter::convertVectorToRVector(value); 
    }
    else
    { 
        Rprintf("Error: Unsupported Conversion Type - %s, ID=%d\n", dataType->Name, id);
        returnValue = R_NilValue; 
    }
    
    return(returnValue);
    
    /* TO BE IMPLEMENTED !!!!
    case DtAny->ID:                    //   Flag to allow any datatype (similar to Null)
    case DtBinary->ID:                //   Binary data
    case DtCMatrix->ID:                //   Matrix of complex
    case DtCVector->ID:                //   Vector of complex
    case DtComplex->ID:                //   Complex number
    case DtError->ID:                //   Just an error (similar to DtNull)
    case DtFloat->ID:                //   Alternate double representation
    case DtGRCurve->ID:                  //   Curves (int rates: fwd pnts...)
    case DtGStructure->ID:              //   Structure ( keys are arbitrary DtValues instead of just strings )
    case DtMathOperation->ID:        //   Math operation [[not a real datatype]]     
    case DtPointerArray->ID:            //     Array of pointers to DT_VALUEs
    case DtRCurve->ID:                  //   Relative Curves (int rates: fwd pnts...)
    case DtRDate->ID:                //     Relative Dates
    case DtReference->ID:            //   Reference to another DT_VALUE
    case DtRTime->ID:                //     Relative Times
    case DtStructureCase->ID:        //   Case sensitive structures of anything
    case DtSymbolCase->ID:            //   Case sensitive symbols
    */
}


/*******************************************************************************
* Convert a variable type SecDb array and convert it to an R list.
* We use R Lists because arrays can be variable type in SecDb and an R array of 
* one type wouldn't be sufficient.
*/
SEXP RSecDbConverter::convertArrayToRList(DT_VALUE *value)
{

    int protectCounter = 0;
    
    const DT_ARRAY
            *arrayPtr = (const DT_ARRAY *) value->Data.Pointer;

    if( !arrayPtr )
    {
        Rprintf("Error: DT_VALUE marked as DtArray has NULL pointer\n");
        return(R_NilValue);
    }


    unsigned int nRows = arrayPtr->Size, nCols = 1;


    //create the actual list structure to return to R
    const int FIELD_COUNT = nRows;
    SEXP listToReturn = allocVector(VECSXP, FIELD_COUNT);
    PROTECT( listToReturn ); protectCounter++;

    
    for( int i = 0; i < nRows; i++ )
    {
        try
        {
            // attaching components to the list:
            SEXP value = RSecDbConverter::convertDataTypeToR( &(arrayPtr->Element[i]) );
            PROTECT( value ); protectCounter++;
            SET_VECTOR_ELT(listToReturn, i, value);
        }
        catch (...)
        {
            Rprintf("Something bad happened, we're in the ConvertArrayToRList catch block.\n");
            throw;
        }
    }

    UNPROTECT(protectCounter);
    return(listToReturn);
}


/*******************************************************************************
* Extract a date from a SecDb value and convert it to an R numeric in seconds
* from 1970.
*/
SEXP RSecDbConverter::convertDateToRNumeric(DT_VALUE *value)
{
     SEXP myDouble = NEW_NUMERIC(1);
     double *doublePtr = NUMERIC_POINTER(myDouble);
     doublePtr[0] = DATE_TO_TIME(value->Data.Number);
     return myDouble;
}


/*******************************************************************************
* Extract a double from a SecDb value and convert it to an R numeric
*/
SEXP RSecDbConverter::convertDoubleToRNumeric(DT_VALUE *value)
{
     SEXP myDouble = NEW_NUMERIC(1);
     double *doublePtr = NUMERIC_POINTER(myDouble);
     doublePtr[0] = value->Data.Number;
     return myDouble;
}


/*******************************************************************************
* Extract the curve data from a SecDb curve DT_VALUE Pointer and return an
* R list structure.
*/
SEXP RSecDbConverter::convertCurveToRList(DT_VALUE *value)
{
    int protectCounter = 0;
    
    //convert the curve data pointer to the appropriate DT structure 
    const DT_CURVE* curveData = reinterpret_cast<const DT_CURVE *>
        (value->Data.Pointer);
    
    if (!curveData)
    {        
        
        error("Curve Data Pointer Is Null.\n");
        UNPROTECT(protectCounter);
        return(R_NilValue);
    }

    //create the name list to attach later
    const int NAME_COUNT = 3;
    char *names[NAME_COUNT] = {"isTCurve", "date", "value"};
    SEXP listNames = allocVector( STRSXP, NAME_COUNT);  
    PROTECT( listNames ); protectCounter++; 
    for(int nameCounter = 0; nameCounter < NAME_COUNT; nameCounter++)
    {
        SET_STRING_ELT( listNames, nameCounter, 
            mkChar( names[nameCounter] )); 
    }
    
    int knotCount = curveData->KnotCount;
    
    //create the isTCurve boolean list element
    SEXP isTCurve = NEW_LOGICAL(1);
    PROTECT( isTCurve ); protectCounter++;
    int *isTCurvePtr = LOGICAL_POINTER(isTCurve);
    
    //create the dateList double list element (stored as seconds)
    SEXP dateList = NEW_NUMERIC(knotCount);
    PROTECT( dateList ); protectCounter++;
    double *dates = NUMERIC_POINTER(dateList);
    
    //create the curve value list element
    SEXP valueList = NEW_NUMERIC(knotCount);
    PROTECT( valueList ); protectCounter++;
    double *values = NUMERIC_POINTER(valueList);
    
    //set the isTCurve boolean to whether we're a TCurve or not
    isTCurvePtr[0] = ( value->DataType == DtTCurve );
    
    //Iterate through our dates and values and set them in our list elements
    DT_CURVE_KNOT* knotBuffer = curveData->Knots;
    if ( isTCurvePtr[0] )
    {
        for (int i = 0; i < knotCount; i++)
        {                    
            dates[i] = knotBuffer[i].Date;
            values[i] = knotBuffer[i].Value;
        }
    }
    else
    {
        for (int i = 0; i < knotCount; i++)
        {                    
            dates[i] = DATE_TO_TIME(knotBuffer[i].Date);
            values[i] = knotBuffer[i].Value;
        }
    } 
    
    
    // Creating a list with X vector elements:
    SEXP listToReturn = allocVector(VECSXP, NAME_COUNT);
    PROTECT( listToReturn ); protectCounter++;
    
    // attaching components to the list:
    SET_VECTOR_ELT(listToReturn, 0, isTCurve);
    SET_VECTOR_ELT(listToReturn, 1, dateList); 
    SET_VECTOR_ELT(listToReturn, 2, valueList); 
    
    // and attach the names to the list:
    setAttrib(listToReturn, R_NamesSymbol, listNames); 
    
    UNPROTECT(protectCounter);
    return(listToReturn);
    
}


/*******************************************************************************
* Extract the curve data from a SecDb curve DT_VALUE Pointer and return an
* R data frame structure.
*/
SEXP RSecDbConverter::convertCurveToRDataFrame(DT_VALUE *value)
{
    int protectCounter = 0;
    
    //convert the curve data pointer to the appropriate DT structure 
    const DT_CURVE* curveData = reinterpret_cast<const DT_CURVE *>
        (value->Data.Pointer);
    
    if (!curveData)
    {        
        
        error("Curve Data Pointer Is Null.\n");
        UNPROTECT(protectCounter);
        return(R_NilValue);
    }
    
    
    //a TCurve has time data, a regular curve can be returned as R Dates
    bool isTCurve = ( value->DataType == DtTCurve );
    
    vector<string> columnNames ;
    if (isTCurve) 
    {
        columnNames.push_back("time");
    }
    else
    {
        columnNames.push_back("date");
    }
    columnNames.push_back("value");
    
    RcppFrame returnValue(columnNames);
    int knotCount = curveData->KnotCount;
    DT_CURVE_KNOT* knotBuffer = curveData->Knots;
    
    if( knotCount==0 || knotBuffer == NULL )
    {
        R_log("ERROR: Curve Data Is Empty.\n");
        UNPROTECT(protectCounter);
        return(R_NilValue);
    }
    

    //Iterate through our dates and values and set them in our list elements
    
    if ( isTCurve )
    {
        for (int i = 0; i < knotCount; i++)
        {    
            double timeValue = knotBuffer[i].Date;
            ColDatum timeColumn ;
            timeColumn.setTimeValue(timeValue);
            
            
            double dblValue = knotBuffer[i].Value;
            ColDatum valueColumn ;
            valueColumn.setDoubleValue(dblValue);
            
            
            vector<ColDatum> rowData;
            rowData.push_back(timeColumn);
            rowData.push_back(valueColumn);
            
            
            returnValue.addRow(rowData);
        }
    }
    else
    {
        for (int i = 0; i < knotCount; i++)
        {                    
            
            DATE dateValue = knotBuffer[i].Date;
            int month=0, day=0, year=0 ;
            DateToMdy(dateValue, &month, &day, &year );

            RcppDate rDateValue( month, day, year );
            ColDatum dateColumn ;
            dateColumn.setDateValue(rDateValue);
            
            
            double dblValue = knotBuffer[i].Value;
            ColDatum valueColumn ;
            valueColumn.setDoubleValue(dblValue);
            
            
            vector<ColDatum> rowData;
            rowData.push_back(dateColumn);
            rowData.push_back(valueColumn);
            
            
            returnValue.addRow(rowData);
        }
    } 
    

    UNPROTECT(protectCounter);
    return(returnValue.getReturnValue());
    
}


/*******************************************************************************
* Extract the curve data from a SecDb GCurve DT_VALUE Pointer and return an
* R data frame structure.
*/
SEXP RSecDbConverter::convertGCurveToRDataFrame(DT_VALUE *value)
{
    int protectCounter = 0;
    
    //convert the curve data pointer to the appropriate DT structure 
    const DT_GCURVE* curveData = reinterpret_cast<const DT_GCURVE *>
        (value->Data.Pointer);
    
    if (!curveData)
    {        
        
        error("GCurve Data Pointer Is Null.\n");
        UNPROTECT(protectCounter);
        return(R_NilValue);
    }
    
    vector<string> columnNames ;
    columnNames.push_back("date");
    columnNames.push_back("value");
    
    RcppFrame returnValue(columnNames);

    int knotCount = curveData->KnotCount;
    DT_GCURVE_KNOT *knotBuffer = curveData->Knots;

    //Iterate through our Structure Date/Value pairings and insert them
    for (int i = 0; i < knotCount; i++)
    {                    
        
        DATE dateValue = knotBuffer[i].Date;
        int month=0, day=0, year=0 ;
        DateToMdy(dateValue, &month, &day, &year );
        
        RcppDate rDateValue( month, day, year );
        ColDatum dateColumn ;
        dateColumn.setDateValue(rDateValue);
        
        
        DT_VALUE valueArray = knotBuffer[i].Value;
        const DT_ARRAY *arrayPtr = reinterpret_cast<const DT_ARRAY *>(valueArray.Data.Pointer);
        DT_VALUE *knotValue = arrayPtr->Element;
        ColDatum valueColumn ;
        valueColumn.setDoubleValue(knotValue->Data.Number);
        
        
        vector<ColDatum> rowData;
        rowData.push_back(dateColumn);
        rowData.push_back(valueColumn);
        
        
        returnValue.addRow(rowData);
    }

    
    UNPROTECT(protectCounter);
    return(returnValue.getReturnValue());
    
}





/*******************************************************************************
* 
*/
SEXP RSecDbConverter::convertMatrixToRMatrix(DT_VALUE *value)
{
    int numProtected = 0;
    
    const DT_MATRIX *Mat = (const DT_MATRIX *) value->Data.Pointer;

    //Do your safety checks to make sure the matrix pointer is properly formed.
    if( !Mat )
    {
        error("Matrix Pointer Is Null.\n");
        UNPROTECT(numProtected);
        return(R_NilValue);
    }
    
    if ( !Mat->NumRows || !Mat->NumCols )
    {
        error("Matrix Has Invalid Dimensions.\n");
        UNPROTECT(numProtected);
        return(R_NilValue);
    }
    
    
    
    SEXP rReturnValue = PROTECT( allocMatrix( REALSXP, Mat->NumRows, Mat->NumCols ) );
    numProtected++;

    for( int r = 0; r < Mat->NumRows; r++ )
    {
        for( int c = 0; c < Mat->NumCols; c++ )
        {
            REAL(rReturnValue)[r + Mat->NumRows*c] = DT_MATRIX_ELEM( Mat, r, c );
        }
    }

    UNPROTECT( numProtected );
    return( rReturnValue );

}



/*******************************************************************************
* We do this because the securities encountered by this function will most
* likely be  ones nested in another security.  As we don't want to infinitely 
* recurse parsing nested securities, we'll just return the name and if the user
* wants it, they look it up individually.
*/
SEXP RSecDbConverter::convertSecurityToRString(DT_VALUE *value)
{
    DT_VALUE stringValue;
    stringValue.DataType = DtString;
    DTM_TO( &stringValue, value );
    return RSecDbConverter::convertStringToRString(&stringValue);
}


/*******************************************************************************
* Extract a string from a SecDb value and convert it to an R char array
*/
SEXP RSecDbConverter::convertStringToRString(DT_VALUE *value)
{
     SEXP myChar = allocVector(STRSXP, 1);
     SET_STRING_ELT(myChar, 0, mkChar((char*)value->Data.Pointer));
     
     return myChar;
}


/*******************************************************************************
* Take a SecDb structure of variable type and convert it and its elements. The
* returned values are stored in an R list that is later returned.
*/
SEXP RSecDbConverter::convertStructureToRList(DT_VALUE *value)
{
    int protectCounter = 0;

    DT_VALUE ElemTag, ElemValue;

    int numFields = 0;
    DTM_FOR_VALUE( &ElemTag, &ElemValue, const_cast<DT_VALUE*>(value) )
    {
        ++numFields;
    }
    
    //create the actual list structure to return to R
    const int FIELD_COUNT = numFields;

    SEXP listToReturn = allocVector(VECSXP, FIELD_COUNT);
    PROTECT( listToReturn ); protectCounter++;
    
    //create the name list 
    SEXP listNames = allocVector( STRSXP, FIELD_COUNT);
    PROTECT( listNames ); protectCounter++;     
    // and attach the names to the list:
    setAttrib(listToReturn, R_NamesSymbol, listNames); 


    int i =0;
    DTM_FOR_VALUE( &ElemTag, &ElemValue, value )
    {

        try
        {
            if( ElemTag.DataType != DtString )
            {
                Rprintf( "Warning: Element of structure has a non-string tag." );
            }

            //Store the name of the element in the names list
            const char * fldName = (const char *)ElemTag.Data.Pointer;
            SET_STRING_ELT( listNames, i, mkChar( fldName ) ); 
            //Rprintf("About to convert structure element: %s\n", fldName);
    
            // attaching components to the list:
            SEXP value = RSecDbConverter::convertDataTypeToR( &ElemValue );
            PROTECT( value ); protectCounter++;
            SET_VECTOR_ELT(listToReturn, i, value);
        }
        catch (...)
        {
            Rprintf("Something bad happened, we're in the convertStructureToRList catch block.\n");
            DTM_FOR_VALUE_BREAK( &ElemTag, &ElemValue, const_cast<DT_VALUE*>(value) );
            throw;
        }

        i++;

    }


    UNPROTECT(protectCounter);
    return(listToReturn);
}


/*******************************************************************************
* Convert a SecDb Time to an R Time
*/
SEXP RSecDbConverter::convertTimeToRTime(DT_VALUE *value)
{
    SEXP timeValue = NEW_NUMERIC(1);
    double *doublePtr = NUMERIC_POINTER(timeValue);
    doublePtr[0] = value->Data.Number ;

    SEXP timeclass = allocVector(STRSXP,1);
    SET_STRING_ELT(timeclass, 0, mkChar("POSIXct"));
    setAttrib(timeValue, R_ClassSymbol, timeclass);
    
    return(timeValue);
}


/*******************************************************************************
* Convert a SecDb Vector to an R Vector
*/
SEXP RSecDbConverter::convertVectorToRVector(DT_VALUE *value)
{
    DT_VECTOR *Vec = (DT_VECTOR *) value->Data.Pointer;

    SEXP doubleVector;
    double *doublePtr;
    PROTECT(doubleVector = NEW_NUMERIC(Vec->Size));
    doublePtr = NUMERIC_POINTER(doubleVector);

    int i = 0;
    DT_VALUE elementName, elementValue;
    DTM_FOR( &elementName, value )
    {
        DtSubscriptGetCopy( value, &elementName, &elementValue );
        doublePtr[i] = elementValue.Data.Number;
        DTM_FREE( &elementValue );
        i++;
    }


    UNPROTECT(1);
    return doubleVector;
}



/******************************************************************************/
/******************** BEGIN R TO SECDB CONVERSION FUNCTIONS *******************/
/******************************************************************************/

DT_VALUE RSecDbConverter::convertRToDataType( SEXP element )
{
    // Initialize the return value to null in case it doesn't get assigned.
    DT_VALUE dtVal;
    dtVal.DataType = DtNull;
    dtVal.Data.Number = 0;

    if( isNull( element ) )
        return dtVal;

    SEXP classname = getAttrib(element, R_ClassSymbol);
    const char *argumentType = NULL; 
    if( classname != R_NilValue )
    {
        argumentType = CHAR(STRING_ELT(classname,0)) ;
        R_log( "RSecDbConverter: Read an argument of class(%s)\n", argumentType );
    }

    //
    // This needs to go first because a class listed here may pass one of the
    // checks below incorrectly (e.g., Date class passes the isNumeric check).
    //

    if( argumentType != NULL )
    {
        if( strcmp("Date", argumentType) == 0 )
        {
            dtVal = RSecDbConverter::convertRDate(element);
        }
        else if( strcmp("POSIXct", argumentType) == 0 )
        {
            dtVal = RSecDbConverter::convertRTime(element);
        }
        else
        {
            Rprintf( "Unsupported conversion type: \"%s\". Please consider changing your data type or breaking it apart into its individual components.\n",
                argumentType );
        }
    }
    else if( isString(element) )
    {
        dtVal = RSecDbConverter::convertRString(element);            
    }
    else if( isNumeric(element) )
    {
        dtVal = RSecDbConverter::convertRNumeric(element);
    }
    else if( isNewList(element) )
    {
        dtVal = RSecDbConverter::convertRList(element);
    }
    else
    {
        Rprintf( "Unsupported conversion type. Please consider changing your data type or breaking it apart into its individual components.\n");
    }

    return(dtVal);
}


/*******************************************************************************
* Convert an R Character Array to a SecDb String
*/
DT_VALUE RSecDbConverter::convertRString(SEXP charValue, int index)
{      
    
    DT_VALUE argument;
    argument.DataType = DtString;
    readCharacterValue( (char**)(&argument.Data.Pointer), charValue, index);
    //Rprintf("Read a character value: %s\n", (char*)argument.Data.Pointer);
    return argument;
}


/*******************************************************************************
* Convert an R Date String to a SecDb Time Value
*/
DT_VALUE RSecDbConverter::convertRTime(SEXP timeValue)
{

    DT_VALUE dtVal ;
    dtVal.DataType = DtTime;

    timeValue = AS_INTEGER(timeValue);
    dtVal.Data.Number = INTEGER_POINTER(timeValue)[0];
    //Rprintf("Read a time value: %f\n", dtVal.Data.Number);
    return dtVal;
}

/*******************************************************************************
* Convert an R Date String to a SecDb Time Value
*/
DT_VALUE RSecDbConverter::convertRDate(SEXP dateValue)
{
    /*
    DT_VALUE argument;
    argument.DataType = DtTime;        
    long timeVal ;
    DateFromString( CHAR(STRING_ELT(dateValue,0)) , &timeVal );
    argument.Data.Number = DATE_TO_TIME(timeVal);
    return argument;*/

    //Create the return argument
    DT_VALUE dtVal ;
    dtVal.DataType = DtDate;

    //Read the date value from R and convert it to a DtDate
    int rDateVal = (int)REAL(dateValue)[0];
    RcppDate date(rDateVal);
    DATE dateVal = DateFromMdy(date.getMonth(), date.getDay(), date.getYear());
    dtVal.Data.Number = dateVal;

    //Rprintf("Read a date value: %d/%d/%d with dtVal=%d\n", 
    //    date.getMonth(), date.getDay(), date.getYear(), (int)dtVal.Data.Number);
    
    return dtVal;
}

/*******************************************************************************
* Convert an R Date String to a SecDb Time Value
*/
DT_VALUE RSecDbConverter::convertRNumeric(SEXP number, int index)
{
    DT_VALUE dtVal;
    dtVal.DataType = DtDouble;
    number = AS_NUMERIC(number);
    dtVal.Data.Number = NUMERIC_POINTER(number)[index];
    //Rprintf("Read a numeric value: %f\n", dtVal.Data.Number);
    return dtVal;
}

/*******************************************************************************
* Convert an R List to a SecDb Structure or Array based on if it is named or 
* not.  
*/
DT_VALUE RSecDbConverter::convertRList( SEXP listValue )
{
    //Rprintf("\nInside convertRList...\n");

    DT_VALUE dtVal; 

    
    int listLength = length(listValue);
    //Rprintf("List Length = %d\n", listLength);


    SEXP nameList = getAttrib(listValue, R_NamesSymbol);

    if(nameList == R_NilValue)
    {
        //Rprintf("Converting R List to SecDb Array...\n");

        DT_VALUE nullValue;
        nullValue.DataType = DtNull;
        nullValue.Data.Number = 0;

        dtVal = DtArrayAllocAndInit(listLength, &nullValue);

        for(int listPosition = 0; listPosition < listLength; listPosition++) 
        {
            SEXP listElement = VECTOR_ELT(listValue,listPosition);

            DT_VALUE valueToAdd = RSecDbConverter::convertRToDataType(listElement);
            
            DtSubscriptSet(&dtVal, listPosition, &valueToAdd);
        }
    }
    else
    {
        //Rprintf("Converting R List to SecDb Structure...\n");

        DTM_ALLOC( &dtVal, DtStructure );

        for(int listPosition = 0; listPosition < listLength; listPosition++) 
        {
            const char *currentName = CHAR(STRING_ELT(nameList,listPosition));
            if(currentName == NULL || strcmp(currentName, "")==0)
            {
                Rprintf("RSecDbConverter: all list elements must be named.\n");
                Rprintf("Position %d returned no name! Skipping Element...\n", listPosition);
            }
            else
            {
                //Rprintf("RSecDbConverter: Read list element of name(%s)\n", currentName );
            
                SEXP listElement = VECTOR_ELT(listValue,listPosition);

                DT_VALUE valueToAdd = RSecDbConverter::convertRToDataType(listElement);
                //DtComponentSet(&dtVal, currentName, &valueToAdd);
                DT_COMPONENT_GIVE(&dtVal, currentName, &valueToAdd);
            }
        }
    }
    
    return(dtVal);
}


/*******************************************************************************
* Convert an R Character Array to a C charater pointer
*/
void RSecDbConverter::readCharacterValue( char** destination, SEXP charValue, 
                                          int index )
{
    charValue = AS_CHARACTER(charValue);
    const char *tempPtr = CHAR( STRING_ELT(charValue, index) );
    *destination = R_alloc( strlen( tempPtr ), sizeof(char) );
    strcpy( *destination, tempPtr );
}


/*******************************************************************************
* Convert an R Integer to a C integer
*/
void RSecDbConverter::readIntegerValue( int* destination, SEXP intValue ) 
{
    *destination = INTEGER_VALUE(intValue);
}


/******************************************************************************/
/******************** BEGIN R-SECDB UTILITY FUNCTIONS *************************/
/******************************************************************************/

/*******************************************************************************
* Creates the SecDb instance if necessary and returns said instance
*  
* Eg: "Bal Prod 2", "User", "!Bal Prod 1;ReadOnly{!Bal Prod 2}"
*/
Gs::SecDbInterface* RSecDbConverter::getSecDbInterface(const char *dbString)
{        
    if (!secDb)
    {
        if (!dbString)
            dbString = "Production";

        RSecDbTracer tracer;
        Gs::SecDbInterface::setTracer(&tracer);
        Gs::SecDbInterface::initialize(DEFAULT_APP_NAME); 
        secDb = Gs::SecDbInterface::instance(dbString);
    }
    
    return secDb;
}



/*******************************************************************************
* Creates an argument array appropriate for passing to the Invoke call of a 
* SlangInvoker. It takes a variable number of DT_VALUES and returns the argument
* array.
*/
DT_VALUE RSecDbConverter::createArgumentArray(DT_VALUE *val1,...)
{
    // Setup variable argument handler
    va_list list ;

    //Initialize the argument array
    DT_VALUE argArray;
    DTM_ALLOC(&argArray, DtArray);
    
    DtAppend(&argArray, val1);

    //Start looking after the first element of the argument list
    va_start(list,val1); 

    for (int i = 1; ; i++)
    {
        DT_VALUE *nthValue = va_arg( list, DT_VALUE* );
        
        if (nthValue == NULL)
        {
            break;
        }
        else
        {
            DtAppend(&argArray, nthValue);
        }
    }
    
    // cleanup variable argument handlers 
    va_end(list);    
    
    return argArray;     
}


/*******************************************************************************
* Prints all the known elements of the DT_Value Pointer
*/
int RSecDbConverter::printElements( DT_VALUE *Value )
{
    DT_VALUE    Enum,
                ElemValue,
                StrValue;

    DTM_FOR( &Enum, Value )
    {
        StrValue.DataType = DtString;
        DTM_TO( &StrValue, &Enum );
        Rprintf( "%s:\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
        DTM_FREE( &StrValue );


        DtSubscriptGetCopy( Value, &Enum, &ElemValue );

        StrValue.DataType = DtString;

        DTM_TO( &StrValue, &ElemValue );
        Rprintf( "%s\n\n", (char *) DT_VALUE_TO_POINTER( &StrValue ));
        DTM_FREE( &StrValue );
        DTM_FREE( &ElemValue );

    }
    
    return 0;
}



/*
char slang[100] ;
sprintf(slang, "TsdbSymbolInfo(\"%s\").realtime", dt1.Data.Pointer);
Rprintf("Evaluating slang code: %s\n", slang);
DT_VALUE *symbolInfo = secDb->evalSlang(slang).get();
bool isRealtime = false;
if (symbolInfo->Data.Number == 1)
{
   isRealtime = true;
}  
*/
