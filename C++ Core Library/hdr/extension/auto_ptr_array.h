/****************************************************************
**
**	AUTO_PTR_ARRAY.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/extension/src/extension/auto_ptr_array.h,v 1.2 2001/08/07 15:18:26 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSBASE_AUTO_PTR_ARRAY_H
#define IN_GSBASE_AUTO_PTR_ARRAY_H

#include <arrowkludge.h>

CC_BEGIN_NAMESPACE( extension )

template <class Tp_>
struct auto_ptr_array_ref
{
	Tp_* m_ptr;
	explicit auto_ptr_array_ref( Tp_* p_ ) : m_ptr(p_) {}
};

template <class Tp_>
class auto_ptr_array
{
private:
	Tp_* m_ptr;

public:
	typedef Tp_ element_type;

	explicit auto_ptr_array(Tp_* p_ = 0) : m_ptr(p_) {}
	auto_ptr_array(auto_ptr_array& a_) : m_ptr(a_.release()) {}

	auto_ptr_array& operator=( auto_ptr_array& a_) { reset( a_.release() ); return *this; }
	~auto_ptr_array() { delete [] m_ptr; }

    Tp_& operator[] ( int i ) { return m_ptr[ i ]; }
    Tp_& operator*  () const  { return *m_ptr; }
    Tp_* get        () const  { return m_ptr; }

	Tp_* release() {
		Tp_* tmp_ = m_ptr;
		m_ptr = 0;
		return tmp_;
	}
	void reset(Tp_* p_ = 0) {
		if( p_ != m_ptr )
		{
			delete [] m_ptr;
			m_ptr = p_;
		}
	}

	// auto_ptr_array_ref magic
	auto_ptr_array( auto_ptr_array_ref<Tp_> ref_ ) : m_ptr( ref_.m_ptr ) {}
	auto_ptr_array& operator=( auto_ptr_array_ref<Tp_> ref_ ) { this->reset( ref_.m_ptr ); return *this;}
	operator auto_ptr_array_ref<Tp_>() { return auto_ptr_array_ref<Tp_>(this->release());}
};

CC_END_NAMESPACE

#endif 
