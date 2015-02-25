/****************************************************************
**
**	fcadlib/adslang.h	- Adlib functions/macros for Slang
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adslang.h,v 1.84 2012/04/12 17:33:23 e19351 Exp $
**
****************************************************************/

#ifndef IN_FCADLIB_ADSLANG_H
#define IN_FCADLIB_ADSLANG_H

#include <secexpr.h>
#include <gsdt/x_gsdt.h>
#include <gsdt/GsDtTypeMap.h>
#include <gscore/gsdt_fwd.h>
#include <fcadlib/AdExpandArg.h>
#include <fcadlib/AdExpandData.h>
#include <fcadlib/AdUsageReporting.h>
#include <secdb/GsNodeValues.h>

#include <exception> // for std::exception

CC_BEGIN_NAMESPACE( Gs )

/*
**	Some forward declarations
*/

class GsDtFunc; // XXX should this be in <gscore/gsdt_fwd.h> ?
class GsSymbolicExpressionVector;
template<class T> GsStatus AdReturn( SLANG_RET &Ret, T Val );

/*
**	Slang
*/

#define AD_API				SLANG_RET_CODE
#define AD_API_EXP			DLLEXPORT AD_API
#define AD_API_ARGS			SLANG_ARGS
#define AD_ARG_COUNT		Root->Argc
#define AD_ARG(_Ct)			SLANG_VAL(_Ct)
#define AD_ARGS(_Nm,_Ct)	_Nm, AD_ARG(_Ct)
#define AD_RET_ARGS			*Ret


#define AD_FUNC_ENTER(_Name_)					\
	Gs::AdReportSlangUsage( _Name_ ); 			\
	AD_API __AD__RET_CODE__ = SLANG_OK;			\
	try											\
	{											\
		SLANG_ARG_PARSE()

//	This used to have another line to catch unknown exceptions, but then it
//	also caught BusErrors and did not invoke the debugger.
//	catch( ... )				{ AdSlangHandleUnknownException( SLANG_ERROR_PARMS );		}
#define AD_FUNC_FAIL(_Name_)																							\
		__AD__RET_CODE__ = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );									\
	}																													\
	catch( GsException& GsE )		{ __AD__RET_CODE__ = AdSlangHandleGsException(	 SLANG_ERROR_PARMS, GsE );	}		\
	catch( CC_NS(std,exception)& E )	{ __AD__RET_CODE__ = AdSlangHandleException(	 SLANG_ERROR_PARMS, E );	}	\
	SLANG_ARG_FREE();																									\
	return __AD__RET_CODE__;


#define AD_FUNC_OK(_Name_)		\
{								\
	SLANG_ARG_FREE();			\
	return SLANG_OK;			\
}


#define AD_FUNC_ENTER_NO_ARG(_Name_)			\
    AD_API __AD__RET_CODE__ = SLANG_OK;         \
	try											\
	{											\
		SLANG_ARG_NONE()

#define AD_FUNC_FAIL_NO_ARG(_Name_)																										\
		__AD__RET_CODE__ = SlangEvalError( SLANG_ERROR_PARMS, ERR_STR_DONT_CHANGE );																\
	}																																	\
	catch( GsException& GsE )		{ __AD__RET_CODE__ = AdSlangHandleGsException(	 SLANG_ERROR_PARMS, GsE );	}		\
	catch( CC_NS(std,exception)& E )	{ __AD__RET_CODE__ = AdSlangHandleException(	 SLANG_ERROR_PARMS, E );	}	\
	return __AD__RET_CODE__;


#define AD_FUNC_OK_NO_ARG(_Name_)	\
{									\
	return SLANG_OK;				\
}


// SLANG_ARG_PARSE already checked this
#define AD_ARG_COUNT_CHECK(_Need_)	TRUE
#define AD_GET_DATE(_Name_,_ArgNum_,_Value_)	AdGetArg_DATE( Root, _Name_, SLANG_INT(_ArgNum_), _Value_ )

