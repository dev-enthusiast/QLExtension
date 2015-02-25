/****************************************************************
**
**	gscore/GsPTimeVector.h	- Header for GsPTimeVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSPTIMEVECTOR_H)
#define IN_GSCORE_GSPTIMEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsPTime.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsPTimeVector;
class GsPTimeVectorRep;
class GsPTimeVecRevIter;
class GsPTimeVecConstRevIter;

/*
**  class GsPTimeVecRevIter
*/

class EXPORT_CLASS_GSDATE GsPTimeVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsPTime value_type;
	typedef ptrdiff_t difference_type;
	typedef GsPTime *pointer;
	typedef GsPTime &reference;
	typedef GsPTime *iterator_type;

	GsPTimeVecRevIter() 
		: m_current() {}


	explicit GsPTimeVecRevIter( iterator_type x ) 
		: m_current(x) {}

	iterator_type Class() const  { return m_current; }

	reference operator * () const 
	{
		iterator_type tmp=m_current;
		return *--tmp;
	}

#if defined(CC_NO_ARROW_OPERATOR)
	struct __arrow_helper
	{
		__arrow_helper(pointer ptr) : ptr_(ptr) {;}
		operator pointer () { return ptr_; }
		pointer ptr_;
	};

	__arrow_helper operator-> () const 
	{
		iterator_type tmp(m_current);
		return __arrow_helper(--tmp);
	}

#else
	pointer operator-> () const 
	{
		iterator_type tmp=m_current;
		return --tmp;
	}
