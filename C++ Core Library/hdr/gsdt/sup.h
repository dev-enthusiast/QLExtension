/****************************************************************
**
**	sup.h	- Random support functions/classes
**
**	Not intended for export from gsdt project
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/sup.h,v 1.3 2001/11/27 22:45:05 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_SUP_H )
#define IN_GSDT_SUP_H

#include <gsdt/GsDtVector.h>
#include <gsdt/GsDtDouble.h>
#include <gsdt/GsDtArray.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	Iterator for GsDtVector
*/

class GsDtVector::Iterator : public GsDt::Iterator
{
public:
	Iterator( GsDtVector *Vec, int StartIndex = 0 );
	Iterator( const Iterator &Other );
	virtual ~Iterator();

	virtual GsDt::Iterator	*Copy();
	virtual GsDt			*BaseValue();
	virtual GsDt			*CurrentKey();
    virtual const GsDt		*CurrentValue();
	virtual GsDt			*ModifyValueBegin();
	virtual GsStatus		ModifyValueEnd();
	virtual GsStatus		Replace( GsDt *Val, GsCopyAction Action );

	virtual GsBool			More();
	virtual void			Next();

private:
	Iterator &operator=( const Iterator &Rhs );

	GsDtVector		*m_Vec;		// Vector over which we are iterating
	size_t			m_VecSize;	// m_Vec->GetSize();
	GsDtDouble		*m_Key;		// Current key
	size_t			m_Index;	// Actual current index (converted to m_Key as needed)
	GsDtDouble		*m_CurVal;	// Converted from actual vector element as needed

	GsBool			InRange() { return m_Index >= 0 && m_Index < m_VecSize; }
};



/*
**	Iterator for GsDtArray
*/

class GsDtArray::Iterator : public GsDt::Iterator
{
public:
	Iterator( GsDtArray *Vec, int StartIndex = 0 );
	Iterator( const Iterator &Other );
	virtual ~Iterator();

	virtual GsDt::Iterator	*Copy();
	virtual GsDt			*BaseValue();
	virtual GsDt			*CurrentKey();
    virtual const GsDt		*CurrentValue();
	virtual GsDt			*ModifyValueBegin();
	virtual GsStatus		ModifyValueEnd();
	virtual GsStatus		Replace( GsDt *Val, GsCopyAction Action );

	virtual GsBool			More();
	virtual void			Next();

private:
	Iterator &operator=( const Iterator &Rhs );

	GsDtArray		*m_Array;		// Array over which we are iterating
	size_t			m_ArraySize;	// m_Array->GetSize();
	GsDtDouble		*m_Key;			// Current key
	size_t			m_Index;		// Actual current index (converted to m_Key as needed)
	GsBool			m_ModActive;	// TRUE if ModifyBegin() called, but not End() yet

	GsBool			InRange() { return m_Index >= 0 && m_Index < m_ArraySize; }
};


CC_END_NAMESPACE
#endif
