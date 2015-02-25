/****************************************************************
**
**	SECINDEX.H	- Security Database Index information
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secindex.h,v 1.30 2010/04/08 16:56:06 ellist Exp $
**
****************************************************************/

#if !defined( IN_SECINDEX_H )
#define IN_SECINDEX_H

// Forward declare structures to make IBM_CPP happy

#ifndef _IN_SECDB_H
#include <secdb.h>
#endif

/*
**	Define constants
*/

unsigned const SDB_INDEX_NAME_SIZE	= 32;
unsigned const SDB_CONSTRAINT_SIZE	= 32;
int const SDB_MAX_INDEX_PARTS	= 16;
int const SDB_MAX_INDEX_CONSTRAINTS	= 4;

// Could one day have AND and OR versions for more flexible matching
short const SDB_INDEX_CONSTRAINED	= 0x0001;

short const SDB_CONSTRAIN_EQUAL	= 0x0001;
short const SDB_CONSTRAIN_NOT	= 0x0002;

int const SDB_INDEX_ASCENDING	= 0x0001;
int const SDB_INDEX_DESCENDING	= 0x0002;
int const SDB_INDEX_NUMERIC		= 0x0004;
int const SDB_INDEX_STRING		= 0x0008;


/*
**	Structures used to hold index information
*/

typedef long SDB_INDEX_ID;					// Unique Index ID

struct SDB_INDEX_PART
{
	SDB_VALUE_TYPE
			ValueType;						// Value types to index by

	short	Flags,							// Ascending, etc...
			ByteWidth;						// Byte width of part

};

union SDB_INDEX_CONSTRAINT_VALUE
{
	double	NumericValue;

	char	StringValue[ SDB_CONSTRAINT_SIZE ];
};

struct SDB_INDEX_CONSTRAINT
{
	char	VtName[ SDB_VALUE_NAME_SIZE ];	// Value type to check

	DT_DATA_TYPE
			DataType;

	short	ConstraintType;					// How to compare - check equality, etc. 

	SDB_INDEX_CONSTRAINT_VALUE
			Value;							// Value to match
};

struct SDB_INDEX
{
	char	Name[ SDB_INDEX_NAME_SIZE ],	// Name of the index
			Obsolete[ 32 ];					// Name of database

	short	PartCount,						// Number of parts to index
			ClassCount,						// Number of classes that use the index
			TotalByteWidth,					// Number of bytes of index parts
			OpenFlag;						// Flag indicating if index is open

	SDB_INDEX_PART
			Parts[ SDB_MAX_INDEX_PARTS ];	// Parts that make up the index

	SDB_SEC_TYPE
			*Classes;						// Table of security classes

	SDB_DB	*pDb;							// Database pointer

	void	*Handle;						// Pointer reserved for the
											// database driver that keeps track
											// of the index

	SDB_INDEX_ID							// Unique ID-assigned by driver
			IndexID;						//    when index is opened

	short	DescriptorOnly;					// FALSE - Real index
											// TRUE  - Fake index, descriptor only

	short	BuildingIndex;					// only allowed to add classes while we are building the index

	short	Flags;							// Index-level flags

	short	ConstraintCount;				// Number of constraints used to determine inclusion
											// in sparse index

	SDB_INDEX_CONSTRAINT
			Constraints[ SDB_MAX_INDEX_CONSTRAINTS ];
};


/*
**	Linked list structure used by security classes to point to indices
*/

struct SDB_INDEX_POINTER
{
	SDB_INDEX_POINTER* Next;   // Next pointer in list
	SDB_INDEX*		   Index;  // Pointer to index
};


/*
**	Common structure for SDB_INDEX_POS & SDB_TRAN_INDEX*
*/

struct SDB_INDEX_VALUES
{
	SDB_INDEX
			*Index;							// Pointer to associated index

	char	SecName[ SDB_SEC_NAME_SIZE ];	// Name of security found

	DT_VALUE
			Values[ SDB_MAX_INDEX_PARTS ];	// Values to search, values found

};


/*
**	Structure used for maintaining position when performing
**	index retrieval of securities
*/

struct SDB_INDEX_POS
{
	SDB_INDEX_VALUES
			Data;

	int		Modified;						// TRUE if structure modified

	void	*Handle;						// Pointer reserved for database

	SDB_INDEX_POS							// for virtual databases
			*Left,
			*Right;

};


/*
**	Index Descriptor
*/

