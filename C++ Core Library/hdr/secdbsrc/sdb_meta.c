#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_meta.c,v 1.7 2013/08/23 16:55:31 khodod Exp $"
/****************************************************************
**
**	sdb_meta.c	- Metadatabase functions
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_meta.c,v $
**	Revision 1.7  2013/08/23 16:55:31  khodod
**	Squashing gcc warnings.
**	AWR: #310154
**
**	Revision 1.6  2001/03/15 10:35:53  goodfj
**	Placate solaris compiler
**
**	Revision 1.5  2001/03/13 18:18:47  goodfj
**	Path arg to SecDbLoadValueTypeRegistryTable
**	
**	Revision 1.4  2001/03/13 17:39:02  goodfj
**	Added SecDbLoadValueTypeRegistry
**	
**	Revision 1.3  2001/03/09 17:47:45  goodfj
**	MetaDb attach/detach functions moved here from secmeta
**	
**	Revision 1.2  2001/03/09 16:50:20  goodfj
**	Added SecDbValueTypeCheck
**	
**	Revision 1.1  2001/03/09 14:38:39  goodfj
**	Initial revision
**	
****************************************************************/

#define		BUILDING_SECDB

#include	<portable.h>
#include	<datatype.h>
#include	<sdb_meta.h>


const char *SecDbMetaDbVTListContainer = "MetaData ValueType List";


/****************************************************************
**	Routine: SecDbAttachMetaDatabase
**	Returns: Database 		- Pointer to database
**			 NULL  			- Error, couldn't attach to database
**	Action : Open and attach to a SecDb MetaDatabase
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbAttachMetaDatabase"

SDB_DB *SecDbAttachMetaDatabase(
	int		ReadOnly
)
{
	const char
			*MetadataDbName;

	SDB_DB	*MetadataDb;


	MetadataDbName = SecDbConfigurationGet( "SECDB_METADATA", NULL, NULL, NULL );

	if( NULL == MetadataDbName )
		return (SDB_DB *) ErrMoreN( ARGCHECK_FUNCTION_NAME ": Cannot get SECDB_METADATA from secdb.cfg." );

	MetadataDb = SecDbAttach( MetadataDbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_BACK | (ReadOnly ? SDB_READONLY : 0 ) );

	if( !MetadataDb )
		ErrMore( ARGCHECK_FUNCTION_NAME ": Cannot attach to MetaDatabase %s", MetadataDbName );

	return MetadataDb;
}


/****************************************************************
**	Routine: SecDbDetachMetaDatabase
**	Returns: TRUE/Err
**	Action : Detach from a MetaDatabase
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbDetachMetaDatabase"

int SecDbDetachMetaDatabase(
	SDB_DB	*Db
)
{
	return SecDbDetach( Db );
}


/****************************************************************
**	Routine: SecDbLoadValueTypeRegistryTable
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbLoadValueTypeRegistryTable"

int SecDbLoadValueTypeRegistryTable(
	const char *Path
)
{
#if 0
#define MAX_TOKENS	10
	char    *Tokens[ MAX_TOKENS+1 ];

	FILE	*fpTable;

	char	Buffer[ 256 ],
			FileName[ FILENAME_MAX + 1 ],
			*DatFileName = "valuetypes.dat",
			*DataType,
			*ValueType;

	int		Found;

	static int
			TableLoaded = FALSE;

/*
**	Don't start twice
*/

	if( TableLoaded )
		return TRUE;

/*
**	Open the ASCII file containing the table
*/

	if( !KASearchPath( Path, DatFileName, FileName, sizeof( FileName )))
		return ErrMore( ARGCHECK_FUNCTION_NAME ": %s not found", DatFileName );

	if( !(fpTable = fopen( FileName, "r" )))
		return Err( ERR_FILE_NOT_FOUND, ARGCHECK_FUNCTION_NAME "( %s ) %s", FileName, strerror( errno ) );

	SecDbValueTypeDataTypeHash = HashCreate( "ValueType Datatypes", (HASH_FUNC) HashStrHash, (HASH_CMP) HashStricmp, 20000, NULL );

/*
**	Read and parse the file
*/

	while( fgets( Buffer, sizeof( Buffer ), fpTable ))
	{
		Found = StrTokenize( Buffer, MAX_TOKENS, Tokens );
		if( Found == 0 )
			continue;	// Blank line or only comments

		if( 2 != Found )
		{
			fclose( fpTable );
			HashDestroy( SecDbValueTypeDataTypeHash );
			return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": Expected 2 tokens in %s, found %d at '%s'", DatFileName, Found, Buffer );
		}

		ValueType	= Tokens[ 0 ];
		DataType	= Tokens[ 1 ];

		// Add the vt
		if( !HashInsert( SecDbValueTypeDataTypeHash, (HASH_KEY) ValueType, DataType ))
		{
			fclose( fpTable );
			HashDestroy( SecDbValueTypeDataTypeHash );
			return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to insert to hash" );
		}
	}

/*
**	Close the file
*/

	fclose( fpTable );


	TableLoaded = TRUE;
#endif
	return TRUE;
}

#if 0
/****************************************************************
**	Routine: SecDbValueTypeRegisteredDataTypeName
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeRegisteredDataTypeName"

static char *SecDbValueTypeRegisteredDataTypeName(
	const char *ValueName
)
{
	
}
#endif

/****************************************************************
**	Routine: SecDbValueTypeCheck
**	Returns: 
**	Action : 
****************************************************************/

#undef ARGCHECK_FUNCTION_NAME
#define ARGCHECK_FUNCTION_NAME "SecDbValueTypeCheck"

int	SecDbValueTypeCheck(
	const char		*ValueName,
	DT_DATA_TYPE	DataType
)
{
#if 0
	static int
			DisableValueTypeCheck = !stricmp( "true", SecDbConfigurationGet( "SECDB_DISABLE_VALUETYPE_CHECK", NULL, NULL, "" ) );

	char	*RegisteredDatatypeName;


	if( DisableValueTypeCheck )
		return TRUE;

	if( !ValueName )
		return Err( ERR_INVALID_ARGUMENTS, ARGCHECK_FUNCTION_NAME ": ValueName is NULL" );

	if( !RegisteredDatatypeName = SecDbValueTypeRegisteredDataTypeName( ValueName ) )
		return ErrMore( ARGCHECK_FUNCTION_NAME ": Failed to get registered datatype for %s", ValueName );
#endif

	return TRUE;
}

