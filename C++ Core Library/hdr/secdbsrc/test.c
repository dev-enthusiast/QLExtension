#define GSCVSID "$Header: /home/cvs/src/secdb/src/test.c,v 1.9 2001/04/16 18:01:45 simpsk Exp $"
/****************************************************************
**
**	TEST.C		- Test of the Security Database
**
**	$Log: test.c,v $
**	Revision 1.9  2001/04/16 18:01:45  simpsk
**	eliminate "void main"
**
**	Revision 1.8  1999/09/01 15:29:37  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.7  1999/08/07 01:40:17  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.6  1998/01/02 21:33:28  rubenb
**	added GSCVSID
**	
**	Revision 1.5  1997/12/19 01:58:11  gribbg
**	Change SecDbError to Err and SDB_ERR to ERR
**	
**	Revision 1.4  1994/02/16 18:53:46  GRIBBG
**	Use INCL_TIMEZONE
**	
**	
**	   Rev 1.4   16 Feb 1994 13:53:46   GRIBBG
**	Use INCL_TIMEZONE
**	
**	   Rev 1.3   14 Feb 1994 20:17:10   GRIBBG
**	Use TIMEZONE_SETUP()
**	
**	   Rev 1.2   13 Apr 1993 15:56:10   DUBNOM
**	Minor modifications to enhance portability
**	
**	   Rev 1.1   18 Jan 1993 19:18:26   DUBNOM
**	Changed datatype names from SecDb... to Dt...
**	
**	   Rev 1.0   23 Nov 1992 20:16:00   DUBNOM
**	Initial revision.
**
****************************************************************/

#define BUILDING_SECDB
#define INCL_TIMEZONE
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<time.h>
#include	<date.h>
#include	<secdb.h>
#include	<of_test.h>
#include	<of_cur.h>
#include	<of_cross.h>


/*
**	Declare some test data
*/

TEST_SECURITY_DATA
		TestData[] =
		{ 
			{ "IBM", "DEC", "AAPL", 123.45, 999.99 },
			{ "USD", "DEM", "SEK", 444.44, 987.65 },
			{ "DKK/CHF", "NOK", "JPY", 111.22, 333.44 }
		};


/*
**	Prototype functions
*/

int	main(		void );


/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Test of Security Database
****************************************************************/

int main( void )
{
	SDB_DB_ID
			SdbPublic;

	SDB_OBJECT
			*SecPtr,
			*NewSecPtr;

	SDB_SEC_TYPE
			SecTypeTest,
			SecTypeCross,
			SecTypeCurrency,
			SecTypeKnockout;

	SDB_VALUE_TYPE
			ValueTypePrice,
			ValueTypeQuantity;

	SDB_VALUE_PARAMS
			ValueParamsPrice,
			ValueParamsQuant;

	double	Value;


	TIMEZONE_SETUP();


/*
**	Attach to the test security database
*/

	if( -1 != (SdbPublic = SecDbStartup( NULL, SDB_DEFAULT_DEADPOOL_SIZE, NULL )))
		printf( "Attached to database...\n" );
	else
	{
		printf( "Couldn't attach to security master database\n" );
		return 1;
	}


/*
**	Retrieve security types that we're interested in
*/

	SecTypeTest		= SecDbClassTypeFromName( "Test of a security object" );
	SecTypeCross	= SecDbClassTypeFromName( "Currency Cross" );
	SecTypeCurrency	= SecDbClassTypeFromName( "Currency" );
	SecTypeKnockout	= SecDbClassTypeFromName( "Knockout" );


/*
**	Get value types that we're interested in
*/

	ValueTypeQuantity	= SecDbValueTypeFromName( "Quantity",	DtDouble );
	ValueTypePrice		= SecDbValueTypeFromName( "Price",		DtDouble );


/*
**	Create test securities
*/

	// Security number 1
	SecPtr = SecDbNew(
			"Test Security 1", 		// Security name
			SecTypeTest, 			// Security type
			&TestData[ 0 ] );		// Security specific data

	if( SecDbAdd( SecPtr, SdbPublic, NULL ))
		printf( "Created Test Security 1\n" );
	else
		printf( "Error, couldn't create security\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


	// Security number 2
	SecPtr = SecDbNew(
			"Test Security 2", 		// Security name
			SecTypeTest, 			// Security type
			&TestData[ 1 ] );		// Security specific data

	if( SecDbAdd( SecPtr, SdbPublic, NULL ))
		printf( "Created Test Security 2\n" );
	else
		printf( "Error, couldn't create security\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


	// Security number 3
	SecPtr = SecDbNew(
			"Test Security 3", 		// Security name
			SecTypeTest, 			// Security type
			&TestData[ 2 ] );		// Security specific data

	if( SecDbAdd( SecPtr, SdbPublic, NULL ))
		printf( "Created Test Security 3\n" );
	else
		printf( "Error, couldn't create security\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


/*
**	Setup valuation parameters
*/

	ValueParamsPrice.Type	= ValueTypePrice;
	ValueParamsPrice.Arg1	= 0.0;
	ValueParamsPrice.Arg2	= 0.0;

	ValueParamsQuant.Type	= ValueTypeQuantity;
	ValueParamsQuant.Arg1	= 0.0;
	ValueParamsQuant.Arg2	= 0.0;


/*
**	Get a security
*/

	if( SecPtr = SecDbGetByName( "Test Security 3", SdbPublic ))
	{
		printf( "Security loaded!!!\n" );
		printf( "Dump o' data: %s %s %s %lf %lf\n",
				((TEST_SECURITY_DATA *)(SecPtr->Mem))->Underlyer1,
				((TEST_SECURITY_DATA *)(SecPtr->Mem))->Underlyer2,
				((TEST_SECURITY_DATA *)(SecPtr->Mem))->Underlyer3,
				((TEST_SECURITY_DATA *)(SecPtr->Mem))->BogusValue1,
				((TEST_SECURITY_DATA *)(SecPtr->Mem))->BogusValue2 );
		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsQuant, &Value, NULL ))
			printf( "Couldn't retrieve quantity value\n" );
		else
			printf( "Quantity: %lf\n", Value );
		SecDbFree( SecPtr );
	}
	else
		printf( "Couldn't load Test Security 2\nERROR TEXT: %s\n", ErrGetString() );


/*
**	Get a security
*/

	if( SecPtr = SecDbGetByName( "DKK/CHF", SdbPublic ))
	{
		printf( "Security loaded!!!\n" );
		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsQuant, &Value, NULL ))
			printf( "Couldn't retrieve quantity value\n" );
		else
			printf( "Quantity: %lf\n", Value );
	}
	else
		printf( "Couldn't load DKK/CHF\nERROR TEXT: %s\n", ErrGetString() );


