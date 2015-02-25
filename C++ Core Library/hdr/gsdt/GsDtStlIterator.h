/****************************************************************
**
**	GSDTSTLITERATOR.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtStlIterator.h,v 1.9 2012/07/19 13:16:30 e19351 Exp $
**
****************************************************************/

#ifndef IN_GSDTLITE_GSDTSTLITERATOR_H
#define IN_GSDTLITE_GSDTSTLITERATOR_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>
#include <gsdt/GsDtTypeMap.h>
#include <gsdt/GsDtTraits.h>

CC_BEGIN_NAMESPACE( Gs )

template< class T >
class GsDtDefaultStlIterator : public GsDt::Iterator
{
public:
	GsDtDefaultStlIterator( typename GsDtTypeMap< T >::GsDtType *t );
	GsDtDefaultStlIterator( const GsDtDefaultStlIterator< T > &Other );
	virtual ~GsDtDefaultStlIterator();

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
	GsDt::Iterator &operator=( const GsDt::Iterator &Rhs );

private:
	typename GsDtTypeMap< T >::GsDtType *m_Underlying;

	typename T::iterator m_Begin;
        typename T::iterator m_End;
        typename T::iterator m_Iter;

	typename GsDtTypeMap< typename T::value_type >::GsDtType *m_CurVal;
};

CC_END_NAMESPACE

#endif 