inline bool AdIsArgMissing( DT_VALUE& ArgVal )
{
	return ArgVal.DataType == DtNull;
}


EXPORT_CPP_FCADLIBS AD_API
		AdSlangHandleException(			SLANG_ARGS, CC_NS(std,exception)& E ),
		AdSlangHandleGsException(		SLANG_ARGS, GsException& E ),
		AdSlangHandleUnknownException(	SLANG_ARGS );

inline GsStatus
		AdGetArgCheck(			const char *FuncName, int Need, int Got );

GsStatus
		AdReturnErr(			void ),
		AdReturnNA(				void );
		
EXPORT_CPP_FCADLIBS void AdReportSlangUsage( const char* functionName );

/****************************************************************
**	Routine: AdGetArgCheck
**	Returns: GsStatus
**	Action : Check arg counts
****************************************************************/

inline GsStatus AdGetArgCheck(
	const char	*FuncName,
	int			Need,
	int			Got
)
{
	if( Need == Got )
		return GS_OK;
	return GsErr( ERR_INVALID_ARGUMENTS, "%s: Expected %d args, got %d args",
				  FuncName, Need, Got );
}



/****************************************************************
**	Routine: <class GsDt_T>AdGetArg
**	Returns: GsStatus
**	Action : Get a GsDtXXXX argument
****************************************************************/

template<class T>
GsStatus AdGetArgGsDt(
	const char*		Name,
	DT_VALUE&		ArgVal,		// Must be DtGsDt
	AutoArgRef<T>&	Value
)
{
	if( ArgVal.DataType == DtGsDt )
	{
		const GsDt *Obj = (GsDt *) ArgVal.Data.Pointer;
		const T	*Val = T::Cast( Obj );
		if( !Val )
		{
			return GsErrMore( "%s: Expected GsDt/%s, not %s",
							  Name, T::staticType().name().c_str(), Obj->typeName() );
		}
		Value.cPtrSet( Val );
	}
	else
	{
		GsDt* Obj = GsDtFromDtValue(&ArgVal);

		T	*Val;
		
		if( !Obj || !( Val = T::Cast( Obj ) ) )
		{
			if (Obj)
				delete Obj;
			return GsErrMore( "%s: Cannot get GsDt/%s from %s",
							  Name, 
							  T::staticType().name().c_str(), 
							  DtNameFromType( ArgVal.DataType ) );
		}
		Value.ptrSet( Val );
	}
	return GS_OK;
}



/****************************************************************
**	Routine: AdGetArg<T>
**	Returns: GsStatus
**	Action : Retrieve a T from ArgVal (assumed to be a handle)
****************************************************************/

template<class T>
GsStatus AdGetArg(
	const char*	Name,
	DT_VALUE&	ArgVal,
	T&			Value
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( ArgVal.DataType == DtGsDt )
	{
		GsDt       const* const Obj = static_cast<GsDt const*>( ArgVal.Data.Pointer );
		MyGsDtType const* const Val = MyGsDtType::Cast( Obj );

		if( !Val )
			return GsErrMore( "%s: Expected GsDt/%s, not %s",
							  Name, 
							  MyGsDtType::staticType().name().c_str(),
							  Obj->typeName() );
		Value = Val->data();
		return GS_OK;
	}

	// else
	{
	    GsDt       const* const Obj = GsDtFromDtValue( &ArgVal );
		MyGsDtType const* const Val = Obj ? MyGsDtType::Cast( Obj ) : NULL;
		if( !Val )
		    return GsErrMore( "%s: Expected GsDt/%s, not Dt/%s",
							  Name, 
							  MyGsDtType::staticType().name().c_str(),
							  ArgVal.DataType->Name );
		Value = Val->data();
		delete Obj; // SW4.2 incorrectly complains about this line.
	}
	return GS_OK;
}



/****************************************************************
**	Routine: AdGetArg<double>
**	Returns: GsStatus
**	Action : Retrieve a double from ArgVal
****************************************************************/