struct SDB_INDEX_DESCRIPTOR_PART
{
	char	ValueTypeName[	SDB_VALUE_NAME_SIZE ],
			DataTypeName[	DT_DATA_TYPE_NAME_SIZE ];

	short	Flags,				// Ascending, etc...
			ByteWidth,			// Byte width of part
			Offset;				// Byte offset of part

};

struct SDB_INDEX_DESCRIPTOR
{
	char	Name[ SDB_INDEX_NAME_SIZE ];	// Name of the index
	
	short	TotalByteWidth,
			PartCount;						// Number of parts to index

	SDB_INDEX_DESCRIPTOR_PART
			Parts[ SDB_MAX_INDEX_PARTS ];	// Parts that make up the index

	SDB_INDEX_ID							// Assigned by driver
			IndexID;

};


/*
**	Structure used to return object checksums
*/

struct SDB_OBJECT_CHECK_SUM
{
	char	IndexName[ SDB_INDEX_NAME_SIZE ];

	UINT16	RecordCrc;

	SDB_OBJECT_CHECK_SUM*
			Next;

};


/*
**	Prototype functions
*/

EXPORT_C_SECDB long
		SecDbIndexBuildPartCount;	// Default 100

EXPORT_C_SECDB SDB_INDEX
		*SecDbIndexNew(				const char *Name, SDB_DB *Db ),
		*SecDbIndexFromName( 		const char *Name, SDB_DB *Db ),
		*SecDbIndexEnumFirst(		SDB_ENUM_PTR *EnumPtr, SDB_DB *pDb ),
		*SecDbIndexEnumNext(		SDB_ENUM_PTR EnumPtr, SDB_DB *pDb );

EXPORT_C_SECDB void
		SecDbIndexClose(			SDB_INDEX *Index, SDB_DB *Db, int RemoveFlag ),
		SecDbIndexEnumClose(		SDB_ENUM_PTR EnumPtr );

EXPORT_C_SECDB int
		SecDbIndexAdd(				SDB_INDEX *Index ),
		SecDbIndexAddPart(	   		SDB_INDEX *Index, SDB_VALUE_TYPE ValueType, int Flags, int ByteWidth ),
		SecDbIndexAddConstraint(	SDB_INDEX *Index, const char *VtName, short Type, DT_VALUE *Value ),
		SecDbIndexAddClass(	   		SDB_INDEX *Index, SDB_SEC_TYPE SecType, int ModifyExistingIndex ),
		SecDbIndexDeleteClass( 		SDB_INDEX *Index, SDB_SEC_TYPE SecType, int ModifyExistingIndex ),
		SecDbIndexCreate(			SDB_INDEX *Index, SDB_DB *Db ),
		SecDbIndexRegister(			SDB_INDEX *Index ),
		SecDbIndexGetByName(		SDB_INDEX_POS *IndexPos, int GetType, const char *SecName ),
		SecDbIndexRebuild(			char *Name, SDB_DB *Db ),
		SecDbIndexDestroy(			char *Name, SDB_DB *Db ),
		SecDbIndexValidate(			char *Name, SDB_DB *Db, int ErrorLimit ),
		SecDbIndexRepairObject(		char *SecName, SDB_DB *Db ),
		SecDbIndexAddClassToExisting( char *Name, SDB_DB *Db, SDB_SEC_TYPE SecType ),
		SecDbIndexDeleteClassFromExisting( char *Name, SDB_DB *Db, SDB_SEC_TYPE SecType );

EXPORT_C_SECDB char
		*SecDbIndexGet(				SDB_INDEX_POS *IndexPos, int GetType );

EXPORT_C_SECDB SDB_INDEX_POS
		*SecDbIndexPosCreate(		SDB_INDEX *Index ),
		*SecDbIndexPosCopy(			SDB_INDEX_POS *IndexPos );

EXPORT_C_SECDB void
		SecDbIndexPosDestroy(		SDB_INDEX_POS *IndexPos );

EXPORT_C_SECDB int
		SecDbIndexPosSetValue(		SDB_INDEX_POS *IndexPos, SDB_VALUE_TYPE ValueType, DT_VALUE *Value ),
		SecDbIndexPosCompare(		SDB_INDEX_POS *IndexPos1, SDB_INDEX_POS *IndexPos2 );

EXPORT_C_SECDB DT_VALUE
		*SecDbIndexPosGetValue(		SDB_INDEX_POS *IndexPos, SDB_VALUE_TYPE ValueType );

#endif

