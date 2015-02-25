/****************************************************************
**
**	fcadlib/adexcel.h	- Adlib functions and macros for Excel
**
**	Included from <fcadlib/adlib.h>
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adexcel.h,v 1.84 2012/06/28 15:46:36 e19351 Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADEXCEL_H )
#define IN_FCADLIB_ADEXCEL_H

#include <fcadlib/base.h>
#include <exception>
#include <date.h>
#include <fcadlib/xlcall.h>
#include <fcadlib/gsexcel.h>
#include <fcadlib/autoarg.h>
#include <fcadlib/AdExpandArg.h>
#include <fcadlib/AdExpandData.h>
#include <fcadlib/adexcelsup.h>
#include <gsdt/GsType.h>
#include <gscore/GsException.h>
#include <gscore/gsdt_fwd.h>
#include <gsdt/GsDtTypeMap.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Excel
*/

// TODO: The functions in this file were mostly defined as some mash up of
// templates and concrete functions. I've assumed that the functions that at
// all resembled a template declaration were templates, and the rest were not,
// but there's no way to know the original author's intent (if they knew which
// they wanted at all). Sort through this mess at some point.

#define AD_API				LPXLOPER
#define AD_API_EXP			DLLEXPORT LPXLOPER
#undef	AD_API_ARGS
#define AD_ARG_COUNT		NumberOfArgs
#define AD_ARG(_Ct)			*e##_Ct
#define AD_ARGS(_Nm,_Ct)	_Nm, AD_ARG(_Ct)
#define AD_RET_ARGS			*XlRetVal

#define AD_FUNC_ENTER(_Name_)	\
	Gs::AdReportExcelUsage( _Name_ ); \
	const char *CurrentFunc;	\
	try		\
	{		\
		CurrentFunc = _Name_

// Now defers to AD_FUNC_FAIL_WITH_LOG, logging disabled
#define AD_FUNC_FAIL(_Name_)	\
		AdXlFuncFail(_Name_, *XlRetVal );	\
	}										\
	catch( GsException& GsE )	{ AdXlHandleGsException(	  CurrentFunc, *XlRetVal, GsE ); }	\
	catch( std::exception& E )	{ AdXlHandleException(		  CurrentFunc, *XlRetVal, E );	 }	\
	return XlRetVal;

// Ensures the error message gets output to the log file
#define AD_FUNC_FAIL_WITH_LOG(_Name_)	\
		AdXlFuncFail(_Name_, *XlRetVal );	\
	}										\
	catch( GsException& GsE )	{ AdXlHandleGsException(	  CurrentFunc, *XlRetVal, GsE ); }	\
	catch( std::exception& E )	{ AdXlHandleException(		  CurrentFunc, *XlRetVal, E );	 }	\
	AdXLLogFunctionTraceResult( XlRetVal );	\
	return XlRetVal;

#define AD_FUNC_OK(_Name_)		\
{								\
	return XlRetVal;			\
}

#define AD_FUNC_ENTER_NO_ARG(_Name_)	AD_FUNC_ENTER(_Name_)
#define AD_FUNC_FAIL_NO_ARG(_Name_)		AD_FUNC_FAIL(_Name_)
#define AD_FUNC_FAIL_WITH_LOG_NO_ARG(_Name_)		AD_FUNC_FAIL_WITH_LOG(_Name_)
#define AD_FUNC_OK_NO_ARG(_Name_)		AD_FUNC_OK(_Name_)

extern EXPORT_CPP_FCADLIBX const int
		AdXlDateOffset;

EXPORT_CPP_FCADLIBX void AdReportExcelUsage( const char* functionName );
		
EXPORT_CPP_FCADLIBX bool 
		AdIsArgMissing( XLOPER& Op );

inline	GsStatus
		AdGetArgCheck(			const char *FuncName, int Need, int Got );

EXPORT_CPP_FCADLIBX void
		AdXlFuncFail(			const char *FuncName, XLOPER &RetVal );

EXPORT_CPP_FCADLIBX void
		AdXlHandleException(		const char *FuncName, XLOPER &Val, std::exception &E ) CC_NOTHROW,
		AdXlHandleGsException(		const char *FuncName, XLOPER &Val, GsException &E ) CC_NOTHROW,
		AdXlHandleUnknownException(	const char *FuncName, XLOPER &Val ) CC_NOTHROW;

EXPORT_CPP_FCADLIBX void
		AdXlPopupErrorsSet( GsBool Enabled ),
		AdXlPopupErrorsToggle(),
		AdXlVersionDisplay(),
		AdXlFormatHandles();

template<class T> GsStatus AdGetArg( const char *Name, const XLOPER& ArgVal, T& Value );