template<> inline GsStatus AdGetArg(
	const char	*Name,
	DT_VALUE	&ArgVal,
	double		&Value
)
{
	if( ArgVal.DataType != DtDouble )
		return GsErr( ERR_INVALID_ARGUMENTS, "%s: Expected %s, not %s", Name, DtDouble->Name, ArgVal.DataType->Name );

	Value = ArgVal.Data.Number;

	return GS_OK;
}




/****************************************************************
**	Routine: AdReturn
**	Returns: GsStatus
**	Action : Store a double in Ret
****************************************************************/

template<> inline GsStatus AdReturn(
	SLANG_RET	&Ret,
	double		RetVal
)
{
	if( !VALID_DOUBLE( RetVal ))
		return GS_ERR;	// The called function should have already set the error state

	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtDouble;
	Ret.Data.Value.Data.Number	= RetVal;

	return GS_OK;
}



/****************************************************************
**	Routine: AdGetArg<int>
**	Returns: GsStatus
**	Action : Retrieve an int from ArgVal
****************************************************************/

template<> inline GsStatus AdGetArg(
	const char	*Name,
	DT_VALUE	&ArgVal,
	int			&Value
)
{
	double	Tmp = ArgVal.Data.Number;
	if( Tmp < INT_MIN || Tmp > INT_MAX )
		return GsErr( ERR_INVALID_NUMBER, "%s: Expected unsigned btwn %d & %d", Name, INT_MIN, INT_MAX );
	Value = static_cast<int>( Tmp );

	return GS_OK;
}


/****************************************************************
**	Routine: AdReturn<int>
**	Returns: GsStatus
**	Action : Store an int in Ret
****************************************************************/

template<> inline GsStatus AdReturn(
	SLANG_RET	&Ret,
	int			RetVal
)
{
	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtDouble;
	Ret.Data.Value.Data.Number	= RetVal;

	return GS_OK;
}


/****************************************************************
**	Routine: AdGetArg<unsigned>
**	Returns: GsStatus
**	Action : Retrieve an unsigned from ArgVal
****************************************************************/

template<> inline GsStatus AdGetArg(
	const char	*Name,
	DT_VALUE	&ArgVal,
	unsigned	&Value
)
{
	double	Tmp = ArgVal.Data.Number;
	if( Tmp < 0 || Tmp > UINT_MAX )
		return GsErr( ERR_INVALID_NUMBER, "%s: Expected unsigned btwn %u & %u", Name, 0, UINT_MAX );
	Value = static_cast<unsigned>( Tmp );

	return GS_OK;
}


/****************************************************************
**	Routine: AdReturn<unsigned>
**	Returns: GsStatus
**	Action : Store an unsigned in Ret
****************************************************************/

template<> inline GsStatus AdReturn(
	SLANG_RET	&Ret,
	unsigned	RetVal
)
{
	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtDouble;
	Ret.Data.Value.Data.Number	= RetVal;

	return GS_OK;
}


#ifndef CC_BOOL_IS_INT
/****************************************************************
**	Routine: AdGetArg<bool>
**	Returns: GsStatus
**	Action : Retrieve an bool from ArgVal
****************************************************************/

template<> inline GsStatus AdGetArg(
	const char	*Name,
	DT_VALUE	&ArgVal,
	bool		&Value
)
{
	double	Tmp = ArgVal.Data.Number;
	// Use Tmp != 0 to avoid warning from compiler if using static_cast<bool>
	Value = Tmp != 0;

	return GS_OK;
}


/****************************************************************
**	Routine: AdReturn<bool>
**	Returns: GsStatus
**	Action : Store an bool in Ret
****************************************************************/

template<> inline GsStatus AdReturn(
	SLANG_RET	&Ret,
	bool		RetVal
)
{
	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtDouble;
	Ret.Data.Value.Data.Number	= RetVal;

	return GS_OK;
}
#endif /* !CC_BOOL_IS_INT */


