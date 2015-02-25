/* $Header: /home/cvs/src/exslang/src/dtole.h,v 1.8 2012/04/18 18:37:56 e19351 Exp $ */
/****************************************************************
**
**	dtole.h	- Ole Datatype headers
**
**	Copyright 1997 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.8 $
**
****************************************************************/

#if !defined( IN_DTOLE_H )
#define IN_DTOLE_H

#include	<portable.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<err.h>
#include	<datatype.h>

#ifndef _INC_WINDOWS
#include	<windows.h>
#endif
#ifndef _OLE2_H_
#include	<ole2.h>
#endif


/*
**	OLE datatypes are IDispatch interface pointers
*/

class EXPORT_CLASS_EXPORT DT_OLE_METHOD;
class EXPORT_CLASS_EXPORT DT_OLE_TYPE_INFO;
class EXPORT_CLASS_EXPORT DT_OLE_TYPE_LIB;
class EXPORT_CLASS_EXPORT DT_OLE;

class DT_OLE_METHOD
{
	DISPID	m_MethodId;		// Dispatch ID of bound method

	IDispatch
			*m_Dispatch;	// Ole object this method is for

	int		m_RefCount;
	
	bool		m_ReturnsTimes; // Whether to convert OLE variants to times instead of dates

public:
	DT_OLE_METHOD( IDispatch *Disp, const char *MethodName, bool ReturnTimes );
	DT_OLE_METHOD( IDispatch *Disp, DISPID DispId, bool ReturnTimes );
	~DT_OLE_METHOD()
	{
		if( m_Dispatch )
			m_Dispatch->Release();
	}
	void AddRef()
	{
		m_RefCount++;
	}
	void Release()
	{
		if( --m_RefCount <= 0 )
			delete this;
	}
	int operator!()
	{
		return m_MethodId == -1;
	}
	int operator==( const DT_OLE_METHOD &O )
	{
		return m_MethodId == O.m_MethodId && m_Dispatch == O.m_Dispatch;
	}
	char *Name( char *Buffer )
	{
		sprintf( Buffer, "Ole Method: %08lx/%ld Rc:%d", (long) m_Dispatch, (long) m_MethodId, m_RefCount );
		return Buffer;
	}
	int Invoke( DT_VALUE *Result, DT_VALUE *Params );
};


/*
**	I know this is horrible, but we couldn't get it to work any other way...
*/

#define DT_OLE_METHOD_NOT_INITIALIZED ((DT_OLE_METHOD *) -1)

class DT_OLE
{
	IDispatch
			*m_Dispatch;

	ITypeInfo
			*m_TypeInfo;

	ITypeComp
			*m_TypeComp;

	int		m_RefCount;

	DT_OLE_METHOD
			*m_Default;

	int		InitTypes( void );

	void		InitDefault();
	
	bool		m_ReturnsTimes;

public:
	DT_OLE( IDispatch *Disp, bool ReturnTimes )
	{
		m_Dispatch = Disp;
		m_RefCount = 1;
		m_TypeInfo = NULL;
		m_TypeComp = NULL;
		m_Default  = DT_OLE_METHOD_NOT_INITIALIZED;
		m_ReturnsTimes = ReturnTimes;
	}
	~DT_OLE()
	{
		if( m_Dispatch )
			m_Dispatch->Release();
		if( m_TypeInfo )
			m_TypeInfo->Release();
		if( m_TypeComp )
			m_TypeComp->Release();
		if( m_Default && m_Default != DT_OLE_METHOD_NOT_INITIALIZED )
			m_Default->Release();
	}
	void AddRef()
	{
		m_RefCount++;
	}
	void Release()
	{
		if( --m_RefCount <= 0 )
			delete this;
	}
	char *Name( char *Buf )
	{
		sprintf( Buf, "Ole Object: %08lx Rc:%d Ti:%08lx Tc:%08lx Dflt:%08lx",
                 (long) m_Dispatch, m_RefCount, (long) m_TypeInfo, (long) m_TypeComp,
                 (long) m_Default );
		return Buf;
	}

	int operator!()
	{
		return !m_Dispatch;
	}
	int PropertyGet( const char *Property, DT_VALUE *Result );
	int PropertySet( const char *Property, DT_VALUE *Value  );
	int PropertyList( DT_VALUE *Result );
	int PropertyInfo( const char *Property, DT_VALUE *Result );
	void ToVariant( VARIANT *Var )
	{
		Var->vt		  = VT_DISPATCH;
		Var->pdispVal = m_Dispatch;
		m_Dispatch->AddRef();
	}
	int Invoke( DT_VALUE *Result, DT_VALUE *Params )
	{
		if( m_Default == DT_OLE_METHOD_NOT_INITIALIZED )
			InitDefault();
		if( !m_Default )
			return Err( ERR_UNKNOWN, "Can't [[ ]], no default method" );
		else
			return m_Default->Invoke( Result, Params );
	}
	int	IsBound( const char *Method, DISPID *DispId );
	DT_OLE_METHOD	*Method( const char *Name )
	{
		return new DT_OLE_METHOD( m_Dispatch, Name, m_ReturnsTimes ); 
	}
	ITypeLib	*GetContainingTypeLib();
};


