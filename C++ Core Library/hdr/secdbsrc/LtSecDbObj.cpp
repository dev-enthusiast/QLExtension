#define GSCVSID "$Header: /home/cvs/src/secdb/src/LtSecDbObj.cpp,v 1.8 2001/11/27 23:23:30 procmon Exp $"
/****************************************************************
**
**	LtSecDbObj.cpp	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: LtSecDbObj.cpp,v $
**	Revision 1.8  2001/11/27 23:23:30  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.5  2001/03/26 11:38:14  johnsoc
**	LtSecDb convertors to be registered on GsDt initialisation
**	
**	Revision 1.4  2001/01/31 22:12:13  johnsoc
**	Removed need to export LtSecDbObj converter functions
**	
**	Revision 1.3  2001/01/26 23:50:45  simpsk
**	Minor compilation fixes.
**	
**	Revision 1.2  2001/01/26 23:49:06  singhki
**	remove static from exported functions
**	
**	Revision 1.1  2001/01/26 19:07:50  johnsoc
**	Relocated LtSecDbObj class
**	
**
****************************************************************/


#define BUILDING_SECDB
#include <portable.h>
#include <ccstream.h>
#include <datatype.h>
#include <gscore/GsDate.h>
#include <gscore/GsStringVector.h>
#include <gsdt/GsDtDate.h>
#include <gsdt/GsDtArray.h>
#include <gsdt/GsDtDouble.h>
#include <gsdt/GsDtDictionary.h>
#include <gsdt/GsDtGeneric.h>
#include <gsdt/GsDtMatrix.h>
#include <gsdt/GsDtVector.h>
#include <gsquant/DictionaryUtil.h>
#include <gsdt/x_gsdt.h>
#include <secdb/LtSecDbObj.h>


CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: LtSecDbObj
**	Returns: 
**	Action : Construtor
****************************************************************/

LtSecDbObj::LtSecDbObj(
)
: m_SecDbObject( NULL )
{
}



/****************************************************************
**	Class  : LtSecDbObj
**	Routine: LtSecDbObj
**	Returns: 
**	Action : Construtor
****************************************************************/

