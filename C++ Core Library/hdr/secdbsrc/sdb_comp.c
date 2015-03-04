#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_comp.c,v 1.20 2001/11/27 23:23:35 procmon Exp $"
/****************************************************************
**
**	SDB_COMP.C	- Security database object compare function
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Log: sdb_comp.c,v $
**	Revision 1.20  2001/11/27 23:23:35  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.19.1.1  2001/11/05 16:20:13  singhki
**	removed 1.18 for prodver
**	
**	Revision 1.19  2001/09/19 15:10:51  nauntm
**	added IgnoredSavedSecurities option for duplicate
**	
**	Revision 1.17  2001/08/03 17:20:33  nauntm
**	IsEqual now handles security lists
**	
**	Revision 1.16  2001/08/03 16:22:52  nauntm
**	fix logic error in SecDbIsEqual, made code readable
**	
**	Revision 1.15  2001/06/29 15:15:29  nauntm
**	changed SkipSecurities to an int
**	fixed functions being called with wrong arg count
**	SkipSecurities also applies to SecLists.
**	some code prettified
**	
**	Revision 1.14  2001/06/25 23:03:15  tomasje
**	Option to skip security vts in cache duplicate function.
**	
**	Revision 1.13  2001/03/07 20:51:13  nauntm
**	moved (and enhanced) SecDbDuplicateCache from trade project
**	
**	Revision 1.12  2001/03/07 17:45:31  nauntm
**	SecDbDuplicate now dups security graphs.
**	
**	Revision 1.11  2001/03/05 18:23:00  singhki
**	recurse into sub value types which are securities during SecDbIsEqual
**	
**	Revision 1.10  2000/06/07 14:05:28  goodfj
**	Fix includes
**	
**	Revision 1.9  1999/09/01 15:29:27  singhki
**	Use BUILDING_SECDB/EXPORT_CLASS macros to export new C++ child apis
**	
****************************************************************/

#define BUILDING_SECDB
#include	<portable.h>
#include	<secdb.h>
#include	<err.h>
#include    <hash.h>

typedef int (*DupFuncPtr)( SDB_OBJECT*,	SDB_OBJECT *, unsigned int, unsigned int, int, int );

static HASH
		*SecRefHash=NULL;


/****************************************************************
**	Routine: SecDbSecListIsEqual
**	Returns: 
**	Summary: 
****************************************************************/

static int SecDbSecListIsEqual(
	DT_VALUE        *SL1,
	DT_VALUE        *SL2,
	SDB_VALUE_FLAGS	ValueFlags
)
{
	int     I,
			Sz;
	DT_VALUE 
			IAsDT;

	IAsDT.DataType = DtDouble;

	Sz = DtSize( SL1 );
	if ( DtSize( SL2 ) != Sz )
		return FALSE;

	for( I=0; I<Sz; I++ )
	{
		DT_VALUE 
				Elem1, 
				Elem2;

		IAsDT.Data.Number = I;
		if ( !DtSubscriptGetCopy( SL1, &IAsDT, &Elem1 ))
			return FALSE;
		if ( !DtSubscriptGetCopy( SL2, &IAsDT, &Elem2 ))
		{
			DTM_FREE( &Elem1 );
			return FALSE;
		}
		int ret = SecDbIsEqual( (SDB_OBJECT*)Elem1.Data.Pointer, (SDB_OBJECT*)Elem2.Data.Pointer, ValueFlags );
		DTM_FREE( &Elem1 );
		DTM_FREE( &Elem2 );
		if ( !ret )
			return FALSE;
	}

	return TRUE;
}

/****************************************************************
**	Routine: SecDbIsEqual
**	Returns: TRUE	- All compared values are the same
**			 FALSE	- At least one value was not the same
**	Summary: Compare two securities
****************************************************************/

