/****************************************************************
**
**	GsDt.h	- GsDt base class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDt.h,v 1.12 2001/11/27 22:44:58 procmon Exp $
**
****************************************************************/

#ifndef IN_GSDT_DT_H
#define IN_GSDT_DT_H

#include <gsdt/base.h>
#include <gsdt/GsType.h>
#include <gsdt/macros.h>
#include <gscore/GsException.h>
#include <gscore/err.h>
#include <gscore/GsFuncHandle.h>
#include <string>

#include <iosfwd>						// for ostream

CC_BEGIN_NAMESPACE( Gs )

class GsType;
class GsDtArray; 

/*
**	GsDt is the base class for all GsDt objects
*/

class EXPORT_CLASS_GSDTLITE GsDt
{
	GSDT_DECLARE_ABSTRACT_TYPE( GsDt )
private:
	int		m_LockCount;

	// MUTEX	m_Mutex;

public:
	class Iterator;			// Used by subscripting operators
	class FormatInfo;		// Used by A.doFormat( FmtInfo )
	class FuncArgs;         // Used by R = A.B( Args )

	// These can't be sub-classes because they are needed by GsType
	//    GsDtuBinaryFunc;	// Handles R = A OP B and A OP= B
	class ConvertFunction;	// Handles R = newtype( A )

	enum IterationFlag
	{
		ITER_NATIVE,		// Native order
		ITER_SORTED			// Sorted order
	};

	// Format appropriate error message
	static void	CastErr( const GsType& NewType, const GsDt* Obj );

	GsDt();
	virtual ~GsDt() = 0;	// Force this to be abstract

	const char*		typeName() const	{ return type().name().c_str(); }
	GsTypeId		typeId() const 		{ return type().Id(); }

	// General management functions
	void				lock();		// Lock object to prevent modification
	void				unlock();
	bool				isLocked() const { return m_LockCount > 0; }
//	bool				isLocked()		{ return m_LockCount > 0; }

	// FIX-Also need multi-threading API
	// GsStatus MutexWait( double Timeout );
	// GsStatus MutexLocked();	// Already locked by me?
	// void   MutexRelease();

	// Call these to find high/low/error values for this type
	virtual GsDt		*highLimit() const;
	virtual GsDt		*lowLimit() const;
	virtual GsDt		*errorSentinel() const;

	virtual GsDt		*CopyDeep() const;	// Create a deep copy of this value
	virtual GsDt		*CopyShallow() const;	// Create a shallow copy of this value
	GsDt				*Copy() const { return CopyShallow(); }


	virtual size_t		GetSize() const;
	virtual size_t		GetCapacity() const;
	virtual size_t		GetMemUsage() const;
	virtual void		GetInfo( FcDataTypeInstanceInfo &Info ) const;

	static size_t       GetGsHeapMemSize( const void *ptr );

	// DT_MSG_TO/FROM_STREAM
	virtual void		StreamStore( GsStreamOut &Stream ) const;
	virtual void		StreamRead(  GsStreamIn  &Stream );

	// DT_MSG_HASH_XXX
	virtual FcHashCode	HashComplete() const;	// Complete hashing (like _HASH_PORTABLE)
    virtual FcHashCode  HashQuick() const;	// Quick hashing (like HashStrHash--for structures)


	// DT_MSG_ADD/SUBTRACT/...
	// Arbitrary other type.  Dispatches through table
	GsDt				*BinaryOp( GSDT_BINOP Op, GsDt &Other ) const;

	// DT_MSG_ASSIGN_ADD/SUBTRACT/...
	// Arbitrary other type.  Dispatches through table
	GsStatus			BinaryOpAssign( GSDT_BINOP Op, const GsDt &Other );
	// Only called when both types are the same (called by BinaryOpAssign())
	virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );

	// DT_MSG_COMPARE and related
	// Maybe should have a separate equivalence test?
	virtual int			CompareSame( const GsDt &Other ) const;
	virtual bool		CompareEqual( const GsDt &Other ) const;
	int					Compare(	 const GsDt &Other ) const;
	bool				Equal(		 const GsDt &Other ) const	{ return Compare( Other ) == 0; }
	bool				NotEqual(	 const GsDt &Other ) const	{ return Compare( Other ) != 0; }
	bool				Greater(	 const GsDt &Other ) const	{ return Compare( Other ) >  0; }
	bool				GreaterEq(	 const GsDt &Other ) const	{ return Compare( Other ) >= 0; }
	bool				Less(		 const GsDt &Other ) const	{ return Compare( Other ) <  0; }
	bool				LessEq(		 const GsDt &Other ) const	{ return Compare( Other ) <= 0; }

	// DT_MSG_TRUTH	 returned value TRUE or FALSE (on a)
	virtual bool		Truth() const;
	// Is this the error sentinel?
	virtual bool		IsValid() const;


	// DT_MSG_UNARY_MINUS/...
	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsDt		*Max( const GsDt& rhs ) const { return NULL; }
	virtual GsDt		*Min( const GsDt& rhs ) const { return NULL; }
	
	virtual GsString	toString() const;
	virtual bool        toXmlString( CC_NS( std, ostream ) & ) const;

	// DT_MSG_DEREFERENCE			pValue = &(*This)
	//virtual GsStatus	Dereference( GsDt **pValue );
	// DT_MSG_DEREFERENCE_VALUE		pValue = *This		--FIX-KLL-Help!
	//virtual GsStatus	DereferenceValue( GsDt **pValue );

	// DT_MSG_MATH_OPERATE_ON_ALL	return (FuncInfo)( This[0..n] )
	//virtual GsDt		*MathOperateOnAll( GsDtuMathFunction &FuncInfo );

	virtual GsDt		*MathOperateOnAll( GsFuncHandle<double, double> Func ) const;

	virtual GsDt		*MathBinOpOnAll(   GsFunc2Handle<double, double, double> Func, const GsDt &Rhs ) const;

	virtual GsDt		*IfOpOnAll( const GsDt &TrueVal, const GsDt &FalseVal ) const;