template<> EXPORT_CPP_FCADLIBX GsStatus AdGetArg<bool>( const char *Name, const XLOPER &ArgVal, bool &Value );
template<> EXPORT_CPP_FCADLIBX GsStatus	AdGetArg<DATE>( const char *Name, const XLOPER &ArgVal, DATE &Value );
template<> EXPORT_CPP_FCADLIBX GsStatus AdGetArg<double>( const char *Name, const XLOPER &ArgVal, double &Value );
template<> EXPORT_CPP_FCADLIBX GsStatus AdGetArg<int>( const char *Name, const XLOPER &ArgVal, int &Value );
template<> EXPORT_CPP_FCADLIBX GsStatus AdGetArg<unsigned>( const char *Name, const XLOPER &ArgVal, unsigned &Value );

EXPORT_CPP_FCADLIBX GsDt*
		AdGetArgByHandle(		const char *Name, const XLOPER &ArgVal, const GsType& Type );

EXPORT_CPP_FCADLIBX GsStatus
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsDate			&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsPeriod			&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsRDate			&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsString			&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsSymbol			&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsDayCountISDA	&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsDayCount		&Value ),
		AdGetArg( const char *Name, const XLOPER &ArgVal, GsSymDate			&Value );

EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgEnumSup( const char* ArgName, const XLOPER& Arg, const char* EnumName, int& Value );



/****************************************************************
**	Routine: <class T>AdGetArg
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template<class T>
GsStatus AdGetArg(
	const char*		Name,
	const XLOPER&	ArgVal,
	T&				Value
)
{
	GsDt *GObj = AdGetArgByHandle( Name, ArgVal, GsDtTypeMap< T >::GsDtType::staticType() );
	if( !GObj )
		return GS_ERR;
	GsDtTypeMap< T >::GsDtType *Obj = GsDtTypeMap< T >::GsDtType::Cast( GObj );
	if( !Obj )
		return GS_ERR;
	Value = Obj->data();
	return GS_OK;
}



/****************************************************************
**	Routine: AdGetArgEnum
**	Returns: GS_OK	- arg gotten OK
**			 GS_ERR	- error getting arg
**	Action : Parse an ENUM from an XLOPER
****************************************************************/

template<class E>
GsStatus AdGetArgEnum(
	const char*			Name,
	const XLOPER&		Arg,
	E&					Val
)
{
	int		Tmp;
	const char *EnumName = GsEnumAttributes<E>::enumName();

	if( !AdGetArgEnumSup( Name, Arg, EnumName, Tmp ))
		return GS_ERR;
	Val = static_cast<E>(Tmp);
	return GS_OK;
}



/****************************************************************
**	Routine: <class T>AdGetArg
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template<class T>
GsStatus AdGetArgRef(
	const char*		Name,
	const XLOPER&	ArgVal,
	AutoArgRef<T>&	Value
)
{
	GsDt *GObj = AdGetArgByHandle( Name, ArgVal, GsDtTypeMap< T >::GsDtType::staticType() );
	if( !GObj )
		return GS_ERR;
	GsDtTypeMap< T >::GsDtType *Obj = GsDtTypeMap< T >::GsDtType::Cast( GObj );
	if( !Obj )
		return GS_ERR;
	Value.cPtrSet( &Obj->data() );
	return GS_OK;
}

// Specializations of the above template function
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsDt> &Value );
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsDtTime> &Value );
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsMatrix> &Value );
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDoubleTimeSeries> &Value);

// HACK: This is unacceptable on so many levels. This is not a proper use of include guards, and there's
// no comment to explain this egregious violation of readability.
#ifdef IN_GSDT_GSDTDICTIONARY_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsDtDictionary> &Value );
#endif
#ifdef IN_GSCORE_VECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsVector> &Value );
#endif
#ifdef IN_GSCORE_GSRDATEVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char *Name, const XLOPER &ArgVal, AutoArgRef<GsRDateVector> &Value );
#endif
#ifdef IN_GSCORE_GSDATEVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDateVector>&			Val );
#endif
#ifdef IN_GSCORE_GSDOUBLEVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDoubleVector>&			Val );
#endif
#ifdef IN_GSDT_GSDTARRAY_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDtArray>&				Val );
#endif
#ifdef IN_GSCORE_GSINTEGERVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsIntegerVector>&			Val );
#endif
#ifdef IN_GSCORE_GSUNSIGNEDVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsUnsignedVector>&		Val );
#endif
#ifdef IN_GSCORE_GSSTRINGVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsStringVector>&			Val );
#endif
#ifdef IN_GSCORE_GSSYMDATEVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsSymDateVector>&			Val );
#endif
#ifdef IN_GSCORE_GSDAYCOUNTISDAVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDayCountISDAVector>&	Val );
#endif
#ifdef IN_GSCORE_GSDAYCOUNTVECTOR_H
EXPORT_CPP_FCADLIBX GsStatus
		AdGetArgRef( const char* Name, const XLOPER& xArg, AutoArgRef<GsDayCountVector>&		Val );
#endif


template <class T>
GsStatus AdGetArgExpand(
	const char*		Name,
	const XLOPER&	ArgVal,
	AdExpandArg<T>&	Value,
    AdExpandData&	ExpandData
)
{
	GsStatus Status;

	AutoArgRef< AdExpandArg<T>::timeseries_type > 
			TS;

	Status = AdGetArgRef( Name, ArgVal, TS );
	if( Status )
	{
		Value = *( TS.cPtr() );
		ExpandData.insert( &Value, TS.cPtr()->dates() );
	}
	else
	{
		AutoArgRef< AdExpandArg<T>::vector_type > 
				Vec;
		Status = AdGetArgRef(Name, ArgVal, Vec );

		if( Status ) 
		{
			if (Vec.cPtr()->size() == 1)
				Value = ( *( Vec.cPtr() ) )[0];
			else
			{
				Value = *( Vec.cPtr() );
				ExpandData.insert( &Value );
			}
		}
		else
		{
			T	Temp;
			Status = AdGetArg<T>( Name, ArgVal, Temp );
			if( Status ) 
			{
				Value = Temp;
				ExpandData.insert( &Value );
			}
		}
	}

	return Status;
}


/****************************************************************
**	Routine: <class T>AdGetArg
**	Returns: GsStatus
**	Action : Get a GsDt-type argument
****************************************************************/