#endif

	reference operator [] (difference_type n) const 
	{
		iterator_type tmp=m_current;
		return *(--tmp+n);
	}

	GsPTimeVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsPTimeVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsPTimeVecRevIter operator++(int) 
	{
		GsPTimeVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsPTimeVecRevIter operator--(int) 
	{
		GsPTimeVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsPTimeVecRevIter operator+(
	const GsPTimeVecRevIter &iter,
	GsPTimeVecRevIter::difference_type n
) 
{
	return GsPTimeVecRevIter(iter.Class()-n);
}

inline GsPTimeVecRevIter &operator+=(
	GsPTimeVecRevIter &iter,
	GsPTimeVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsPTimeVecRevIter operator-(
	const GsPTimeVecRevIter &iter,
	GsPTimeVecRevIter::difference_type n
) 
{
	return GsPTimeVecRevIter( iter.Class() + n );
}

inline GsPTimeVecRevIter &operator-=(
	GsPTimeVecRevIter &iter,
	GsPTimeVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsPTimeVecRevIter &lop,
	const GsPTimeVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsPTimeVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsPTimeVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsPTime value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsPTime *pointer;
	typedef const GsPTime &reference;
	typedef const GsPTime *iterator_type;

	GsPTimeVecConstRevIter() 
		: m_current() {}

	explicit GsPTimeVecConstRevIter(iterator_type x) 
		: m_current(x) {}

	iterator_type Class() const  { return m_current; }

	reference operator * () const 
		{ iterator_type tmp=m_current; return *--tmp; }

#if defined(CC_NO_ARROW_OPERATOR)
	struct __arrow_helper
	{
		__arrow_helper(pointer ptr) : ptr_(ptr) {;}
		operator pointer () { return ptr_; }
		pointer ptr_;
	};

	__arrow_helper operator-> () const 
	{
		iterator_type tmp(m_current);
		return __arrow_helper(--tmp);
	}

#else
	pointer operator-> () const 
	{
		iterator_type tmp=m_current;
		return --tmp;
	}
#endif

	reference operator [] (difference_type n) const 
		{ iterator_type tmp(m_current); return *(--tmp+n); }

	GsPTimeVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsPTimeVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsPTimeVecConstRevIter operator++(int) 
		{ GsPTimeVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsPTimeVecConstRevIter operator--(int) 
		{ GsPTimeVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsPTimeVecConstRevIter  operator+(
	const GsPTimeVecConstRevIter &iter,
	GsPTimeVecConstRevIter::difference_type n
) 
{
	return GsPTimeVecConstRevIter(iter.Class()-n);
}

inline GsPTimeVecConstRevIter &operator+=(
	GsPTimeVecConstRevIter &iter,
	GsPTimeVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsPTimeVecConstRevIter operator-(
	const GsPTimeVecConstRevIter &iter,
	GsPTimeVecConstRevIter::difference_type n
) 
{
	return GsPTimeVecConstRevIter( iter.Class() + n );
}

inline GsPTimeVecConstRevIter &operator-=(
	GsPTimeVecConstRevIter &iter,
	GsPTimeVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsPTimeVecConstRevIter &lop,
	const GsPTimeVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsPTimeVectorRep
*/

class EXPORT_CLASS_GSDATE GsPTimeVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsPTime value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsPTime* iterator;
	typedef const GsPTime* const_iterator;
	typedef GsPTimeVecRevIter reverse_iterator;
	typedef GsPTimeVecConstRevIter const_reverse_iterator;

	typedef GsPTime &reference;
	typedef const GsPTime &const_reference;

	// Constructors
	GsPTimeVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsPTimeVectorRep(size_t);
	GsPTimeVectorRep(size_t,const value_type &x);
	GsPTimeVectorRep(const_iterator begin, const_iterator end);
	GsPTimeVectorRep(const GsPTimeVectorRep &other);

	// Destructor
	~GsPTimeVectorRep();

	// Assignment
	GsPTimeVectorRep &operator = (const GsPTimeVectorRep&);

	// Copy
	GsPTimeVectorRep *copy() { return new GsPTimeVectorRep( *this ); }

	// Element Access
	reference operator [] (int n) { return *(m_begin+n); }
	const_reference operator [] (int n) const { return *(m_begin+n); }

	reference front() { return *m_begin; }
	const_reference front() const  { return *m_begin; }

	reference back()  { return *(m_end-1); }
	const_reference back() const  { return *(m_end-1); }

	// Iterator Access
	iterator begin()  { return m_begin; }
	const_iterator begin() const  { return m_begin; }

	iterator end()  { return m_end; }
	const_iterator end() const  { return m_end; }

	reverse_iterator rbegin() 
	{ return reverse_iterator( m_end ); }

	const_reverse_iterator rbegin() const 
	{ return const_reverse_iterator( m_end ); }

	reverse_iterator rend() { return reverse_iterator( m_begin ); }
	const_reverse_iterator rend() const 
	{ return const_reverse_iterator( m_begin ); }

	// Stack and Queue Operations
	void push_back( const value_type &x ) { insert( m_end, x ); }
	void pop_back()  { erase( m_end - 1 ); }

	// List Operations
	iterator insert( iterator p );
	iterator insert( iterator p, const value_type &x );
	void insert( iterator p, size_type n, const value_type &x);
	void insert( iterator p, const_iterator fist, const_iterator last );
	void erase( iterator p);
	iterator erase( iterator first, iterator last );
	void clear()  { erase( m_begin, m_end ); }

	// Other Operations
	size_type size() const { return m_begin ? m_end - m_begin : 0; }
	size_type empty() const	{ return m_begin == m_end; }
	size_type max_size() const { return /*?*/ 0; }
	size_type capacity() const { return m_begin ? m_max - m_begin : 0; }

	void reserve( size_type n );
	void resize( size_type n );
	void resize( size_type n, const value_type &x );

	bool operator==(const GsPTimeVectorRep &x) const;
	bool operator<(const GsPTimeVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsPTimeVector
*/

class EXPORT_CLASS_GSDATE GsPTimeVector : public GsHandleCopyOnWrite<GsPTimeVectorRep>
{
public:
	typedef GsPTimeVectorRep::value_type value_type;
	typedef GsPTimeVectorRep::size_type size_type;
	typedef GsPTimeVectorRep::difference_type difference_type;

	typedef GsPTimeVectorRep::iterator iterator;
	typedef GsPTimeVectorRep::const_iterator const_iterator;
	typedef GsPTimeVectorRep::reverse_iterator reverse_iterator;
	typedef GsPTimeVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsPTimeVectorRep::reference reference;
	typedef GsPTimeVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsPTimeVectorRep* const_rep;
public:

	// Constructors
	GsPTimeVector();
	GsPTimeVector(size_t);
	GsPTimeVector(size_t,const value_type &x);
	GsPTimeVector(const_iterator begin, const_iterator end);
	GsPTimeVector(const GsPTimeVector &other);

	// Destructor
	virtual ~GsPTimeVector() {}

	// Assignment
	GsPTimeVector &operator = (const GsPTimeVector&);

	// Element Access
	reference operator [] (int n);
	const_reference operator [] (int n) const { return ( *m_rep )[ n ]; }

	reference front();
	const_reference front() const { return m_rep->front(); }

	reference back();
	const_reference back() const { return m_rep->back(); }

	// Iterator Access
	iterator begin();
	const_iterator begin() const { return m_rep->begin(); }

	iterator end();
	const_iterator end() const { return m_rep->end(); }

	reverse_iterator rbegin();
	/* FIX: comp can't pick correct m_rep->rbegin() so have to cast */
	const_reverse_iterator rbegin() const
	{ const_rep temp = m_rep; return temp->rbegin(); }

	reverse_iterator rend(); 
	const_reverse_iterator rend() const /* FIX: comp can't pick rend */
	{ const_rep temp =  m_rep; return temp->rend(); }

	// Stack and Queue Operations
	void push_back( const value_type &x ); 
	void pop_back();

	// List Operations
	iterator insert( iterator p ); 
	iterator insert( iterator p, const value_type &x ); 
	void insert( iterator p, size_type n, const value_type &x);
	void insert( iterator p, const_iterator first, const_iterator last );
	void erase( iterator p); 
	iterator erase( iterator first, iterator last );
	void clear();

	// Other Operations
	size_type size() const { return m_rep->size(); }
	size_type empty() const { return m_rep->empty(); }
	size_type max_size() const { return m_rep->max_size(); }
	size_type capacity() const { return m_rep->capacity(); }

	void reserve( size_type n ); 
	void resize( size_type n ); 
	void resize( size_type n, const value_type &x );	

	bool operator==(const GsPTimeVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsPTimeVector &x) const { return (!(*this == x)); }
	bool operator<(const GsPTimeVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsPTimeVector &x) const { return (x < *this); }
	bool operator<=(const GsPTimeVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsPTimeVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsPTimeVector> GsDtType;
	typedef GsDtPTime GsDtTypeElem;
	inline static const char* typeName() { return "GsPTimeVector"; }
	GsString toString() const;
	int compare( const GsPTimeVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSPTIMEVECTOR_H) */