LtSecDbObj::LtSecDbObj(
	SDB_OBJECT*	SecDbObject
)
: m_SecDbObject( SecDbObject )
{
	// No need to SecDbIncrementReference; assume we own this reference
	// i.e we are stealing this pointer.
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: LtSecDbObj
**	Returns: 
**	Action : Copy Construtor
****************************************************************/

LtSecDbObj::LtSecDbObj(
	const LtSecDbObj& Rhs
)
{
	m_SecDbObject = Rhs.m_SecDbObject;
	SecDbIncrementReference( m_SecDbObject );
}

/****************************************************************
**	Class  : LtSecDbObj
**	Routine: ~LtSecDbObj
**	Returns: 
**	Action : Destructor
****************************************************************/

LtSecDbObj::~LtSecDbObj(
)
{
	if ( m_SecDbObject )
		SecDbFree( m_SecDbObject );
}


/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: toString 
**	Returns: The stringized object.
**	Action : 
****************************************************************/

GsString LtSecDbObj::toString(
) const
{
	GsString
			Result = GsString( typeName() ) + " [ ";

	if( hasSecurity() )
		Result += GsString( "\"" ) + getString( GsString( "Security Name" ) ) + GsString( "\"" );
	else
		Result += "No Underying Security";

	Result += " ]";

	return Result;
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: operator=
**	Returns: *this
**	Action : Assignment
****************************************************************/

LtSecDbObj& LtSecDbObj::operator=(
	const LtSecDbObj& Rhs	
)
{
	if ( this != &Rhs )
	{
		if ( m_SecDbObject != Rhs.m_SecDbObject )
		{
			SecDbFree( m_SecDbObject );
			m_SecDbObject = Rhs.m_SecDbObject;
			SecDbIncrementReference( m_SecDbObject );
		}
	}
		
	return *this;
}

/****************************************************************
**	Class  : LtSecDbObj
**	Routine: SecDbRootDb
**	Returns: SDB_DB *
**	Action : Returns SecDbRootDb, and starts SecDb if not initialized
****************************************************************/
SDB_DB *LtSecDbObj::SecDbRootDb()
{
    if( !::SecDbRootDb )
        ::SecDbStartup( NULL, SDB_DEFAULT_DEADPOOL_SIZE, NULL, NULL, "-LtSecDbObj", "Default" );
    return( ::SecDbRootDb );
};


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getFromName
**	Returns: 
**	Action : 
****************************************************************/

LtSecDbObj LtSecDbObj::getByName(
	const GsString& SecurityName, 
	SDB_DB* Db,
	unsigned Flags
)
{
	SDB_OBJECT*
	  Object = secDbGetByName( SecurityName, Db, Flags);

	return LtSecDbObj( Object );
}



/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: secDbGetByName 
**	Returns: 
**	Action : 
****************************************************************/


SDB_OBJECT* LtSecDbObj::secDbGetByName(
	const GsString& SecurityName, 
	SDB_DB* Db,
	unsigned Flags
)
{
	SDB_OBJECT*
	  Object = SecDbGetByName( SecurityName.c_str(), Db, Flags);

	if( !Object )
		GSX_THROW( GsXSecDbError, ErrGetString() );
	
	return Object;
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: securityNew 
**	Returns: new security
**	Action : 
****************************************************************/

LtSecDbObj LtSecDbObj::securityNew(	
	const GsString& SecurityType, 
	const GsString& SecurityName,
	SDB_DB* Db
)
{
	SDB_SEC_TYPE SecType = SecDbClassTypeFromName( SecurityType.c_str() );

	if ( !SecType )
	{
		CC_OSTRSTREAM stream;
		stream << "LtSecDbObj::securityName " << ErrGetString();
		GSX_THROW( GsXSecDbError,  stream.str() );
	}
		
	SDB_OBJECT
			*SecPtr = SecDbNew( SecurityName.size() == 0 ? NULL : SecurityName.c_str(), SecType, Db );

	if ( !SecPtr )
	{
		CC_OSTRSTREAM stream;
		stream << "LtSecDbObj::securityName " << ErrGetString();
		GSX_THROW( GsXSecDbError,  stream.str() );
	}

	return LtSecDbObj( SecPtr );
}


/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: valueTypeFromTypeName 
**	Returns: SDB_VALUE_TYPE
**	Action : 
****************************************************************/

SDB_VALUE_TYPE LtSecDbObj::valueTypeFromTypeName(
	const GsString& ValueName, 
	DT_DATA_TYPE DataType
)
{
	SDB_VALUE_TYPE 
			ValueType = SecDbValueTypeFromName( ValueName.c_str(), DataType );


	if( !ValueType )
		GSX_THROW( GsXSecDbError, ErrGetString() );

	return ValueType;
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getValue
**	Returns: DT_VALUE*
**	Action : Get DT_VALUE* for ValueName
****************************************************************/

DT_VALUE* LtSecDbObj::getValue(
	SDB_OBJECT* SecDbObject, 
	const GsString& ValueName,
	DT_DATA_TYPE	DataType,
	LT_DT_ARGS_DEFINE
) 
{
	return getValue( SecDbObject, valueTypeFromTypeName( ValueName, DataType ), LT_DT_ARGS );
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getValue
**	Returns: DT_VALUE*
**	Action : Get DT_VAUE* for ValueType
****************************************************************/

DT_VALUE* LtSecDbObj::getValue(
	SDB_OBJECT* SecDbObject,
	SDB_VALUE_TYPE	ValueType,
	LT_DT_ARGS_DEFINE
) 
{
	DT_VALUE
			*Value = SecDbGetValueWithArgs( SecDbObject, ValueType, LT_DT_ARGS, ValueType->DataType );

	if( !Value )
	{
//  Inline assembly is evil - use a portable macro!
//  HOMANDA 12/31/1999
//	 __asm{int 3};
//
		GSX_THROW( GsXSecDbError, ErrGetString() );
	}

	return Value;
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: setValue 
**	Returns: 
**	Action : 
****************************************************************/

void LtSecDbObj::setValue(
	SDB_OBJECT*	SecDbObject,
	const GsString& ValueName, 
	DT_VALUE*		Value,
	LT_DT_ARGS_DEFINE 
)
{
	setValue( SecDbObject, valueTypeFromTypeName( ValueName, Value->DataType ), Value, LT_DT_ARGS );
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: setValue 
**	Returns: 
**	Action : 
****************************************************************/

void  LtSecDbObj::setValue(
	SDB_OBJECT* SecDbObject,
	SDB_VALUE_TYPE	ValueType,
	DT_VALUE*		Value,
	LT_DT_ARGS_DEFINE
)
{
	if( !SecDbSetValueWithArgs( SecDbObject, ValueType, LT_DT_ARGS, Value, 0) )
		GSX_THROW( GsXSecDbError, ErrGetString() );
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: setDiddle 
**	Returns: 
**	Action : 
****************************************************************/

SDB_DIDDLE_ID LtSecDbObj::setDiddle(
	SDB_OBJECT*	SecDbObject,
	const GsString& ValueName, 
	DT_VALUE*		Value,
	LT_DT_ARGS_DEFINE 
)
{
	return setDiddle( SecDbObject, valueTypeFromTypeName( ValueName, Value->DataType ), Value, LT_DT_ARGS );
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: setDiddle 
**	Returns: 
**	Action : 
****************************************************************/

SDB_DIDDLE_ID LtSecDbObj::setDiddle(
	SDB_OBJECT* SecDbObject,
	SDB_VALUE_TYPE	ValueType,
	DT_VALUE*		Value,
	LT_DT_ARGS_DEFINE
)
{
	SDB_DIDDLE_ID toReturn = SecDbSetDiddleWithArgs( SecDbObject, ValueType, LT_DT_ARGS, Value, 0, SDB_DIDDLE_ID_NEW );

	if( toReturn == SDB_DIDDLE_ID_ERROR )
		GSX_THROW( GsXSecDbError, ErrGetString() );

	return toReturn; 
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: removeDiddle 
**	Returns: 
**	Action : 
****************************************************************/

void LtSecDbObj::removeDiddle(
	SDB_OBJECT*	SecDbObject,
	const GsString& ValueName, 
	DT_VALUE*		Value,
	SDB_DIDDLE_ID DiddleId,
	LT_DT_ARGS_DEFINE
)
{
	removeDiddle( SecDbObject, valueTypeFromTypeName( ValueName, Value->DataType ), DiddleId, LT_DT_ARGS );
}

/****************************************************************
**	Class  : LtSecDbObj 
**	Routine: removeDiddle 
**	Returns: 
**	Action : 
****************************************************************/

void LtSecDbObj::removeDiddle(
	SDB_OBJECT* SecDbObject,
	SDB_VALUE_TYPE	ValueType,
	SDB_DIDDLE_ID DiddleId,
	LT_DT_ARGS_DEFINE
)
{
	if( ! SecDbRemoveDiddleWithArgs( SecDbObject, ValueType, LT_DT_ARGS, DiddleId ) )
		GSX_THROW( GsXSecDbError, ErrGetString() );
}

/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getDouble
**	Returns: double
**	Action : Get ValueName as double
****************************************************************/

double LtSecDbObj::getDouble(
	SDB_OBJECT* SecDbObject,
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE 
) 
{
	return getValue( SecDbObject, ValueName, DtDouble, LT_DT_ARGS )->Data.Number;
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getString
**	Returns: GsString
**	Action : Get ValueName as GsString
****************************************************************/

GsString LtSecDbObj::getString(
	SDB_OBJECT* SecDbObject, 
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE
) 
{
	return GsString( static_cast<const char*>( getValue( SecDbObject, ValueName, DtString, LT_DT_ARGS )->Data.Pointer ) );
}



/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getAsString
**	Returns: GsString
**	Action : Get ValueName, convert to GsString, and return.
****************************************************************/

GsString LtSecDbObj::getAsString(
	SDB_OBJECT* SecDbObject, 
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE
) 
{
	const DT_VALUE
			*Value = getValue( SecDbObject, ValueName, NULL, LT_DT_ARGS );

	DT_VALUE
			ValueAsString;

	DTM_ALLOC( &ValueAsString, DtString );

	if( !DTM_TO( &ValueAsString, const_cast< DT_VALUE *>( Value ) ) )
		GSX_THROW( GsXSecDbError, GsString( "Failed to convert value to a string\n" ) + ErrGetString() );

	GsString	
			Result( static_cast<const char*>( ValueAsString.Data.Pointer ) );

	DTM_FREE( &ValueAsString );

	return Result;
}



/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getArray
**	Returns: GsDtArray as a GsDt that must be freed by caller.
**	Action : Get ValueName as GsDtArray.  DtArray is converted if possible.
****************************************************************/

GsDtArray *LtSecDbObj::getArray(
	SDB_OBJECT* SecDbObject, 
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE
) 
{
	const DT_VALUE
			*Value = getValue( SecDbObject, ValueName, NULL, LT_DT_ARGS );

	GsDtArray
			*ValueAsArray = NULL;

	const GsDtArray
			*ConstValueAsArray;

	if( Value->DataType == DtGsDt )
	{
		if( ( ConstValueAsArray = GsDtArray::Cast( static_cast<const GsDt*>( Value->Data.Pointer ) ) ) )
			ValueAsArray = const_cast< GsDtArray * >( GsDtArray::Cast( ConstValueAsArray->Copy() ) );
	}
	else if( Value->DataType == DtArray )
	{
		// FIX - GsDtFromDtValue should take a const DT_VALUE * as an arg.
		GsDt	*ConvertedDt = GsDtFromDtValue( const_cast< DT_VALUE * >( Value ) );
		if( NULL == ConvertedDt )
			GSX_THROW( GsXInvalidArgument, GsString( "LtSecDbObj::getArray: Couldn't convert Array to GsDt\n" )
											+ ErrGetString() );
		if( ( ConstValueAsArray = GsDtArray::Cast( ConvertedDt ) ) )
			ValueAsArray = const_cast< GsDtArray * >( ConstValueAsArray );		
	}

	if( ValueAsArray )
		return ValueAsArray;

	GSX_THROW( GsXInvalidArgument, "LtSecDbObj::getArray: Value must be an array or a GsDtArray already." );
	return NULL;
}



/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getDictionary
**	Returns: GsDtDictionary as a GsDt that must be freed by caller.
**	Action : Get ValueName as GsDtDictionary.  DtStructure is converted if possible.
****************************************************************/

GsDtDictionary *LtSecDbObj::getDictionary(
	SDB_OBJECT* SecDbObject, 
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE
) 
{
	const DT_VALUE
			*Value = getValue( SecDbObject, ValueName, NULL, LT_DT_ARGS );

	GsDtDictionary
			*ValueAsDictionary = NULL;

	const GsDtDictionary
			*ConstValueAsDictionary;

	if( Value->DataType == DtGsDt )
	{
		if( ( ConstValueAsDictionary = GsDtDictionary::Cast( static_cast<const GsDt*>( Value->Data.Pointer ) ) ) )
			ValueAsDictionary = const_cast< GsDtDictionary * >( GsDtDictionary::Cast( ConstValueAsDictionary->Copy() ) );
	}
	else if( Value->DataType == DtStructure )
	{
		// FIX - GsDtFromDtValue should take a const DT_VALUE * as an arg.
		GsDt	*ConvertedDt = GsDtFromDtValue( const_cast< DT_VALUE * >( Value ) );

		if( ( ConstValueAsDictionary = GsDtDictionary::Cast( ConvertedDt ) ) )
			ValueAsDictionary = const_cast< GsDtDictionary * >( ConstValueAsDictionary );		
	}

	if( ValueAsDictionary )
		return ValueAsDictionary;

	GSX_THROW( GsXInvalidArgument, "LtSecDbObj::getDictionary: Value must be an structure or a GsDtDictionary already." );
	return NULL;
}



/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getDate
**	Returns: GsDate
**	Action : Get ValueName as GsString
****************************************************************/

GsDate LtSecDbObj::getDate(
	SDB_OBJECT* SecDbObject,
	const GsString& ValueName,
	LT_DT_ARGS_DEFINE
)
{
	GsDate	RetDate;
		   
	DT_VALUE
			*Value = getValue( SecDbObject, ValueName, DtDate, LT_DT_ARGS );

// 	if( Value->DataType == DtGsDt )
// 	{
// 		const GsDt* Obj = static_cast<GsDt*>( const_cast<void *>( Value->Data.Pointer ) );
// 		const GsDtDate* DtVal = GsDtDate::Cast( Obj );

// 		if( DtVal )
// 		{
// 			RetDate = DtVal->data();
// 		}
// 		else
// 		{	
// 			CC_OSTRSTREAM stream;
// 			stream << "LtSecDbObj::getDate: Expected " << "GsDtDate, not " << Obj->typeName();
// 			GSX_THROW( GsXSecDbError, stream.str() );
// 		}
// 	} else
	if( Value->DataType == DtDate )
	{
		RetDate = GsDate::fromDATE( Value->Data.Number );
	}
	else
	{	
		CC_OSTRSTREAM stream;
		stream << "LtSecDbObj::getDate: Expected " << "GsDtDate, not " << Value->DataType->Name;
		GSX_THROW( GsXSecDbError, stream.str() );
	}
		
	return  RetDate;
}


/****************************************************************
**	Class  : LtSecDbObj
**	Routine: getGsDt
**	Returns: GsDt*
**	Action : Get ValueName as GsDt 
****************************************************************/

const GsDt* LtSecDbObj::getGsDt( 
	SDB_OBJECT* SecDbObject,
	const GsString& ValueName, 
	LT_DT_ARGS_DEFINE
)
{
	DT_VALUE
			*Value = getValue( SecDbObject, ValueName, DtGsDt, LT_DT_ARGS );

	return static_cast<const GsDt*>( Value->Data.Pointer );
}


CC_END_NAMESPACE