template <class T>
GsStatus AdGetArgExpand(
	const char*					Name,
	const XLOPER&				ArgVal,
	AdExpandAutoArgRef< T >&	Value,
	AdExpandData& 				ExpandData	
)
{
	GsStatus Status;

	AutoArgRef< AdExpandAutoArgRef<T>::timeseries_type > 
			TS;

	if ( ( Status = AdGetArgRef( Name, ArgVal, TS ) ) == GS_OK ) 
	{
		Value = *( TS.cPtr() );
		ExpandData.insert( &Value, TS.cPtr()->dates() );
	}
	else
	{
		typedef AdExpandAutoArgRef<T>::vector_type vector_type;

		AutoArgRef <GsDtArray>
				Array;
		if ( ( Status = AdGetArgRef<GsDtArray>( Name, ArgVal, Array ) ) == GS_OK )
		{
			AdExpandAutoArgRef<T>::vector_type
					Vec;

			Status = fromGsDtArrayToVector<T, vector_type>(*(Array.cPtr()), Vec, (T*) NULL );
			if ( GS_OK == Status )
			{
				if( 1 == Vec.size() )
					Value = AutoArgRef< T >( new T( *( Vec[0].getPtr() ) ) );
				else
				{
					Value = Vec;
					ExpandData.insert( &Value );
				}
			}
		}
		else
		{
			AutoArgRef <vector_type>
					Vec;

			if( GS_OK == ( Status = AdGetArgRef<vector_type>(Name, ArgVal, Vec ) ) )
			{	
				if( 1 == Vec.cPtr()->size() )
					Value = ( Vec.cPtr() )[0];
				else
				{
					Value = *( Vec.cPtr() );
					ExpandData.insert( &Value );
				}
			}
			else
			{
				AdExpandAutoArgRef<T>::value_type
						Temp;

				if ( ( Status = AdGetArgRef( Name, ArgVal, Temp ) ) == GS_OK )
				{
					Value = Temp;
					ExpandData.insert( &Value );
				}
			}
		}
	}	

	return Status;
}

EXPORT_CPP_FCADLIBX GsStatus
		AdReturn( XLOPER &Ret, GsDate			RetVal ),
		AdReturn( XLOPER &Ret, GsCalendar		RetVal ),
		AdReturn( XLOPER &Ret, GsPeriod			RetVal ),
		AdReturn( XLOPER &Ret, GsRDate			RetVal ),
		AdReturn( XLOPER &Ret, GsDayCountISDA	RetVal ),
		AdReturn( XLOPER &Ret, GsDayCount		RetVal ),
		AdReturn( XLOPER &Ret, GsString			RetVal ),
		AdReturn( XLOPER &Ret, GsSymbol			RetVal );



/****************************************************************
**	Routine: <class T>AdReturnRef
**	Returns: GsStatus
**	Action : Return a GsDt-type value
****************************************************************/

