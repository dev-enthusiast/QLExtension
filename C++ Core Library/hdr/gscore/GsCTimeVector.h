/****************************************************************
**
**	gscore/GsCTimeVector.h	- Header for GsCTimeVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSCTIMEVECTOR_H)
#define IN_GSCORE_GSCTIMEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsCTime.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsCTimeVector;
class GsCTimeVectorRep;
class GsCTimeVecRevIter;
class GsCTimeVecConstRevIter;

/*
**  class GsCTimeVecRevIter
*/

class EXPORT_CLASS_GSDATE GsCTimeVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsCTime value_type;
	typedef ptrdiff_t difference_type;
	typedef GsCTime *pointer;
	typedef GsCTime &reference;
	typedef GsCTime *iterator_type;

	GsCTimeVecRevIter() 
		: m_current() {}


	explicit GsCTimeVecRevIter( iterator_type x ) 
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

	GsCTimeVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsCTimeVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsCTimeVecRevIter operator++(int) 
	{
		GsCTimeVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsCTimeVecRevIter operator--(int) 
	{
		GsCTimeVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsCTimeVecRevIter operator+(
	const GsCTimeVecRevIter &iter,
	GsCTimeVecRevIter::difference_type n
) 
{
	return GsCTimeVecRevIter(iter.Class()-n);
}

inline GsCTimeVecRevIter &operator+=(
	GsCTimeVecRevIter &iter,
	GsCTimeVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsCTimeVecRevIter operator-(
	const GsCTimeVecRevIter &iter,
	GsCTimeVecRevIter::difference_type n
) 
{
	return GsCTimeVecRevIter( iter.Class() + n );
}

inline GsCTimeVecRevIter &operator-=(
	GsCTimeVecRevIter &iter,
	GsCTimeVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsCTimeVecRevIter &lop,
	const GsCTimeVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsCTimeVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsCTimeVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsCTime value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsCTime *pointer;
	typedef const GsCTime &reference;
	typedef const GsCTime *iterator_type;

	GsCTimeVecConstRevIter() 
		: m_current() {}

	explicit GsCTimeVecConstRevIter(iterator_type x) 
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

	GsCTimeVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsCTimeVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsCTimeVecConstRevIter operator++(int) 
		{ GsCTimeVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsCTimeVecConstRevIter operator--(int) 
		{ GsCTimeVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsCTimeVecConstRevIter  operator+(
	const GsCTimeVecConstRevIter &iter,
	GsCTimeVecConstRevIter::difference_type n
) 
{
	return GsCTimeVecConstRevIter(iter.Class()-n);
}

inline GsCTimeVecConstRevIter &operator+=(
	GsCTimeVecConstRevIter &iter,
	GsCTimeVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsCTimeVecConstRevIter operator-(
	const GsCTimeVecConstRevIter &iter,
	GsCTimeVecConstRevIter::difference_type n
) 
{
	return GsCTimeVecConstRevIter( iter.Class() + n );
}

inline GsCTimeVecConstRevIter &operator-=(
	GsCTimeVecConstRevIter &iter,
	GsCTimeVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsCTimeVecConstRevIter &lop,
	const GsCTimeVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsCTimeVectorRep
*/

class EXPORT_CLASS_GSDATE GsCTimeVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsCTime value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsCTime* iterator;
	typedef const GsCTime* const_iterator;
	typedef GsCTimeVecRevIter reverse_iterator;
	typedef GsCTimeVecConstRevIter const_reverse_iterator;

	typedef GsCTime &reference;
	typedef const GsCTime &const_reference;

	// Constructors
	GsCTimeVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsCTimeVectorRep(size_t);
	GsCTimeVectorRep(size_t,const value_type &x);
	GsCTimeVectorRep(const_iterator begin, const_iterator end);
	GsCTimeVectorRep(const GsCTimeVectorRep &other);

	// Destructor
	~GsCTimeVectorRep();

	// Assignment
	GsCTimeVectorRep &operator = (const GsCTimeVectorRep&);

	// Copy
	GsCTimeVectorRep *copy() { return new GsCTimeVectorRep( *this ); }

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

	bool operator==(const GsCTimeVectorRep &x) const;
	bool operator<(const GsCTimeVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsCTimeVector
*/

class EXPORT_CLASS_GSDATE GsCTimeVector : public GsHandleCopyOnWrite<GsCTimeVectorRep>
{
public:
	typedef GsCTimeVectorRep::value_type value_type;
	typedef GsCTimeVectorRep::size_type size_type;
	typedef GsCTimeVectorRep::difference_type difference_type;

	typedef GsCTimeVectorRep::iterator iterator;
	typedef GsCTimeVectorRep::const_iterator const_iterator;
	typedef GsCTimeVectorRep::reverse_iterator reverse_iterator;
	typedef GsCTimeVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsCTimeVectorRep::reference reference;
	typedef GsCTimeVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsCTimeVectorRep* const_rep;
public:

	// Constructors
	GsCTimeVector();
	GsCTimeVector(size_t);
	GsCTimeVector(size_t,const value_type &x);
	GsCTimeVector(const_iterator begin, const_iterator end);
	GsCTimeVector(const GsCTimeVector &other);

	// Destructor
	virtual ~GsCTimeVector() {}

	// Assignment
	GsCTimeVector &operator = (const GsCTimeVector&);

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

	bool operator==(const GsCTimeVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsCTimeVector &x) const { return (!(*this == x)); }
	bool operator<(const GsCTimeVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsCTimeVector &x) const { return (x < *this); }
	bool operator<=(const GsCTimeVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsCTimeVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsCTimeVector> GsDtType;
	typedef GsDtCTime GsDtTypeElem;
	inline static const char* typeName() { return "GsCTimeVector"; }
	GsString toString() const;
	int compare( const GsCTimeVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSCTIMEVECTOR_H) */


