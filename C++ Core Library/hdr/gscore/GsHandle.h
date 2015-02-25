/****************************************************************
**
**	gscore/GsHandle.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsHandle.h,v 1.16 2012/07/19 13:12:15 e19351 Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSHANDLE_H )
#define IN_GSCORE_GSHANDLE_H

#include	<gscore/base.h>

CC_BEGIN_NAMESPACE( Gs )

//	- The GsHandle class will be brilliantly documented
//	- as soon as Pantelis gets done rewriting it.
//	This seems unlikely.

/****************************************************************
** Class	   : GsHandleShared
** Description : 
****************************************************************/

template< class Rep > class GsHandleShared 
{
protected:
	GsHandleShared() : m_rep( 0 ) {}
	GsHandleShared( Rep* rep ) : m_rep( rep ) 
	{
		if( m_rep )
			m_rep->incCount();
	}
	GsHandleShared( const GsHandleShared& x ) :	m_rep( x.m_rep )
	{
		if( m_rep )
			m_rep->incCount();
	}
	GsHandleShared& operator = ( const GsHandleShared& x )
	{
		if( m_rep != x.m_rep )
		{
			if( m_rep != 0 )
			{
				m_rep->decCount();
			}
			m_rep = x.m_rep;
			if( m_rep )
			{
				m_rep->incCount();
			}
		}
		return *this;
	}
	GsHandleShared& operator = ( Rep* rep )
	{
		if( m_rep != rep )
		{
			if( m_rep != 0 )
			{
				m_rep->decCount();
			}
			m_rep = rep;
			if( m_rep )
			{
				m_rep->incCount();
			}
		}
		return *this;
	}
	virtual ~GsHandleShared()
	{
		if( m_rep != 0 )
		{
			m_rep->decCount();
		}
	}

protected:
	Rep* m_rep;
};

/****************************************************************
** Class	   : GsHandleCopyOnWrite
** Description : 
****************************************************************/

template< class Rep > class GsHandleCopyOnWrite :
	public GsHandleShared< Rep >
{
protected:
	void cloneRep( bool forceNewCopy = false )
	{
		if( this->m_rep && ( ! this->m_rep->isShareable() || forceNewCopy ) )
		{
			this->m_rep->decCount();
			this->m_rep = this->m_rep->copy();
			this->m_rep->incCount();
		}
	}
	void singleShareableRep()
	{
		if( this->m_rep && ! this->m_rep->isUniqueRep() ) 
			cloneRep( true ); 
	}
	void singleUnshareableRep()
	{
		singleShareableRep(); 
		if( this->m_rep )
			this->m_rep->markUnshareable(); 
	}
	GsHandleCopyOnWrite() : GsHandleShared< Rep >() {}
	GsHandleCopyOnWrite( Rep* rep ) : GsHandleShared< Rep >( rep ) 
	{
		cloneRep();
	}
	GsHandleCopyOnWrite( const GsHandleCopyOnWrite& x ): 
	GsHandleShared<Rep>( x )
	{
		cloneRep();
	}
	GsHandleCopyOnWrite& operator = ( const GsHandleCopyOnWrite& x )
	{
		GsHandleShared< Rep >::operator = ( x );
		cloneRep();

		return *this;
	}
	GsHandleCopyOnWrite& operator = ( Rep* rep )
	{
		GsHandleShared< Rep >::operator = ( rep );
		cloneRep();

		return *this;
	}
	virtual ~GsHandleCopyOnWrite() {}
};

/****************************************************************
** Class	   : GsHandleReadOnly
** Description : 
****************************************************************/

template< class Rep > class GsHandleReadOnly 
{
private:
	void	incCount()
	{
		if( m_rep )
			const_cast<Rep *>( m_rep )->incCount();
	}
	void	decCount()
	{
		if( m_rep )
			const_cast<Rep *>( m_rep )->decCount();
	}
protected:
	GsHandleReadOnly() : m_rep( 0 ) {}
	GsHandleReadOnly( const Rep* rep ) : m_rep( rep ) 
	{
		incCount();
	}
	GsHandleReadOnly( const GsHandleReadOnly& x ) :	m_rep( x.m_rep )
	{
		incCount();
	}
	GsHandleReadOnly& operator = ( const GsHandleReadOnly& x )
	{
		if( m_rep != x.m_rep )
		{
			decCount();
			m_rep = x.m_rep;
			incCount();
		}
		return *this;
	}
	GsHandleReadOnly& operator = ( const Rep* rep )
	{
		if( m_rep != rep )
		{
			decCount();
			m_rep = rep;
			incCount();
		}
		return *this;
	}
	virtual ~GsHandleReadOnly()
	{
		decCount();
	}

protected:
	const Rep* m_rep;
};

CC_END_NAMESPACE
#endif