class DT_OLE_TYPE_LIB
{
	ITypeLib
			*m_TypeLib;

	int		m_RefCount;

public:
	DT_OLE_TYPE_LIB( ITypeLib *TypeLib )
	{
		m_TypeLib = TypeLib;
		m_RefCount = 1;
	}
	~DT_OLE_TYPE_LIB()
	{
		if( m_TypeLib )
			m_TypeLib->Release();
	}
	void AddRef()
	{
		m_RefCount++;
	}
	void Release()
	{
		if( --m_RefCount <= 0 )
			delete this;
	}
	char *Name( char *Buf )
	{
		sprintf( Buf, "Ole TypeLib: %08lx Ref:%d", (long) m_TypeLib, m_RefCount );
		return Buf;
	}

	int operator!()
	{
		return !m_TypeLib;
	}

	int					TypeInfoList( DT_VALUE *Result );
	ITypeInfo			*TypeInfoGet(   unsigned int Index ); 
	DT_OLE_TYPE_INFO	*TypeInfoGetDt( unsigned int Index ); 
};


class DT_OLE_TYPE_INFO
{
	ITypeInfo
			*m_TypeInfo;

	int		m_RefCount;

public:
	DT_OLE_TYPE_INFO( ITypeInfo *TypeInfo )
	{
		m_TypeInfo = TypeInfo;
		m_RefCount = 1;
	}
	~DT_OLE_TYPE_INFO()
	{
		if( m_TypeInfo )
			m_TypeInfo->Release();
	}
	void AddRef()
	{
		m_RefCount++;
	}
	void Release()
	{
		if( --m_RefCount <= 0 )
			delete this;
	}
	char *Name( char *Buf )
	{
		sprintf( Buf, "Ole TypeInfo: %08lx Ref:%d", (long) m_TypeInfo, m_RefCount );
		return Buf;
	}

	int operator!()
	{
		return !m_TypeInfo;
	}

	int			PropertyList( DT_VALUE *Result );
	int			PropertyInfo( const char *Property, DT_VALUE *Result );
	int			IsBound( const char *Method, DISPID *DispId );
	ITypeLib	*GetContainingTypeLib();
};


/*
**	Export functions and variables
*/

DLLEXPORT DT_DATA_TYPE
		DtOleObject,		// == DataType( "Ole Object" )
		DtOleTypeInfo,		// == DataType( "Ole Type Info" )
		DtOleTypeLib,		// == DataType( "Ole Type Lib" )
		DtOleMethod;		// == DataType( "Ole Method" )


DLLEXPORT int
		DtOleInit( void );

DLLEXPORT int
		DtOleObjectCreate(		DT_VALUE *Value, IDispatch *Disp ),
		DtOleTypeInfoCreate(	DT_VALUE *Value, ITypeInfo *TypeInfo ),
		DtOleTypeLibCreate(		DT_VALUE *Value, ITypeLib  *TypeLib ),
		DtOleMethodCreate(		DT_VALUE *Value, DT_OLE *Obj, const char *Method );

// From dtolesup.c

DLLEXPORT BSTR
		StrDupWide( const char *Str );

DLLEXPORT const char
		*OleCallConvName(	CALLCONV CallConv ),
		*OleFuncKindName(	FUNCKIND FuncKind ),
		*OleInvokeKindName(	INVOKEKIND InvokeKind ),
		*OleTypeKindName(   TYPEKIND  Kind ),
		*OleVarKindName(	VARKIND VarKind ),
		*OleVarTypeName(	VARTYPE VarType );

DLLEXPORT DISPID
		OleGetId( LPDISPATCH pDisp, const char *Name );

DLLEXPORT int
		OleDtToVar( DT_VALUE *Value, VARIANT  *Var   ),
		OleVarToDt( VARIANT  *Var,   DT_VALUE *Value,	bool OleDateTimesAsTimes );

DLLEXPORT int
		OlePropertyList( ITypeInfo *TypeInfo, DT_VALUE *Result );


/*
**	Initialization macro
*/

#define DT_OLE_INIT()	(DtOleObject || DtOleInit())
#define X_OLE_INIT()	\
	if( !DT_OLE_INIT())	\
	{					\
		ErrMore( "%s: Ole subsystem not initialized", Root->StringValue );	\
		return SlangEvalError( SLANG_ERROR_PARMS, NULL );					\
	}


/* 
**	Narrow/Wide character conversion functions
*/

#define W2N( _Narrow, _Wide, _NarrowLen )	\
	WideCharToMultiByte( CP_ACP, 0, _Wide, -1, _Narrow, _NarrowLen, NULL, NULL )
#define N2W( _Wide, _Narrow, _WideSize )	\
	MultiByteToWideChar( CP_ACP, 0, _Narrow, -1, _Wide, _WideSize / sizeof( *_Wide ))


#endif