template<class T>
GsStatus AdReturnRef(
	XLOPER			&Ret,
	AutoArgRef<T>	&RetVal
)
{
	if( RetVal.cPtr() == NULL )
		return GS_ERR;

	// RAA 13-9-2000 need to get the calling cell from inside fcadlibx
	char	callingCell[256];
	AdXlCallingCell( callingCell, sizeof( callingCell ), 0, 0 );
	ObjHandle	*Handle = ObjHandle::create( callingCell );

	GsDtTypeMap< T >::GsDtType
			*Obj = NULL;

	if( RetVal.ptr() )
	{
		Obj = new GsDtTypeMap< T >::GsDtType( RetVal.ptr(), GS_NO_COPY );
		RetVal.deleteSet( FALSE );
	}
	else
		Obj = new GsDtTypeMap< T >::GsDtType( *RetVal.cPtr() );
	Handle->objSet( Obj );

	GsXlOperAllocStr( Ret, Handle->name().c_str() );

	return GS_OK;
}


// Specializations of the above template function
	
EXPORT_CPP_FCADLIBX GsStatus
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsDt>&					RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsDateVector>&			RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsDtArray>&			RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsDtDictionary>&		RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsIntegerVector>&		RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsUnsignedVector>&		RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsMatrix>&				RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsStringVector>&		RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsVector>&				RetVal ),
		AdReturnRef( XLOPER &Ret, AutoArgRef<GsDoubleTimeSeries>&	RetVal );


EXPORT_CPP_FCADLIBX GsStatus
		AdReturnErr(			void ),
		AdReturnNA(				void );

template<class T> GsStatus AdReturn( XLOPER& Ret, T Val );

template<> EXPORT_CPP_FCADLIBX GsStatus AdReturn<DATE>( XLOPER &Ret, DATE RetVal );
template<> EXPORT_CPP_FCADLIBX GsStatus AdReturn<double>( XLOPER &Ret, double RetVal );

template<> inline GsStatus AdReturn<bool>( XLOPER &Ret, bool RetVal )
{
	return static_cast< GsStatus >( AdReturn( Ret, (double) RetVal ));
}

template<> inline GsStatus AdReturn<int>( XLOPER &Ret, int RetVal )
{
	return static_cast< GsStatus >( AdReturn( Ret, (double) RetVal ));
}

template<> inline GsStatus AdReturn<unsigned>( XLOPER &Ret, unsigned RetVal )
{
	return static_cast< GsStatus >( AdReturn( Ret, (double) RetVal ));
}



/****************************************************************
**	Routine: AdGetArgCheck
**	Returns: GS_OK
**	Action : Ignored for Excel platform
****************************************************************/

inline GsStatus AdGetArgCheck(
	const char	*	,	// All are ignored
	int				,
	int
)
{
	/*
	** The default arg logic assumes this return GS_OK. If you change this function make sure you do not
	** break the default arg code for EXCEL addins.
	*/
	return GS_OK;
}

/// This is used by adexcelret.cpp and xlmain.cpp to notify user of bad array size.
struct AdResizeRequest
{
	/// Id that follows cell across moves/renames/etc, used internally.
	size_t TrackId;

	/// Current row and column.
	size_t Row, Column;
	/// Current sheet ID.
	size_t SheetId;

	/// Previous dimensions.
	size_t OldHeight, OldWidth;
	/// New requested dimensions.
	size_t Height, Width;
	/// Current array formula.
	std::string ArrayFormula;
};
/// Called to report that an array has the wrong size.
void AdReportResizeRequest(AdResizeRequest req);

/*
**	Declarations used by fcadlib/adxlmain.h
*/

// What FuncTable is made of
struct XlFuncEntry
{
	char	*DllFuncName;	// Name of function in DLL (leading @ means command)
	char	*Types;			// Function Return and argument types
	char	*FuncName;		// Name of function displayed in Excel
	char	*Args;			// Names of arguments (comma sep'd list)
	char	*FuncDesc;		// Help text for function
	char	*ArgsDesc;		// Help text for each argument ('\0' sep'd list, double '\0' terminates)
};

// What XlMenu is made of
struct XlMenuEntry
{
	char	*ItemName;		// Name of Menu or Item
	char	*FuncName;		// Name of function to call
	char	*ItemDesc;		// Help text for item/menu
};

EXPORT_CPP_FCADLIBX int
		AdXRegisterFunc(
			const XlFuncEntry *Func,
			XLOPER &xRegId,
			const char	*DllName,
			const char	*CategoryName
		);

EXPORT_CPP_FCADLIBX void
		AdXlAddMenuCommands( const XlMenuEntry *MenuTable );

EXPORT_CPP_FCADLIBX int
		AdXAutoOpen( XlFuncEntry *FuncTable, XlMenuEntry *MenuTable,
					 const char *DllName,
					 const char *CatName, const char *LibName,
                     const char *DateTime, char *VersionString );

EXPORT_CPP_FCADLIBX LPXLOPER
		AdXAddInManagerInfo( LPXLOPER xAction, const char *LongName );


EXPORT_C_FCADLIBX void
		AdXOverrideRegistration(
			const char	*CategoryName = NULL);// (optional) names of specified categories


CC_END_NAMESPACE

#endif

