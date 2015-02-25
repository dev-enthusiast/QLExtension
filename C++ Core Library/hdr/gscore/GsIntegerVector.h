/****************************************************************
**
**	gscore/GsIntegerVector.h	- Header for GsIntegerVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSINTEGERVECTOR_H)
#define IN_GSCORE_GSINTEGERVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsInteger.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsIntegerVector;
class GsIntegerVectorRep;
class GsIntegerVecRevIter;
class GsIntegerVecConstRevIter;

/*
**  class GsIntegerVecRevIter
*/

class EXPORT_CLASS_GSBASE GsIntegerVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsInteger value_type;
	typedef ptrdiff_t difference_type;
	typedef GsInteger *pointer;
	typedef GsInteger &reference;
	typedef GsInteger *iterator_type;

	GsIntegerVecRevIter() 
		: m_current() {}


	explicit GsIntegerVecRevIter( iterator_type x ) 
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

	GsIntegerVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsIntegerVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsIntegerVecRevIter operator++(int) 
	{
		GsIntegerVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsIntegerVecRevIter operator--(int) 
	{
		GsIntegerVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsIntegerVecRevIter operator+(
	const GsIntegerVecRevIter &iter,
	GsIntegerVecRevIter::difference_type n
) 
{
	return GsIntegerVecRevIter(iter.Class()-n);
}

inline GsIntegerVecRevIter &operator+=(
	GsIntegerVecRevIter &iter,
	GsIntegerVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsIntegerVecRevIter operator-(
	const GsIntegerVecRevIter &iter,
	GsIntegerVecRevIter::difference_type n
) 
{
	return GsIntegerVecRevIter( iter.Class() + n );
}

inline GsIntegerVecRevIter &operator-=(
	GsIntegerVecRevIter &iter,
	GsIntegerVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsIntegerVecRevIter &lop,
	const GsIntegerVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsIntegerVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsIntegerVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsInteger value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsInteger *pointer;
	typedef const GsInteger &reference;
	typedef const GsInteger *iterator_type;

	GsIntegerVecConstRevIter() 
		: m_current() {}

	explicit GsIntegerVecConstRevIter(iterator_type x) 
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

	GsIntegerVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsIntegerVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsIntegerVecConstRevIter operator++(int) 
		{ GsIntegerVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsIntegerVecConstRevIter operator--(int) 
		{ GsIntegerVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsIntegerVecConstRevIter  operator+(
	const GsIntegerVecConstRevIter &iter,
	GsIntegerVecConstRevIter::difference_type n
) 
{
	return GsIntegerVecConstRevIter(iter.Class()-n);
}

inline GsIntegerVecConstRevIter &operator+=(
	GsIntegerVecConstRevIter &iter,
	GsIntegerVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsIntegerVecConstRevIter operator-(
	const GsIntegerVecConstRevIter &iter,
	GsIntegerVecConstRevIter::difference_type n
) 
{
	return GsIntegerVecConstRevIter( iter.Class() + n );
}

inline GsIntegerVecConstRevIter &operator-=(
	GsIntegerVecConstRevIter &iter,
	GsIntegerVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsIntegerVecConstRevIter &lop,
	const GsIntegerVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsIntegerVectorRep
*/

class EXPORT_CLASS_GSBASE GsIntegerVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsInteger value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsInteger* iterator;
	typedef const GsInteger* const_iterator;
	typedef GsIntegerVecRevIter reverse_iterator;
	typedef GsIntegerVecConstRevIter const_reverse_iterator;

	typedef GsInteger &reference;
	typedef const GsInteger &const_reference;

	// Constructors
	GsIntegerVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsIntegerVectorRep(size_t);
	GsIntegerVectorRep(size_t,const value_type &x);
	GsIntegerVectorRep(const_iterator begin, const_iterator end);
	GsIntegerVectorRep(const GsIntegerVectorRep &other);

	// Destructor
	~GsIntegerVectorRep();

	// Assignment
	GsIntegerVectorRep &operator = (const GsIntegerVectorRep&);

	// Copy
	GsIntegerVectorRep *copy() { return new GsIntegerVectorRep( *this ); }

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

	bool operator==(const GsIntegerVectorRep &x) const;
	bool operator<(const GsIntegerVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsIntegerVector
*/

class EXPORT_CLASS_GSBASE GsIntegerVector : public GsHandleCopyOnWrite<GsIntegerVectorRep>
{
public:
	typedef GsIntegerVectorRep::value_type value_type;
	typedef GsIntegerVectorRep::size_type size_type;
	typedef GsIntegerVectorRep::difference_type difference_type;

	typedef GsIntegerVectorRep::iterator iterator;
	typedef GsIntegerVectorRep::const_iterator const_iterator;
	typedef GsIntegerVectorRep::reverse_iterator reverse_iterator;
	typedef GsIntegerVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsIntegerVectorRep::reference reference;
	typedef GsIntegerVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsIntegerVectorRep* const_rep;
public:

	// Constructors
	GsIntegerVector();
	GsIntegerVector(size_t);
	GsIntegerVector(size_t,const value_type &x);
	GsIntegerVector(const_iterator begin, const_iterator end);
	GsIntegerVector(const GsIntegerVector &other);

	// Destructor
	virtual ~GsIntegerVector() {}

	// Assignment
	GsIntegerVector &operator = (const GsIntegerVector&);

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

	bool operator==(const GsIntegerVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsIntegerVector &x) const { return (!(*this == x)); }
	bool operator<(const GsIntegerVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsIntegerVector &x) const { return (x < *this); }
	bool operator<=(const GsIntegerVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsIntegerVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsIntegerVector> GsDtType;
	typedef GsDtInteger GsDtTypeElem;
	inline static const char* typeName() { return "GsIntegerVector"; }
	GsString toString() const;
	int compare( const GsIntegerVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSINTEGERVECTOR_H) */