int SecDbIsEqual(
	SDB_OBJECT		*SecPtr1,		// First object to compare
	SDB_OBJECT		*SecPtr2,		// Second object to compare
	SDB_VALUE_FLAGS	ValueFlags		// Types of values to compare
)
{
	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	DT_VALUE
			Value0,
			*Value1,
			*Value2;

	int		RetValue = TRUE;


	if( SecPtr1 == SecPtr2 )
		return TRUE;

	if( !SecPtr1 || !SecPtr2 )
		return FALSE;

	if( SecPtr1->SecData.Type != SecPtr2->SecData.Type )
		return FALSE;

	for( ValueTypeInfo = SecDbValueTypeEnumFirst( SecPtr1, &EnumPtr ); ValueTypeInfo; ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
		if( ValueTypeInfo->ValueFlags & ValueFlags )
		{
			Value1 = SecDbGetValue( SecPtr1, ValueTypeInfo->ValueType );
			Value2 = SecDbGetValue( SecPtr2, ValueTypeInfo->ValueType );
			if( Value1==NULL || Value2==NULL )
				RetValue = Value1 == Value2;
			else if ( Value1->DataType == DtSecurity && Value2->DataType == DtSecurity )
				RetValue = SecDbIsEqual( (SDB_OBJECT*) Value1->Data.Pointer, (SDB_OBJECT*) Value2->Data.Pointer, ValueFlags );
			else if ( Value1->DataType == DtSecurityList && Value2->DataType == DtSecurityList )
				RetValue = SecDbSecListIsEqual( Value1, Value2, ValueFlags );
			else 
			{
				if ( DT_OP( DT_MSG_EQUAL, &Value0, Value1, Value2 ))
					RetValue = Value0.Data.Number;
			}
			if ( RetValue == FALSE )
				break;
		}
	SecDbValueTypeEnumClose( EnumPtr );

	return RetValue;
}



/****************************************************************
**	Routine: SecDbDuplicateDtSecurity
**	Returns: TRUE	- All values duplicated without error
**			 FALSE	- At least one value failed to copy
**	Summary: Copy a security into a new security
****************************************************************/

static int SecDbDuplicateDtSecurity( 
	DT_VALUE        *Dst,
	DT_VALUE        *Src,
	SDB_VALUE_FLAGS	ValueFlags,		// Types of values to duplicate
	SDB_SET_FLAGS	SetFlags,		// Flags to send to for SecDbSetValue
	int             SkipSecurities,
	int             IgnoreSavedSecurities,
	DupFuncPtr      DupFunc

)
{
	SDB_OBJECT
			*TgtPtr,
			*SecPtr = (SDB_OBJECT*) Src->Data.Pointer,
			*HashValuePtr;

	Dst->DataType = DtSecurity;
	if ( SecPtr == NULL )
	{
		Dst->Data.Pointer = NULL;
		return TRUE;
	}

	if( IgnoreSavedSecurities && !SecDbIsNew( (SDB_OBJECT*) Src->Data.Pointer )) 
	{
		Dst->Data.Pointer = Src->Data.Pointer;
		SecDbIncrementReference( (SDB_OBJECT*) Dst->Data.Pointer );
		return TRUE;
	}

	HashValuePtr = (SDB_OBJECT*) HashLookup( SecRefHash, (HASH_KEY)SecPtr );
	if ( HashValuePtr != NULL )
	{
		Dst->Data.Pointer = SecDbIncrementReference( (SDB_OBJECT *) HashValuePtr );
		return TRUE;
	}

	TgtPtr = SecDbNewByClass( NULL, SecPtr->Class, SecPtr->Db );

	HashInsert(	SecRefHash, (HASH_KEY) SecPtr, (HASH_VALUE) TgtPtr );
	
	if ( TgtPtr == NULL )
		return FALSE;

	if ( (*DupFunc)( TgtPtr, SecPtr, ValueFlags, SetFlags, SkipSecurities, IgnoreSavedSecurities ))
	{
		Dst->Data.Pointer = TgtPtr;
		return TRUE;
	}

	SecDbFree( TgtPtr );
	return FALSE;
}



/****************************************************************
**	Routine: SecDbDuplicateDtSecurityList
**	Returns: TRUE	- All values duplicated without error
**			 FALSE	- At least one value failed to copy
**	Summary: Copy a security into a new security
****************************************************************/

static int SecDbDuplicateDtSecurityList( 
	DT_VALUE        *Dst,
	DT_VALUE        *Src,
	SDB_VALUE_FLAGS	ValueFlags,		// Types of values to duplicate
	SDB_SET_FLAGS	SetFlags,		// Flags to send to for SecDbSetValue
	int             SkipSecurities,
	int             IgnoreSavedSecurities,
	DupFuncPtr      DupFunc
)
{
	int     I,
			Sz,
			Ret = TRUE;
	DT_VALUE 
			IAsDT;

	IAsDT.DataType = DtDouble;

	DTM_ALLOC( Dst, DtSecurityList );

	Sz = DtSize( Src );

	for( I=0; I<Sz; I++ )
	{
		DT_VALUE 
				NewSec,
				Elem;

		IAsDT.Data.Number = I;
		Ret = Ret && DtSubscriptGetCopy( Src, &IAsDT, &Elem );

		Ret = Ret && SecDbDuplicateDtSecurity( &NewSec, 
											   &Elem, 
											   ValueFlags, 
											   SetFlags, 
											   SkipSecurities, 
											   IgnoreSavedSecurities, 
											   DupFunc );
		DT_SUBSCRIPT_GIVE( Dst, I, &NewSec );
		DTM_FREE( &Elem );
	}

	if ( !Ret )
		DTM_FREE( Dst );
	return Ret;
}




/****************************************************************
**	Routine: SecDbDuplicate
**	Returns: TRUE	- All values duplicated without error
**			 FALSE	- At least one value failed to copy
**	Summary: Copy a security into a new security
**           Securities/security lists will only be deepcopied
**           if they are instreams (a KIS idea, he getting far
**           too conservative.)
**           This routine will correctly copy graphs of 
**           securities.
****************************************************************/

int SecDbDuplicate(
	SDB_OBJECT		*TgtSecPtr,		// Target security pointer
	SDB_OBJECT		*SrcSecPtr,		// Source security pointer
	SDB_VALUE_FLAGS	ValueFlags,		// Types of values to duplicate
	SDB_SET_FLAGS	SetFlags,		// Flags to send to for SecDbSetValue
	int             SkipSecurities,
	int             IgnoreSavedSecurities
)
{
	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	DT_VALUE
			*ValueResults,
			ValueCopy;

	int		RetValue = TRUE,
			CreateHash = SecRefHash==NULL;

	if ( CreateHash )
	{
		SecRefHash = HashCreate( "SecRefDupHash", NULL, NULL, 0, NULL );
		HashInsert(	SecRefHash, (HASH_KEY) SrcSecPtr, (HASH_VALUE) TgtSecPtr );
	}

	for( ValueTypeInfo = SecDbValueTypeEnumFirst( SrcSecPtr, &EnumPtr ); RetValue && ValueTypeInfo; ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
		if( ValueTypeInfo->ValueFlags & ValueFlags )
		{
			ValueResults = SecDbGetValue( SrcSecPtr, ValueTypeInfo->ValueType );

			RetValue = ValueResults != NULL;

			if ( RetValue )
			{
				if ( ValueResults->DataType == DtSecurity && ValueTypeInfo->ValueFlags & SDB_IN_STREAM )
				{
					if( SkipSecurities ) 
						continue;
					RetValue = SecDbDuplicateDtSecurity( &ValueCopy, 
														 ValueResults, 
														 ValueFlags, 
														 SetFlags, 
														 SkipSecurities, 
														 IgnoreSavedSecurities,
														 (DupFuncPtr) SecDbDuplicate );
				}
				else if ( ValueResults->DataType == DtSecurityList && ValueTypeInfo->ValueFlags & SDB_IN_STREAM )
				{
					if( SkipSecurities ) 
						continue;
					RetValue = SecDbDuplicateDtSecurityList( &ValueCopy, 
															 ValueResults, 
															 ValueFlags, 
															 SetFlags, 
															 SkipSecurities, 
															 IgnoreSavedSecurities,
															 (DupFuncPtr) SecDbDuplicate );
				}
				else
					RetValue = RetValue && DTM_ASSIGN( &ValueCopy, ValueResults );
			}

			if( RetValue )
			{
				if( !SecDbSetValue( TgtSecPtr, ValueTypeInfo->ValueType, &ValueCopy, SetFlags | SDB_CACHE_NO_COPY ))
				{
					DTM_FREE( &ValueCopy );
					RetValue = FALSE;
				}
			}
		}
	SecDbValueTypeEnumClose( EnumPtr );

	if ( CreateHash )
	{
		HashDestroy( SecRefHash );
		SecRefHash = NULL;
	}

	if( !RetValue )
		ErrMore( "Duplicate( %s, %s )", TgtSecPtr->SecData.Name, SrcSecPtr->SecData.Name );
	return RetValue;
}


/****************************************************************
**	Routine: SecDbDuplicateCache
**	Returns: TRUE	- Values duplicated without error
**			 FALSE	- Error duplicating the values
**	Action : Copy the contents of the source security into the
**			 target security
****************************************************************/

int SecDbDuplicateCache(
	SDB_OBJECT		*TgtSecPtr,		// Target security pointer
	SDB_OBJECT		*SrcSecPtr,		// Source security pointer
	SDB_SET_FLAGS	CacheFlags,		// Cache Flags of values to duplicate
	SDB_SET_FLAGS	SetFlags,		// Flags to send to for SecDbSetValue
	int 			SkipSecurities,	// Whether or not to duplicate in-stream securities
	int             IgnoreSavedSecurities
)
{
	SDB_VALUE_TYPE_INFO
			*ValueTypeInfo;

	SDB_ENUM_PTR
			EnumPtr;

	DT_VALUE
			*ValueResults,
			ValueCopy;

	int		RetValue = TRUE,
			CreateHash = SecRefHash==NULL;

	if ( CreateHash )
	{
		SecRefHash = HashCreate( "SecRefDupHash", NULL, NULL, 0, NULL );
		HashInsert(	SecRefHash, (HASH_KEY) SrcSecPtr, (HASH_VALUE) TgtSecPtr );
	}

	for( ValueTypeInfo = SecDbValueTypeEnumFirst( SrcSecPtr, &EnumPtr ); RetValue && ValueTypeInfo; ValueTypeInfo = SecDbValueTypeEnumNext( EnumPtr ))
		if( SecDbGetFlags( SrcSecPtr, ValueTypeInfo->ValueType, 0, NULL ) & CacheFlags )
		{
			ValueResults = SecDbGetValue( SrcSecPtr, ValueTypeInfo->ValueType );

			RetValue = ValueResults != NULL;

			if ( RetValue )
			{
				if ( ValueResults->DataType == DtSecurity && ValueTypeInfo->ValueFlags & SDB_IN_STREAM )
				{
					if( SkipSecurities ) 
						continue;
					RetValue = SecDbDuplicateDtSecurity( &ValueCopy, 
														 ValueResults, 
														 CacheFlags, 
														 SetFlags, 
														 SkipSecurities, 
														 IgnoreSavedSecurities,
														 (DupFuncPtr) SecDbDuplicateCache );
				}
				else if ( ValueResults->DataType == DtSecurityList && ValueTypeInfo->ValueFlags & SDB_IN_STREAM )
				{
					if( SkipSecurities ) 
						continue;
					RetValue = SecDbDuplicateDtSecurityList( &ValueCopy, 
															 ValueResults, 
															 CacheFlags, 
															 SetFlags, 
															 SkipSecurities, 
															 IgnoreSavedSecurities,
															 (DupFuncPtr) SecDbDuplicateCache );
				}
				else
					RetValue = RetValue && DTM_ASSIGN( &ValueCopy, ValueResults );
			}

			if( RetValue )
			{
				if( !SecDbSetValue( TgtSecPtr, ValueTypeInfo->ValueType, &ValueCopy, SetFlags | SDB_CACHE_NO_COPY ))
				{
					DTM_FREE( &ValueCopy );
					RetValue = FALSE;
				}
			}
		}
	SecDbValueTypeEnumClose( EnumPtr );

	if ( CreateHash )
	{
		HashDestroy( SecRefHash );
		SecRefHash = NULL;
	}

	if( !RetValue )
		ErrMore( "Duplicate( %s, %s )", TgtSecPtr->SecData.Name, SrcSecPtr->SecData.Name );
	return RetValue;
}