/*
**	Strange, it seems necessary to define this after the functions in order
**	to avoid multiple definitions of the AdGetArg functions in different
**	.obj files.
*/

template<class T> GsStatus AdReturn( SLANG_RET& Ret, T Val );



/****************************************************************
**	Routine: AdGetArgVector<T>
**	Returns: GsStatus
**	Action : template for getting a vector
****************************************************************/

template<class T>
GsStatus AdGetArgVector(
	const char*		Name,
	DT_VALUE&		ArgVal,
	AutoArgRef<T>&	Value
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( ArgVal.DataType == DtGsDt )
	{
		GsDt       const* const Obj = (GsDt *) ArgVal.Data.Pointer;
		MyGsDtType const* const Val = MyGsDtType::Cast( Obj );
		if( !Val )
			return GsErrMore( "%s: Expected %s, not %s",
							  Name, 
							  MyGsDtType::staticType().name().c_str(),
							  Obj->typeName() );
		Value.cPtrSet( &Val->data() );
		return GS_OK;
	}

	if( ArgVal.DataType == DtArray )
	{
		DT_ARRAY* const Arr = (DT_ARRAY*) ArgVal.Data.Pointer;
		T*        const dv = new T(Arr->Size);
		for( int i = 0; i < Arr->Size; ++i )
		{
			GsStatus const status = AdGetArgElem(Arr->Element[ i ], (*dv)[i]);
			if (status != GS_OK)
			{
				delete dv;
				return GsErrMore( "%s: Array element %d of the wrong type.",
								  Name, i);
			}
		}
		Value.ptrSet(dv);
		return GS_OK;
	}

	if( ArgVal.DataType == DtVector )
	{
		DT_VECTOR 
				*Vec = (DT_VECTOR *)ArgVal.Data.Pointer;

		T	*dv = new T(Vec->Size);
		
		for( int i = 0; i < Vec->Size; i++ )
		{
			// This is a cheezy hack to get around the fact that we
			// can't necessarily directly assign the doubles we find in  
			// vectors to elements of the result.  We have to go through
			// the AdGetArgElem mechanism to do type checking.

			DT_VALUE	
					Element;
			
			DT_NUMBER_TO_VALUE( &Element, Vec->Elem[ i ], DtDouble );

			GsStatus status = AdGetArgElem( Element, (*dv)[i] );
			if (status != GS_OK)
			{
				delete dv;
				return GsErrMore( "%s: Array element %d of the wrong type.",
								  Name, i);
			}
		}
		Value.ptrSet(dv);
		return GS_OK;		
	}

	if( ArgVal.DataType == DtGsNodeValues )
	{
		GsNodeValues* const NodeValues = static_cast< GsNodeValues* >( const_cast< void *>( ArgVal.Data.Pointer ) );

		T* const dv = new T( NodeValues->size() );

		for( int i = 0; i < NodeValues->size(); ++i )
		{
			GsStatus const status = AdGetArgElem( *(NodeValues->operator[]( i )), (*dv)[i] );
			if( status != GS_OK )
			{
				delete dv;
				return GsErrMore( "%s: Array element %d of the wrong type.",
								  Name, i );
			}
		}
		Value.ptrSet(dv);
		return GS_OK;
	}

	return GsErrMore( "%s: Expected GsDt/%s, not Dt/%s", 
					  Name, 
					  MyGsDtType::staticType().name().c_str(),
					  ArgVal.DataType->Name );
}



/****************************************************************
**	Routine: AdGetArgEnum
**	Returns: GS_OK	- arg gotten OK
**			 GS_ERR	- error getting arg
**	Action : Parse an ENUM from an DT_VALUE
**			 FIX-Could range check the enum
****************************************************************/

