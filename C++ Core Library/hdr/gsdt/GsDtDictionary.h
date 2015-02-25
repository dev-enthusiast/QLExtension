/****************************************************************
**
**	gsdt/GsDtDictionary.h	- The GsDtDictionary class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	GenInfo
**	Class		- GsDtDictionary
**      Base		- Dictionary
**      BaseParent	- 
**      HdrName		- gsdt/GsDtDictionary.h
**      FnClass		- GsDtDictionary.cpp
**      Streamable	- 1
**      Iterable	- 1
**      PrintTo		- 1
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtDictionary.h,v 1.8 2001/11/27 22:45:00 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_GSDTDICTIONARY_H )
#define IN_GSDT_GSDTDICTIONARY_H

#include	<gsdt/base.h>
#include	<gsdt/GsDt.h>
#include	<gscore/GsString.h>
#include	<gscore/GsDouble.h>
#include	<gscore/GsInteger.h>
#include	<gscore/GsUnsigned.h>
#include	<gsdt/GsDtString.h>
#include	<gscore/GsHash.h>


CC_BEGIN_NAMESPACE( Gs )


/*
**	Forward declare classes
*/

typedef GsHashMap< GsString, GsDt*, GsHashFromString, StrICmpFunctor > GsHashMapStringToGsDtPtrNoCase;

class GsDtDictionary;


class EXPORT_CLASS_GSDTLITE GsDtDictionary : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtDictionary )
public:
	GsDtDictionary();
	GsDtDictionary( const GsDtDictionary &Obj );

	virtual ~GsDtDictionary();

	virtual GsDt*		highLimit() const;
	virtual GsDt*		lowLimit() const;
	virtual GsDt*		errorSentinel() const;

	virtual GsDt*		CopyDeep() const;
	virtual GsDt*		CopyShallow() const;

	virtual size_t		GetSize() const;
	virtual size_t		GetCapacity() const;
	virtual size_t		GetMemUsage() const;
	virtual void		GetInfo( FcDataTypeInstanceInfo &Info ) const;

	virtual void		StreamStore( GsStreamOut &Stream ) const;
	virtual void		StreamRead(  GsStreamIn  &Stream );

	virtual FcHashCode	HashComplete() const;
	virtual FcHashCode  HashQuick() const;

	virtual GsStatus	BinaryOpAssignSame( GSDT_BINOP Op, const GsDt &Other );
	virtual int			CompareSame( const GsDt &Other ) const;

	virtual GsBool		Truth() const;
	virtual GsBool		IsValid() const;

	virtual GsStatus	Increment();
	virtual GsStatus	Decrement();
	virtual GsDt		*Negate() const;

	virtual GsString	toString() const;

	size_t				size() const { return m_Hash.size(); }
	
	GsDtDictionary &operator=( const GsDtDictionary &Obj );

	// Union in the rhs, i.e. put in every element of rhs that does
	// not already exist.  If component exists, do not replace with
	// value from rhs.

	GsDtDictionary &UnionAssign( 	   const GsDtDictionary &Rhs ); 
	GsDtDictionary &ReplaceComponents( const GsDtDictionary &Rhs ); 

protected:

	virtual GsString instanceInfo( GsDt::FormatInfo &Info );

private:

	typedef GsHashMapStringToGsDtPtrNoCase HashRep;

    HashRep
			m_Hash;

public:

	class Iterator;
	friend class Iterator;

	virtual GsDt::Iterator*	Subscript( const GsDt &Index );
	virtual GsDt::Iterator*	SubscriptFirst( IterationFlag Flag );
	virtual GsStatus		SubscriptReplace( const GsDt &Index, GsDt *NewValue, GsCopyAction Action );
	virtual GsStatus		SubscriptDelete( const GsDt &Index );
	virtual GsStatus		SubscriptAppend( GsDt *NewValue, GsCopyAction Action );

#if 0
	virtual GsStatus	DoFormat( GsDtuFormatInfo &Info );

	virtual GsStatus	CallMember( GsDtuFuncArgs &Args, GsDtPtr &RetValue );
	virtual GsStatus	UserMessage( GsDtuUserMessage &Msg, GsDtuFuncArgs &Args );
#endif
	
	typedef HashRep::Entry 			Entry;
	typedef HashRep::ConstEntry 	ConstEntry;

	typedef HashRep::Iterator		iterator;
	typedef HashRep::ConstIterator	const_iterator;

	Entry 	   Lookup( const GsString &Key ) 	   { return m_Hash.Lookup( Key ); }
	ConstEntry Lookup( const GsString &Key ) const { return m_Hash.Lookup( Key ); }

	void Insert( const GsString &Key, GsDt *Value ) { m_Hash.Insert( Key, Value ); }

	const_iterator begin() const { return m_Hash.Begin(); }
	      iterator begin()       { return m_Hash.Begin(); }

	const_iterator end() const { return m_Hash.End(); }
	      iterator end()       { return m_Hash.End(); }

};

// These are the functions that one will normally use to get and set
// elements.  

// NOTE: In GsDtDictionaryElement() we have to violate the standard
// convention of returning by value and return by reference so that we
// can get the type T into the template signature.


template< class T > void GsDtDictionaryElement( 		const GsDtDictionary &Dict, const GsString &Tag, const T * &ReturnValue );
template< class T > void GsDtDictionaryRequiredElement( const GsDtDictionary &Dict, const GsString &Tag, const T * &ReturnValue, 
														const GsString &CustomErrorMsg = "" );
template< class T > void GsDtDictionaryElementSet( 		GsDtDictionary &Dict, const GsString &Tag, const T &Value 		);


CC_END_NAMESPACE

#endif


