#define GSCVSID "$Header: /home/cvs/src/secdb/src/secdoc.c,v 1.18 2001/04/16 18:01:45 simpsk Exp $"
/****************************************************************
**
**	SECDOC.C	- Program to auto-document security classes
**
**	$Log: secdoc.c,v $
**	Revision 1.18  2001/04/16 18:01:45  simpsk
**	eliminate "void main"
**
**	Revision 1.17  2000/12/19 17:20:46  simpsk
**	Remove unneeded typedefs.
**	For SW6 compilability.
**	
**	Revision 1.16  1999/11/01 21:58:59  singhki
**	oops, no BUILDING_SECDB in executables which link with secdb
**	
**	Revision 1.15  1999/10/14 08:24:53  goodfj
**	Main doesn't return anything, so declare it void
**	
**	Revision 1.14  1999/09/01 15:29:36  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
**	Revision 1.13  1999/08/07 16:05:18  singhki
**	Change SecDb APIs to use SDB_DB * instead of SDB_DB_IDs
**	
**	Revision 1.12  1998/11/16 23:00:06  singhki
**	DiddleColors: Merge from branch
**	
**	Revision 1.11.4.1  1998/11/05 01:01:26  singhki
**	TempRev: hacks to support OS/2 IBM compiler in C++ mode
**	
**	Revision 1.11  1998/05/27 06:19:32  gribbg
**	Use DateToday()
**	
**	Revision 1.10  1998/01/02 21:33:25  rubenb
**	added GSCVSID
**	
**	Revision 1.9  1997/12/19 01:58:09  gribbg
**	Change SecDbError to Err and SDB_ERR to ERR
**	
**	Revision 1.8  1997/05/23 06:58:20  gribbg
**	Const: Make sources more const-correct
**	
**	Revision 1.7  1996/01/17 23:12:58  ERICH
**	NT Compatibility
**
**	
**	   Rev 1.7   17 Jan 1996 18:12:58   ERICH
**	NT Compatibility
**
**	   Rev 1.6   07 Apr 1994 19:10:48   LUNDEK
**	Split off datatype project
**
**	   Rev 1.5   16 Feb 1994 13:53:44   GRIBBG
**	Use INCL_TIMEZONE
**
**	   Rev 1.4   14 Feb 1994 20:17:06   GRIBBG
**	Use TIMEZONE_SETUP()
**
**	   Rev 1.3   31 Aug 1993 09:28:10   LUNDEK
**	UserName, ApplicationName, and ErrorMessage
**
**	   Rev 1.2   28 Apr 1993 09:56:38   DUBNOM
**	Hash API changes
**
**	   Rev 1.1   13 Apr 1993 15:56:10   DUBNOM
**	Minor modifications to enhance portability
**
**	   Rev 1.0   23 Nov 1992 20:15:52   DUBNOM
**	Initial revision.
**
****************************************************************/

#define INCL_TIMEZONE
#include	<portable.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<math.h>
#include	<malloc.h>
#include	<date.h>
#include	<hash.h>
#include	<outform.h>
#include	<secdb.h>
#include	<secexpr.h>


/*
**	Structure
*/

struct SECDOC
{
	char	ClassName[ SDB_CLASS_NAME_SIZE ];

	SDB_OBJECT
			*SecPtr;

};


/*
**	Variables
*/

SDB_DB	*SdbPublic;


/*
**	Prototype functions
*/

void	main(						int argc, char *argv[] );

static void
		Message(				const char *Msg );


/****************************************************************
**	Routine: main
**	Returns: None
**	Action : Document security classes
****************************************************************/

int main(
	int		argc,		// Command line argument count
	char	*argv[]		// Command line argument vectors
)
{
	SDB_OBJECT
			*SecPtr;

	SDB_CLASS_INFO
			*ClassInfo;

	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	SECDOC	*SecDoc;

	HASH	*Hash;

	HASHPAIR
			*HashPairPos,
			*HashPair;

	char	SecName[ SDB_SEC_NAME_SIZE ],
			Buffer[ 128 ];


	TIMEZONE_SETUP();


/*
**	Startup SecDb
*/

	if( !(SdbPublic = SecDbStartup( NULL, SDB_DEFAULT_DEADPOOL_SIZE, Message, Message, "SecDoc", getenv( "USER_NAME" ))))
	{
		fprintf( stderr, "%s\n", ErrGetString() );
		return 1;
	}


/*
**	Put in a cover
*/

	printf( "      FUNCTION: Objects - Security Database Objects\n\n" );
	printf( "          TAIL: OBJECTS - SECURITY OBJECTS - %s - J. ARON\n\n", DateToString( Buffer, DateToday()));
	printf( "         COVER: SecDb\n\n" );
	printf( "                Security\n\n" );
	printf( "                Objects\n" );
	printf( "\n" );


/*
**	Enumerate security classes
*/

	// FIX(OPTLINK) - added explicit casts 'cause OS/2 is upset about linkage
 	Hash = HashCreate( "Securities", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 0, NULL );
	if( ClassInfo = SecDbClassEnumFirst( &EnumPtr ))
		do
		{
			*SecName = '\0';
			if( SecDbNameLookup( SecName, ClassInfo->Type, SDB_GET_GREATER, SdbPublic )
					&& (SecPtr = SecDbGetByName( SecName, SdbPublic, 0 )))
			{
				SecDoc = (SECDOC *) malloc( sizeof( SECDOC ));
				strcpy( SecDoc->ClassName, ClassInfo->Name );
				SecDoc->SecPtr = SecPtr;
				HashInsert( Hash, SecDoc->ClassName, SecDoc );
			}
		} while( ClassInfo = SecDbClassEnumNext( EnumPtr ));
	SecDbClassEnumClose( EnumPtr );


/*
**	Sort the names
*/

	HashPair = HashSortedArray( Hash, NULL );


/*
**	Print summary sheet
*/

	HashPairPos = HashPair;
	printf( "      FUNCTION: Object Summary\n\n" );
	printf( "   DESCRIPTION: Summary of objects:\n\n" );
	while( SecDoc = (SECDOC *) (HashPairPos++)->Key )
		printf( "                    %s\n", SecDoc->ClassName );
	printf( "\n" );


/*
**	Print detail for each security class
*/

	HashPairPos = HashPair;
	while( SecDoc = (SECDOC *) (HashPairPos++)->Key )
	{
		printf( "      FUNCTION: %s\n\n", SecDoc->ClassName );
		printf( "   DESCRIPTION: Value methods and their types:\n\n" );

		if( ValueTypeInfo = SecDbValueTypeEnumFirst( SecDoc->SecPtr, &EnumPtr ))
		{
			do
			{
				if( !(ValueTypeInfo->ValueFlags & SDB_HIDDEN))
					printf( "                %-*s %s\n",
							SDB_VALUE_NAME_SIZE,
							ValueTypeInfo->ValueType->Name,
							ValueTypeInfo->ValueType->DataType->Name );

			} while( ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ));
		}
		else
			printf( "                No value types were found\n" );

		SecDbValueTypeEnumClose( EnumPtr );
		SecDbFree( SecDoc->SecPtr );
		printf( "\n" );
	}
	return 0;
}



/****************************************************************
**	Routine: Message
**	Returns: None
**	Action : Display a message from SecDb on the screen
****************************************************************/

static void Message(
	const char	*Msg
)
{
	if( !Msg )
		Msg = "";

	fprintf( stderr, "%-78.78s\r", Msg );
}
