/****************************************************************
**
**	GSDTDEFAULTITERATOR.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtDefaultIterator.h,v 1.4 2001/11/27 22:44:59 procmon Exp $
**
****************************************************************/

#ifndef IN_GSDTLITE_GSDTDEFAULTITERATOR_H
#define IN_GSDTLITE_GSDTDEFAULTITERATOR_H

#include <gsdt/base.h>
#include <gsdt/GsDt.h>

CC_BEGIN_NAMESPACE( Gs )

template< class T >
class GsDtDefaultIterator : public GsDt::Iterator
{
public:
	GsDtDefaultIterator();
	GsDtDefaultIterator( const GsDt::Iterator &Other );
	virtual ~GsDtDefaultIterator();

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
};

CC_END_NAMESPACE

#endif 