template<class E>
GsStatus AdGetArgEnum(
	const char*		Name,	// Name of argument
	DT_VALUE&		ArgVal,	// Integer expected
	E&				Val		// Enum result goes here
)
{
	int		Tmp;
	if( !AdGetArg( Name, ArgVal, Tmp ))
		return GsErrMore( "Expected int to get enum of type %s", GsEnumAttributes<E>::enumName() );
	Val = static_cast<E>( Tmp );
	return GS_OK;
}


#if 0
/*
**	The enums we know about
*/

#define AD_ENUM(T)	\
	template<> inline GsStatus AdGetArg( const char *Name, DT_VALUE& ArgVal, T& Value )	\
	{	\
		return AdGetArgEnum( Name, #T, ArgVal, Value );	\
	}

AD_ENUM( FQ_CALIB_INSTR );
AD_ENUM( Q_OPT_STYLE );
AD_ENUM( Q_OPT_TYPE );
AD_ENUM( FQ_MEASURE );
AD_ENUM( GsInterpolation );
AD_ENUM( FQCREDIT_MODELTYPES );
#endif



/*
**	Specializations
*/

template<class T>
GsStatus AdGetArgRef(
	const char*		Name,
	DT_VALUE&		ArgVal,		// Must be DtGsDt
	AutoArgRef<T>&	Value
);

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char *Name, DT_VALUE& ArgVal, AutoArgRef<GsDt>&					Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char *Name, DT_VALUE& ArgVal, AutoArgRef<GsDtTime>&				Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char *Name, DT_VALUE& ArgVal, AutoArgRef<GsDtDictionary>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char *Name, DT_VALUE& ArgVal, AutoArgRef<GsDtFunc>&				Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char *Name, DT_VALUE& ArgVal, AutoArgRef<GsVector>&				Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsDtArray>&				Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsDateVector>&			Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsRDateVector>&			Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsDoubleVector>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsIntegerVector>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsUnsignedVector>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsStringVector>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsSymDateVector>&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsDayCountISDAVector>&	Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsDayCountVector>&	Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<GsSymbolicExpressionVector>& Value );


template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<dt_value_addin_arg>& Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgRef( const char* Name, DT_VALUE& ArgVal, AutoArgRef<dt_slang_addin_arg>& Value );


/****************************************************************
**	Routine: <class T>AdGetArgRef
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template<class T>
GsStatus AdGetArgRef(
	const char*		Name,
	DT_VALUE&		ArgVal,		// Must be DtGsDt
	AutoArgRef<T>&	Value
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( ArgVal.DataType == DtGsDt )
	{
		GsDt       const* const Obj = static_cast<GsDt const*>( ArgVal.Data.Pointer );
		MyGsDtType const* const Val = MyGsDtType::Cast( Obj );
		if( !Val )
			return GsErrMore( "%s: Expected GsDt/%s, not %s",
							  Name, MyGsDtType::staticType().name().c_str(), Obj->typeName() );
		Value.cPtrSet( &Val->data() );
		return GS_OK;
	}

	//RAA 25-9-2000 retrieve GsDt corresponding to a string handle if relevant
	if( ArgVal.DataType == DtString )
	{
	    GsDt *HandleObj = AdGetArgByHandle( Name, static_cast<const char*>( ArgVal.Data.Pointer ), GsDt::staticType() );
		if( HandleObj != NULL )
		{
		    MyGsDtType *TObj = MyGsDtType::Cast( HandleObj );
			if( !TObj )
			    return GS_ERR;

			Value.cPtrSet( &TObj->data() );
			return GS_OK;
		}
	}

	{
	    GsDt*       const Obj = GsDtFromDtValue( &ArgVal );
		MyGsDtType* const Val = Obj ? MyGsDtType::Cast( Obj ) : NULL;
		if( !Val )
		    return GsErrMore( "%s: Expected GsDt/%s, not Dt/%s",
							  Name, MyGsDtType::staticType().name().c_str(), ArgVal.DataType->Name );

		Value.ptrSet( Val->stealData() );
		delete Obj;
	}
	return GS_OK;
}

/****************************************************************
**	Routine: <class T>AdGetArgRefNonConst
**	Returns: GsStatus
**	Action : Get a Non Const GsDt Ref argument
****************************************************************/

