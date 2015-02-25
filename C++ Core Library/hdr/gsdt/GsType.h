/****************************************************************
**
**	gsdt/GsType.h - type info class for all GsDt classes
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsType.h,v 1.6 2001/11/27 22:45:03 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSTYPE_H )
#define IN_GSDT_GSTYPE_H

#include <gsdt/base.h>
#include <gsdt/dttypes.h>
#include <gscore/GsException.h>
#include <new>

CC_BEGIN_NAMESPACE( Gs )


/*
**	GsDtInitialize functions must be of this type
*/

typedef void (GsDtInitializeFunc)( void );

/*
**	Datatype flags
**	All 'started' values are non-zero
*/

enum GsTypeFlags
{
	GSTF_NOT_STARTED= 0x00000000,
	GSTF_PLAIN		= 0x10000000,
	GSTF_NUMERIC	= 0x10000001,
	GSTF_POINTER	= 0x10000002,
	GSTF_REFERENCE	= 0x10000004,
	GSTF_ITERABLE	= 0x10000008
};


/*
**	GsType is the TypeInfo class for all GsDt objects
**
**	Nothing should inherit from GsType.
*/

class EXPORT_CLASS_GSDTLITE GsType
{
private:
	// All constructors/destructors private to prevent inheritance and undesired instantiation
	GsType( GsType **Prev, const char *Name, GsTypeId Id, int Flags, int CompVal );
	GsType( const GsType &NotImplemented );
	void operator=( const GsType &NotImplemented );
	~GsType();
	friend void BogusFriendToShushGnu2_95CompilerWarning();
public:

	bool operator==( const GsType &Rhs ) const { return this == &Rhs; }
	bool operator!=( const GsType &Rhs ) const { return this != &Rhs; }

	// Delegate some functions to the m_emptyInstance object
	GsDt	*highLimit()	 const;
	GsDt	*lowLimit()		 const;
	GsDt	*errorSentinel() const;
	GsDt	*emptyInstance() const;

	const GsDt		*defaultEmptyInstance() const { return m_emptyInstance; }

	const GsString&	name() const			{ return m_Name; }
	int				CompareValue() const 	{ return m_CompareValue; }
	GsTypeId 		Id() const		 		{ return m_Id; }

	// Various flag operations
	GsBool	 		IsNumeric() const		{ return m_Flags & GSTF_NUMERIC;	}
	GsBool			IsReference() const    	{ return m_Flags & GSTF_REFERENCE;	}
	GsBool			IsIterable() const		{ return m_Flags & GSTF_ITERABLE;	}

	// Binary operator lookup
	void			BinOpRegister(		GsDtuBinaryFunc *BinOpFunc );
    GsDtuBinaryFunc	*BinOpLookup(		const GsType& TypeOther ) const;

	// Random construction operations
	void			ConvertRegister(	GsDtuConvFunc *ConvFunc );
	GsDtuConvFunc	*ConvertLookup(		const GsType& TypeOther ) const;
	GsDt			*ConvertFrom(		const GsDt &Other ) const;

public:
	static void		RegisterDatatypes();
	static void		RegisterDat( const char *Name, GsTypeId Id, const char *Dll );
	static void		Register( const char *Name, GsTypeId Id, int Flags, int CompVal, GsDt *EmptyInstance );
	static GsType	*Lookup(			const char *Name ) { return LookupPriv( Name ); }
	static GsType	*LookupOrDie(		const char *Name );
	static GsType	*LookupOrCreate(	const char *Name );
	static GsType	*Lookup(			GsTypeId Id 	 ) { return LookupPriv( Id ); }
	static GsDt		*getTypes();

	// Attach an instance of the appropriate type
	void			attach( GsDt *Empty );

	static const GsType	*getFirst() { return allTypes(); }
	const GsType		*getNext() const { return m_Next; }

/*
**	Implementation
*/

private:
	class EXPORT_CLASS_GSDTLITE Dll
	{
	public:
		GsString	m_Name;
		bool		m_Started;
		Dll();
		void startup();
		static Dll* lookupOrCreate( const char* Name );
	};

	// These items loaded from GsDtType.dat
	GsTypeId		m_Id;				// Unique id for this datatype
	GsString		m_Name;				// The name of the type
	Dll				*m_Dll;				// The object representing the DLL for this datatype

	// These are setup by the code
	int				m_Flags;			// Numeric/Reference/Iterable/...
	int				m_CompareValue;		// Used to order among different types
	GsDt			*m_emptyInstance;

	// Managed by the class
	bool			m_Started;			// has startup() been called yet?

	// Call this to initialize the type
	void startup();

	static GsType 	*allTypes();

/*
**	Lists of binary operators and type conversion functions
**	FIX-Should be maps or hash tables
*/

private:
	GsDtuBinaryFunc	*m_BinOps;

	GsDtuConvFunc	*m_Converts;

private:
	GsType			*m_Next;			// Next in list

	// FIX - Need hash tables for both name and Id lookup.  Current
	// linked list approach is temporary.

	static GsType	*LookupPriv( const char *Name, bool SetErr = true, bool CallStartup = true );
	static GsType	*LookupPriv( GsTypeId Id, 	   bool SetErr = true, bool CallStartup = true );
};

CC_END_NAMESPACE

#endif
