// -*-c++-*-

/****************************************************************
**
**	GsRefCountedPtr.H	- A reference counted pointer which does not
**  require the underlying object to subclass anything. This
**  class is constructed with templates to the type and to
**  the counter class. Most common usage is
**
**  GsRefCountedPtr< myClass, int> x( new myClass() );
**
**  then x can be copied, etc... and when all the copies go out
**  of scope the underlying is destroyed. 
**
**  Advanced users can consider using things other than int
**  as the counter class. If you do this, you probably
**  won't be able to count fundamentals (e.g. int, double)
**  using your new counter class on solaris with SW 4.2
**
**  Despite PW moving this to gscore, Kenny wrote it all,
**  and moved it to extension.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$$
**
****************************************************************/


#ifndef EXTENSION_REF_COUNT_H
#define EXTENSION_REF_COUNT_H

#include <arrowkludge.h> // for arrowReturnKludge from portable

CC_BEGIN_NAMESPACE( extension )

//
// Simple reference counted smart-pointer thingy.
//

template <class T_, class C_ /* = int */>
class GsRefCountedPtr
{
public:
    // type names for the value
    typedef T_ element_type;

	// constructor, only constructor can throw if new fails (std::bad_alloc), or if C_'s constructor fails  - all other functions are nothrow
    explicit GsRefCountedPtr( T_* arg = 0 ); // we own arg

    // copy constructor
	GsRefCountedPtr( GsRefCountedPtr<T_, C_> const& ) throw();

    // destructor
	~GsRefCountedPtr() throw();

    // assignment operator
	GsRefCountedPtr& operator=( GsRefCountedPtr<T_, C_> const& rhs ) throw();
    // would be nice to have a templated assignment operator too

    // convenience assignment function
    void reset( T_* ptr = 0 ) { *this = GsRefCountedPtr<T_, C_>( ptr ); }

    // standard smart-pointer methods
	typename arrowReturnKludge<T_>::type operator->() const throw() { return m_stuff; }

	T_& operator*() const throw() { return *m_stuff;}

    T_*	get()       const throw() { return m_stuff;}

	// be VERY VERY careful - NOT for general consumption
	C_& ref() const throw() { return *m_ref_count;}

    void swap( GsRefCountedPtr<T_,C_>& rhs ) throw()
    {
		T_* t_stuff = m_stuff;     m_stuff     = rhs.m_stuff;     rhs.m_stuff     = t_stuff;
		C_* t_count = m_ref_count; m_ref_count = rhs.m_ref_count; rhs.m_ref_count = t_count;
	}

private:
	void m_dec_ref() throw();

	T_* m_stuff;
	C_* m_ref_count;
};

template <class T_, class C_>
inline bool operator<( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() < rhs.get();
}

template <class T_, class C_>
inline bool operator>( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() > rhs.get();
}

template <class T_, class C_>
inline bool operator<=( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() <= rhs.get();
}

template <class T_, class C_>
inline bool operator>=( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() >= rhs.get();
}

template <class T_, class C_>
inline bool operator==( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() == rhs.get();
}

template <class T_, class C_>
inline bool operator!=( GsRefCountedPtr<T_, C_> const& lhs, GsRefCountedPtr<T_, C_> const& rhs )
{
    return lhs.get() != rhs.get();
}

// --------- Implementation --------------------

template <class T_, class C_>
GsRefCountedPtr<T_, C_>::GsRefCountedPtr( T_* arg ) // we own arg - think about exception safety, we must delete it if an exception is thrown
  : m_stuff( 0 ),
	m_ref_count( 0 )
{
	try {
		m_ref_count = new C_(1); // this can throw
		m_stuff = arg;           // this can't
	} 
	catch ( ... ) 
	{
		delete arg;
		throw;
	}
}

template <class T_, class C_>
GsRefCountedPtr<T_, C_>::GsRefCountedPtr( GsRefCountedPtr<T_, C_> const& rhs ) throw()
  : m_stuff( rhs.m_stuff ),
	m_ref_count( rhs.m_ref_count )
{
	++*m_ref_count;
}

template <class T_, class C_>
GsRefCountedPtr<T_, C_>::~GsRefCountedPtr() throw()
{
	m_dec_ref();
}

template <class T_, class C_>
GsRefCountedPtr<T_, C_>& GsRefCountedPtr<T_, C_>::operator=( GsRefCountedPtr<T_, C_> const& rhs ) throw()
{
	++*rhs.m_ref_count;
	m_dec_ref();
	m_stuff     = rhs.m_stuff;
	m_ref_count = rhs.m_ref_count;
	return *this;
}

template <class T_, class C_>
void GsRefCountedPtr<T_, C_>::m_dec_ref() throw()
{
	if (! --*m_ref_count )
	{
		delete m_stuff;
		delete m_ref_count;
	}
}

CC_END_NAMESPACE

#endif
