/****************************************************************
**
**	gscore/GsDayCountISDAVector.h	- Header for GsDayCountISDAVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDAYCOUNTISDAVECTOR_H)
#define IN_GSCORE_GSDAYCOUNTISDAVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsDayCountISDA.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsDayCountISDAVector;
class GsDayCountISDAVectorRep;
class GsDayCountISDAVecRevIter;
class GsDayCountISDAVecConstRevIter;

/*
**  class GsDayCountISDAVecRevIter
*/

class EXPORT_CLASS_GSDATE GsDayCountISDAVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDayCountISDA value_type;
	typedef ptrdiff_t difference_type;
	typedef GsDayCountISDA *pointer;
	typedef GsDayCountISDA &reference;
	typedef GsDayCountISDA *iterator_type;

	GsDayCountISDAVecRevIter() 
		: m_current() {}


	explicit GsDayCountISDAVecRevIter( iterator_type x ) 
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

	GsDayCountISDAVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsDayCountISDAVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsDayCountISDAVecRevIter operator++(int) 
	{
		GsDayCountISDAVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsDayCountISDAVecRevIter operator--(int) 
	{
		GsDayCountISDAVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsDayCountISDAVecRevIter operator+(
	const GsDayCountISDAVecRevIter &iter,
	GsDayCountISDAVecRevIter::difference_type n
) 
{
	return GsDayCountISDAVecRevIter(iter.Class()-n);
}

inline GsDayCountISDAVecRevIter &operator+=(
	GsDayCountISDAVecRevIter &iter,
	GsDayCountISDAVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsDayCountISDAVecRevIter operator-(
	const GsDayCountISDAVecRevIter &iter,
	GsDayCountISDAVecRevIter::difference_type n
) 
{
	return GsDayCountISDAVecRevIter( iter.Class() + n );
}

inline GsDayCountISDAVecRevIter &operator-=(
	GsDayCountISDAVecRevIter &iter,
	GsDayCountISDAVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDayCountISDAVecRevIter &lop,
	const GsDayCountISDAVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsDayCountISDAVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsDayCountISDAVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDayCountISDA value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsDayCountISDA *pointer;
	typedef const GsDayCountISDA &reference;
	typedef const GsDayCountISDA *iterator_type;

	GsDayCountISDAVecConstRevIter() 
		: m_current() {}

	explicit GsDayCountISDAVecConstRevIter(iterator_type x) 
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

	GsDayCountISDAVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsDayCountISDAVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsDayCountISDAVecConstRevIter operator++(int) 
		{ GsDayCountISDAVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsDayCountISDAVecConstRevIter operator--(int) 
		{ GsDayCountISDAVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsDayCountISDAVecConstRevIter  operator+(
	const GsDayCountISDAVecConstRevIter &iter,
	GsDayCountISDAVecConstRevIter::difference_type n
) 
{
	return GsDayCountISDAVecConstRevIter(iter.Class()-n);
}

inline GsDayCountISDAVecConstRevIter &operator+=(
	GsDayCountISDAVecConstRevIter &iter,
	GsDayCountISDAVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsDayCountISDAVecConstRevIter operator-(
	const GsDayCountISDAVecConstRevIter &iter,
	GsDayCountISDAVecConstRevIter::difference_type n
) 
{
	return GsDayCountISDAVecConstRevIter( iter.Class() + n );
}

inline GsDayCountISDAVecConstRevIter &operator-=(
	GsDayCountISDAVecConstRevIter &iter,
	GsDayCountISDAVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDayCountISDAVecConstRevIter &lop,
	const GsDayCountISDAVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsDayCountISDAVectorRep
*/

class EXPORT_CLASS_GSDATE GsDayCountISDAVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsDayCountISDA value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsDayCountISDA* iterator;
	typedef const GsDayCountISDA* const_iterator;
	typedef GsDayCountISDAVecRevIter reverse_iterator;
	typedef GsDayCountISDAVecConstRevIter const_reverse_iterator;

	typedef GsDayCountISDA &reference;
	typedef const GsDayCountISDA &const_reference;

	// Constructors
	GsDayCountISDAVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsDayCountISDAVectorRep(size_t);
	GsDayCountISDAVectorRep(size_t,const value_type &x);
	GsDayCountISDAVectorRep(const_iterator begin, const_iterator end);
	GsDayCountISDAVectorRep(const GsDayCountISDAVectorRep &other);

	// Destructor
	~GsDayCountISDAVectorRep();

	// Assignment
	GsDayCountISDAVectorRep &operator = (const GsDayCountISDAVectorRep&);

	// Copy
	GsDayCountISDAVectorRep *copy() { return new GsDayCountISDAVectorRep( *this ); }

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

	bool operator==(const GsDayCountISDAVectorRep &x) const;
	bool operator<(const GsDayCountISDAVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsDayCountISDAVector
*/

class EXPORT_CLASS_GSDATE GsDayCountISDAVector : public GsHandleCopyOnWrite<GsDayCountISDAVectorRep>
{
public:
	typedef GsDayCountISDAVectorRep::value_type value_type;
	typedef GsDayCountISDAVectorRep::size_type size_type;
	typedef GsDayCountISDAVectorRep::difference_type difference_type;

	typedef GsDayCountISDAVectorRep::iterator iterator;
	typedef GsDayCountISDAVectorRep::const_iterator const_iterator;
	typedef GsDayCountISDAVectorRep::reverse_iterator reverse_iterator;
	typedef GsDayCountISDAVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsDayCountISDAVectorRep::reference reference;
	typedef GsDayCountISDAVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsDayCountISDAVectorRep* const_rep;
public:

	// Constructors
	GsDayCountISDAVector();
	GsDayCountISDAVector(size_t);
	GsDayCountISDAVector(size_t,const value_type &x);
	GsDayCountISDAVector(const_iterator begin, const_iterator end);
	GsDayCountISDAVector(const GsDayCountISDAVector &other);

	// Destructor
	virtual ~GsDayCountISDAVector() {}

	// Assignment
	GsDayCountISDAVector &operator = (const GsDayCountISDAVector&);

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

	bool operator==(const GsDayCountISDAVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsDayCountISDAVector &x) const { return (!(*this == x)); }
	bool operator<(const GsDayCountISDAVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsDayCountISDAVector &x) const { return (x < *this); }
	bool operator<=(const GsDayCountISDAVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsDayCountISDAVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsDayCountISDAVector> GsDtType;
	typedef GsDtDayCountISDA GsDtTypeElem;
	inline static const char* typeName() { return "GsDayCountISDAVector"; }
	GsString toString() const;
	int compare( const GsDayCountISDAVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDAYCOUNTISDAVECTOR_H) */


