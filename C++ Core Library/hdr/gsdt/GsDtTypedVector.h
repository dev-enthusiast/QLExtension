/****************************************************************
**
**	GSDTTYPEDVECTOR.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/GsDtTypedVector.h,v 1.3 2011/05/16 22:22:32 khodod Exp $
**
****************************************************************/

#ifndef IN_GSDT_GSDTTYPEDVECTOR_H
#define IN_GSDT_GSDTTYPEDVECTOR_H

#include <gsdt/base.h>
#include <gsdt/GsDtTraits.h>
#include <gscore/GsTypedVector.h>
#include <gscore/GsException.h>
#include <err_base.h>

CC_BEGIN_NAMESPACE( Gs )

template<class To, class From, class Container>
class GsDtTypedVector : public GsTypedVector<To>
{
public:

	GsDtTypedVector( GsType const& elem_type, Container& array ) : m_elem_type( elem_type ), m_array( array )
	{}

	virtual size_t size() const { return m_array.size(); }

	virtual typename GsTypedVector<To>::reference elem( size_t index ) const
	{
		typedef typename GsDtTypeMap<To>::GsDtType ToGsDtType;

		From* elem_val = m_array.elem( index );
		if( elem_val->isKindOf( m_elem_type ) )
		{
			ToGsDtType const* to_gsdt = (ToGsDtType const* ) elem_val;
			return to_gsdt->data();
		}
		Err( ERR_INVALID_ARGUMENTS, "TypedVector elem %d is not a %s", index, m_elem_type.name().c_str() );
		GSX_THROW( GsXBadCast, ErrGetString() );
	}

protected:
	GsType const& m_elem_type;
	Container& m_array;
};

CC_END_NAMESPACE

#endif 