template<class T>
GsStatus AdGetArgRefNonConst(
	const char*		Name,
	DT_VALUE&		ArgVal,		// Must be DtGsDt
	AutoArgRef<T>&	Value
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( ArgVal.DataType == DtGsDt )
	{
		GsDt *Obj = const_cast<GsDt*>( static_cast<GsDt const *>( ArgVal.Data.Pointer ) );
		MyGsDtType *Val = MyGsDtType::Cast( Obj );
		if( !Val )
			return GsErrMore( "%s: Expected GsDt/%s, not %s",
							  Name, MyGsDtType::staticType().name().c_str(), Obj->typeName() );
		Value.ptrSet( &Val->data(), FALSE );
		return GS_OK;
	}
	return GS_ERR;
}

#if 0
/****************************************************************
**	Routine: <class T>AdGetArg
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template<class T>
GsStatus AdGetArg(
	const char*		Name,
	DT_VALUE&		ArgVal,		// Must be DtGsDt
	AutoArgRef<T>&	Value
)
{
	return AdGetArgRef( Name, ArgVal, Value );
}
#endif


/*
**	AutoArgVal exists only as specializations
*/

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsDate&			Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsPeriod&			Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsString&			Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsDayCountISDA&	Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsDayCount&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsSymDate&		Value );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArg( const char* Name, DT_VALUE& ArgVal, GsRDate&			Value );



/****************************************************************
**	Routine: <class T>AdReturn
**	Returns: GsStatus
**	Action : Return a GsDt-type value
****************************************************************/

template<class T>
inline GsStatus AdReturn(
	SLANG_RET&	Ret,
	T			RetVal
)
{
	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtGsDt;
	Ret.Data.Value.Data.Pointer	= new typename GsDtTypeMap<T>::GsDtType( RetVal );
	return GS_OK;
}


/****************************************************************
**	Routine: <class T>AdReturnRef
**	Returns: GsStatus
**	Action : Return a GsDt-type value
****************************************************************/

template<class T>
inline GsStatus AdReturnRef(
	SLANG_RET&		Ret,
	AutoArgRef<T>&	RetVal
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( !RetVal.ptr() )
		return GsErrMore( "NULL %s value returned",
				MyGsDtType::staticType().name().c_str() );

	Ret.Type					= SLANG_TYPE_VALUE;
	Ret.Data.Value.DataType		= DtGsDt;
	Ret.Data.Value.Data.Pointer	= new MyGsDtType( RetVal.ptr(), GS_NO_COPY );
	RetVal.deleteSet( FALSE );
	return GS_OK;
}


template <class T>
GsStatus AdGetArgExpand(
	const char		*Name,
	DT_VALUE		&ArgVal,
	AdExpandArg<T>	&Value,
	AdExpandData	&ExpandData
);

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdGetArgExpand( const char* Name, DT_VALUE& ArgVal, AdExpandArg<double>& Value,
						  AdExpandData& ExpandData );

template <class T>
GsStatus AdGetArgExpand(
	const char		*Name,
	DT_VALUE		&ArgVal,
	AdExpandArg<T>	&Value,
	AdExpandData	&ExpandData
)
{
	typedef typename AdExpandArg<T>::timeseries_type	MyTsType;
	typedef typename AdExpandArg<T>::vector_type		MyVecType;

	GsStatus 	Status;

	AutoArgRef< MyTsType > TS;
	if( ( Status = AdGetArgRef( Name, ArgVal, TS ) ) == GS_OK ) 
	{
		Value = *( TS.cPtr() );
		ExpandData.insert( &Value, TS.cPtr()->dates() );
	}
	else
	{
		AutoArgRef< MyVecType > 
				Vec;
		if( ( Status = AdGetArgRef( Name, ArgVal, Vec ) ) == GS_OK ) 
		{
			Value = *( Vec.cPtr() );
			ExpandData.insert( &Value );
		}
		else
		{
			T	Temp;
			if( ( Status = AdGetArg( Name, ArgVal, Temp ) ) == GS_OK )
			{
				Value = Temp;
				ExpandData.insert( &Value );
			}
		}
	}		

	return Status;
}