/*
**	Subscripting and iteration
**
**  Note: This[ "foo" ] == This.foo for all classes, so there is only Subscript.
**		  I.e., DT_MSG_SUBSCRIPT and DT_MSG_COMPONENT are the same for GsDt.
*/

	// return modifiable iterator starting at This[ Index ]
	virtual Iterator	*Subscript( const GsDt &Index );

	virtual Iterator    *SubscriptFirst( IterationFlag Flag );	// Sorted, ...

	// delete This[ Index ]; This[ Index ] = NewValue;
	virtual GsStatus	SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );

	// delete This[ Index ];
	virtual GsStatus	SubscriptDelete( const GsDt &Index );

	// Append a value (optionally make a copy)
	virtual GsStatus	SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

	// Handy-dandy accessor functions using the above virtuals
	// return non-modifiable pointer to This[ Index ] (or NULL/GS_ERR)
	const GsDt			*SubscriptValue( const GsDt &Index ) const;
	const GsDt			*SubscriptValue( int Index ) const;
	const GsDt			*SubscriptValue( const char *Index ) const;
	// return a copy (which must be deleted)
	GsDt				*SubscriptValueCopy( const GsDt &Index );
	GsDt				*SubscriptValueCopy( int Index );
	GsDt				*SubscriptValueCopy( const char *Index );


	// D_MSG_FORMAT  sprintf-like behaviour - see <gsdt/format.h>
	GsStatus			format( GsDt::FormatInfo &Info );		// Handles defaults

	// DT_MSG_SET	 0x0050		// Every element of a set to b
	// DT_MSG_QUERY  0x0320		// r = Query( a ), where Query is in (b)

	// DT_MSG_PRINT		print contents of PrintMe 'into' This
	virtual GsStatus	printInto(		GsDt &PrintMe );
	virtual GsStatus	printIntoStart(	GsDt &PrintMe );
	virtual GsStatus	printIntoEnd(	GsDt &PrintMe );

	// DT_MSG_CALL_MEMBER  RetValue = This.Args.Func( Args.Args )
	// GS_SLANG_OK, ..ERROR, etc.
	virtual GsSlangRetCode callMember( GsDt::FuncArgs &Args, GsDt *&RetValue );

	virtual GsStatus callMemberFn( const char *memberName, GsDt *Args[], int nArgs, GsDt *&RetValue );

	//virtual void  toDtValue( void *dtValue ) const;
	//virtual GsDt* fromDtValue( void *dtValue ) const;	

	// Call this object as a function, e.g. @F(x) from slang.
	virtual GsDt *Evaluate( const GsDtArray &Args ) const;

	// the prefix used for generated member function wrappers
	virtual CC_NS(std,string) memberFunctionWrapperPrefix() const;

protected:
	// Implementation only.  Call format(...) instead
	GsStatus			formatAsString( GsDt::FormatInfo &Info );
	virtual GsStatus	doFormat( GsDt::FormatInfo &Info );		// call formatAsString if nothing else
	virtual GsString    instanceInfo( GsDt::FormatInfo &Info );

	// Helper function for hashing derived types that happen to contain doubles.

	static FcHashCode	HashCompleteDouble( double Data );	// Complete hashing (like _HASH_PORTABLE)

private:
	GsDtuBinaryFunc &BinaryFuncLookup( const GsType &OtherType ) const;

};


/*
**	Iterator is the base iterator class for all GsDt objects
*/

class EXPORT_CLASS_GSDTLITE GsDt::Iterator
{
private:
	Iterator( const Iterator &Rhs );
	Iterator &operator=( const Iterator &Rhs );

public:
	// FIX-Should define standard iterator operators
	// ++, *, assignment, copy constructor
	Iterator();
	virtual ~Iterator();

	virtual Iterator	*Copy() = 0;		// Create a copy of this iterator

	// All are pointers because they might fail
	virtual GsDt		*BaseValue();			// Value upon which we are iterating
	virtual GsDt		*CurrentKey();   		// Current key/index
    virtual const GsDt	*CurrentValue();   		// Current value of Base[ Key ]
	virtual GsDt		*ModifyValueBegin();	// Current value that is modifiable
	virtual GsStatus	ModifyValueEnd();		// Done with modifications to current value
	virtual GsStatus	Replace( GsDt *Val, GsCopyAction Action );	// Replace current value

	virtual bool		More();					// True if more items
	virtual void		Next();					// Advance to next item and return More();
//	virtual bool	Prev();				// Advance to prev item
//	virtual bool	First();			// Reset to first item
//	virtual bool	Last();				// Reset to last item
};

CC_END_NAMESPACE

#endif