/*
**	Get the same security again
*/

	if( NewSecPtr = SecDbGetByName( "DKK/CHF", SdbPublic ))
	{
		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsQuant, &Value, NULL ))
			printf( "Couldn't retrieve quantity value\n" );
		else
			printf( "Quantity: %lf\n", Value );
	}
	else
		printf( "Couldn't load DKK/CHF\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );
	SecDbFree( NewSecPtr );


/*
**	Try to get a security that doesn't exist
*/

	if( SecPtr = SecDbGetByName( "INVALID SECURITY", SecDbRootDb, 0 ))
		printf( "ERROR!!! Found invalid security\n" );
	else
		printf( "Invalid security wasn't found (that is good).\nERROR TEXT: %s\n", ErrGetString() );


/*
**	Try to get a security by inference
*/

	if( SecPtr = SecDbGetByInference( "NOK/JPY", SdbPublic, SecTypeCross, NULL ))
	{
		printf( "NOK/JPY Security loaded!!!\n" );
		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsPrice, &Value, NULL ))
			printf( "Couldn't calculate value\n" );
		else
			printf( "Calculated value: %lf\n", Value );

		if( !SecDbGetValue( SecPtr, &ValueParamsQuant, &Value, NULL ))
			printf( "Couldn't retrieve quantity value\n" );
		else
			printf( "Quantity: %lf\n", Value );
	}
	else
		printf( "Couldn't load NOK/JPY\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


/*
**	Try to get a security by inference
*/

	if( SecPtr = SecDbGetByInference( "JPY", SdbPublic, SecTypeCurrency, NULL ))
		printf( "JPY Security loaded!!!\n" );
	else
		printf( "Couldn't load JPY\nERROR TEXT: %s\n", ErrGetString() );

	SecDbFree( SecPtr );


/*
**	Delete one of the securities
*/

	if( SecDbDeleteByName( "Test Security 2", SdbPublic, FALSE, NULL ))
		printf( "Security deleted without error\n" );
	else
		printf( "Couldn't delete test security 2\n" );


/*
**	Test the NextName functions
*/

	{
		char	SecName[ SDB_SEC_NAME_SIZE ];


		printf( "Dump of securities by name:\n" );
		strcpy( SecName, "" );
		while( SecDbNameLookup( SecName, 0, SDB_GET_GREATER, SdbPublic ))
			printf( "%s\n", SecName );
	}
	return 0;
}