template <class T>
GsStatus AdGetArgExpand(
	const char*					Name,
	DT_VALUE&					ArgVal,
	AdExpandAutoArgRef< T >&	Value,
	AdExpandData& 				ExpandData	
)
{
	typedef typename AdExpandAutoArgRef<T>::timeseries_type MyTsType;
	typedef typename AdExpandAutoArgRef<T>::vector_type MyVecType;

	GsStatus Status;

	AutoArgRef< MyTsType > 
			TS;

	Status = AdGetArgRef( Name, ArgVal, TS );
	if( Status == GS_OK ) 
	{
		Value = *( TS.cPtr() );
		ExpandData.insert( &Value, TS.cPtr()->dates() );
	}
	else
	{
		AutoArgRef< GsDtArray >
				Array;
		if( ( Status = AdGetArgRef(Name, ArgVal, Array ) ) == GS_OK )
		{
			MyVecType Vec;
			if( ( Status = fromGsDtArrayToVector(*(Array.cPtr()), Vec, (T*)NULL) ) == GS_OK ) 
			{
				Value = Vec;
				ExpandData.insert( &Value );
			}
		}
		else
		{
			AutoArgRef< MyVecType > Vec;
			if( ( Status = AdGetArgRef(Name, ArgVal, Vec ) ) == GS_OK )
			{	
				Value = *( Vec.cPtr() );
				ExpandData.insert( &Value );
			}
			else
			{
				typename AdExpandAutoArgRef<T>::value_type
						Temp;
				if( ( Status = AdGetArgRef( Name, ArgVal, Temp ) ) == GS_OK )
				{
					Value = Temp;
					ExpandData.insert( &Value );
				}
			}
		}
	}	

	return Status;
}


/*
**	Specializations
*/

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<GsDt>&		 	 RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<GsDtArray>&		 RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<GsDtDictionary>& RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<GsDtFunc>& 		 RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<dt_value_addin_arg>& RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturnRef( SLANG_RET& Ret, AutoArgRef<dt_slang_addin_arg>& RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturn( SLANG_RET& Ret, GsDate	RetVal );

template<>
EXPORT_CPP_FCADLIBS GsStatus
		AdReturn( SLANG_RET& Ret, GsString	RetVal );


// If only we had partial template specialization we wouldn't have to do this :-(
// sigh. I'm a lot more pissed than I sound BTW.

/****************************************************************
**	Routine: <class T>AdGetTypedVectorArgRef
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template<class T>
GsStatus AdGetTypedVectorArgRef(
	const char*								Name,
	DT_VALUE&								ArgVal,		// Must be DtGsDt
	AutoArgRef< GsTypedVector< T > >&	Value
)
{
	typedef typename GsDtTypeMap<T>::GsDtType MyGsDtType;

	if( ArgVal.DataType == DtGsDt )
	{
		GsDt      const* const Obj = static_cast<GsDt const*>( ArgVal.Data.Pointer );
		GsDtArray const* const ArrayVal = GsDtArray::Cast( Obj );
		if( !ArrayVal )
			return GsErrMore( "%s: Expected GsDtArray, not %s",
							  Name, Obj->typeName() );

		Value.ptrSet( static_cast< GsTypedVector< T const >* >(
			new GsDtTypedVector< T const, GsDt const, GsDtArray const >
			(
				MyGsDtType::staticType(),
				*ArrayVal
			) 
		) );
		return GS_OK;
	}
	else
		return GsErrMore( "%s: Expected GsDtArray Typed on %s, not Dt/%s",
						  Name, MyGsDtType::staticType().name().c_str(), ArgVal.DataType->Name );

	return GS_OK;
}

CC_END_NAMESPACE

#endif

