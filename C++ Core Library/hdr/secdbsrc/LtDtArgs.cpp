#define GSCVSID "$Header: /home/cvs/src/secdb/src/LtDtArgs.cpp,v 1.6 2001/11/27 23:23:30 procmon Exp $"
/****************************************************************
**
**	LtDtArgs.cpp	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Log: LtDtArgs.cpp,v $
**	Revision 1.6  2001/11/27 23:23:30  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.4  2001/03/01 14:18:18  goodfj
**	No you are not building LTSECDB
**	
**	Revision 1.3  2001/01/26 23:50:45  simpsk
**	Minor compilation fixes.
**	
**	Revision 1.2  2001/01/26 19:29:00  johnsoc
**	Incorporated brownb's changes
**	
**
****************************************************************/

#define BUILDING_SECDB
#include <portable.h>
#include <secdb/LtDtArgs.h>
#include <gsdt/GsDt.h>
#include <gscore/GsString.h>
#include <gscore/GsDate.h>
#include <gsdt/GsDtDictionary.h>
#include <gsdt/x_gsdt.h>

CC_BEGIN_NAMESPACE( Gs )


/****************************************************************
**	Class  : LtDtArgs 
**	Routine: LtDtArgs
**	Returns: nothing
**	Action : Constructor
****************************************************************/

LtDtArgs::LtDtArgs(
)
: m_Array( NULL )
{
}


/****************************************************************
**	Class  : LtDtArgs
**	Routine: LtDtArgs
**	Returns: 
**	Action : Copy Constructor
****************************************************************/

LtDtArgs::LtDtArgs(
	const LtDtArgs& Rhs
)
: m_Array( NULL )
{
	// Do not copy m_Array , set it to null so it will be created when array() is called
	valueAssign( Rhs );
}

/****************************************************************
**	Class  : LtDtArgs
**	Routine: ~LtDtArgs
**	Returns: 
**	Action : Destructor
****************************************************************/

LtDtArgs::~LtDtArgs(
)
{
	clear();

	if ( m_Array )
		delete [] m_Array;
}





/****************************************************************
**	Class  : LtDtArgs
**	Routine: operator=
**	Returns: *this
**	Action : Assignment
****************************************************************/

LtDtArgs& LtDtArgs::operator=(
	const LtDtArgs& Rhs
)
{
	if ( this != &Rhs )
	{
		// Do not copy m_Array , set it to null so it will be created when array() is called
		// Delete any existing m_Array;

		if ( m_Array )
		{
			delete[] m_Array;
			m_Array = NULL;
		}

		valueAssign( Rhs );
	}

	return *this;
}


/****************************************************************
**	Class  : LtDtArgs
**	Routine: array
**	Returns: m_Array
**	Action : create and return m_Array;
****************************************************************/

DT_VALUE** LtDtArgs::array(
)
{
	if ( m_Array )
		delete [] m_Array;

	int	Size = size();

	if ( Size )
	{
		m_Array = new DT_VALUE*[ Size ];

		for( int i = 0; i < Size; i++)
			m_Array[i] = &m_DtValues[i];
	}
	else
		m_Array = NULL;

	return m_Array;
}



/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : put a DtDouble DT_VALUE in arg list
****************************************************************/

void LtDtArgs::add(
	double Value
)
{
	DT_VALUE 
			DtValue;

	DT_NUMBER_TO_VALUE( &DtValue, Value, DtDouble );

	m_DtValues.push_back( DtValue );
}

/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : 
****************************************************************/

void LtDtArgs::add(
	DT_DATA_TYPE	DataType,
	void			*Pointer
)
{
	DT_VALUE 
			DtValue;

	DT_POINTER_TO_VALUE( &DtValue, Pointer, DataType );

	m_DtValues.push_back( DtValue );
}

/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : put a DtDate DT_VALUE in arg list
****************************************************************/

void LtDtArgs::add(
	const GsDate& Value
)
{
	DT_VALUE 
			DtValue;

	DT_NUMBER_TO_VALUE( &DtValue, Value.toDATE(), DtDate );

	m_DtValues.push_back( DtValue );
}

/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : put a DtString DT_VALUE in arg list
****************************************************************/

void LtDtArgs::add(
	const GsString& Value
)
{
	DT_VALUE 
			DtValue;

	DT_POINTER_TO_VALUE( &DtValue, strdup( Value.c_str() ), DtString );
	m_DtValues.push_back( DtValue );
}


/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : Add a GsDt derived type to the arg list 
**		   : WE OWN THIS POINTER NOW
****************************************************************/

void LtDtArgs::add(
	GsDt* Value
)
{
	DT_VALUE
			DtValue;

	DT_POINTER_TO_VALUE( &DtValue, Value, DtGsDt );

	m_DtValues.push_back( DtValue );
}


/****************************************************************
**	Class  : LtDtArgs
**	Routine: add
**	Returns: 
**	Action : Add a GsDt derived type to the arg list 
**		   : WE OWN THIS POINTER NOW
****************************************************************/

void LtDtArgs::add(
	DT_VALUE* Value
)
{
	m_DtValues.push_back( *Value );
}


/****************************************************************
**	Class  : LtDtArgs 
**	Routine: valueAssign 
**	Returns: 
**	Action : 
****************************************************************/

void LtDtArgs::valueAssign(
	const LtDtArgs& Other
)
{
	size_t size = Other.m_DtValues.size();

	clear();
	m_DtValues.resize( size );

	for( size_t i = 0; i < size; i++ )
		DTM_ASSIGN( &m_DtValues[i], (DT_VALUE *) &Other.m_DtValues[i] );
}


/****************************************************************
**	Class  : LtDtArgs 
**	Routine: clear 
**	Returns: 
**	Action : 
****************************************************************/

void LtDtArgs::clear(
)
{
	size_t size = m_DtValues.size();

	for( size_t i = 0; i < size; i++ )
		DTM_FREE( &m_DtValues[i] );
}

// specialize for some types, other are handled by templates

void LtDtArgAdd( LtDtArgs& Args, double Value )
{
	Args.add( Value );
}

void LtDtArgAdd( LtDtArgs& Args, int Value )
{
	double Val = Value;
	Args.add( Val );
}


void LtDtArgAdd( LtDtArgs& Args, const GsString& Value )
{
	Args.add( Value );
}

void LtDtArgAdd( LtDtArgs& Args, const GsDate& Value )
{
	Args.add( Value );
}

void LtDtArgAdd( LtDtArgs& Args, const GsDt& Value )
{
	Args.add( Value.Copy() );
}

void LtDtArgAdd( LtDtArgs& Args, GsDt* Value )
{
	Args.add( Value );
}

void LtDtArgAdd( LtDtArgs& Args, const GsDtDictionary& Value, bool ConvertToStructure )
{
	if( ConvertToStructure )
	{	
		DT_VALUE DtVal;

		if( GS_OK == GsDtToDtValue( Value, &DtVal, GSDT_TO_DTV_FAIL_WITH_DTGSDT ) )
		{
			Args.add( &DtVal );
		}
		else
		{
			GSX_THROW( GsXInvalidArgument, ErrGetString() );
		}
	}
	else
	{
		Args.add( Value.Copy() );
	}
}

CC_END_NAMESPACE
