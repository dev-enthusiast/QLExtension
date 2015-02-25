/****************************************************************
**
**	gssymexpr/GsSymbolicExpressionVector.h	- Header for GsSymbolicExpressionVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONVECTOR_H)
#define IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gssymexpr/GsSymbolicExpression.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsSymbolicExpressionVector;
class GsSymbolicExpressionVectorRep;
class GsSymbolicExpressionVecRevIter;
class GsSymbolicExpressionVecConstRevIter;

/*
**  class GsSymbolicExpressionVecRevIter
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbolicExpression value_type;
	typedef ptrdiff_t difference_type;
	typedef GsSymbolicExpression *pointer;
	typedef GsSymbolicExpression &reference;
	typedef GsSymbolicExpression *iterator_type;

	GsSymbolicExpressionVecRevIter() 
		: m_current() {}


	explicit GsSymbolicExpressionVecRevIter( iterator_type x ) 
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

	GsSymbolicExpressionVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsSymbolicExpressionVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsSymbolicExpressionVecRevIter operator++(int) 
	{
		GsSymbolicExpressionVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsSymbolicExpressionVecRevIter operator--(int) 
	{
		GsSymbolicExpressionVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsSymbolicExpressionVecRevIter operator+(
	const GsSymbolicExpressionVecRevIter &iter,
	GsSymbolicExpressionVecRevIter::difference_type n
) 
{
	return GsSymbolicExpressionVecRevIter(iter.Class()-n);
}

inline GsSymbolicExpressionVecRevIter &operator+=(
	GsSymbolicExpressionVecRevIter &iter,
	GsSymbolicExpressionVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsSymbolicExpressionVecRevIter operator-(
	const GsSymbolicExpressionVecRevIter &iter,
	GsSymbolicExpressionVecRevIter::difference_type n
) 
{
	return GsSymbolicExpressionVecRevIter( iter.Class() + n );
}

inline GsSymbolicExpressionVecRevIter &operator-=(
	GsSymbolicExpressionVecRevIter &iter,
	GsSymbolicExpressionVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolicExpressionVecRevIter &lop,
	const GsSymbolicExpressionVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsSymbolicExpressionVecConstRevIter
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbolicExpression value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsSymbolicExpression *pointer;
	typedef const GsSymbolicExpression &reference;
	typedef const GsSymbolicExpression *iterator_type;

	GsSymbolicExpressionVecConstRevIter() 
		: m_current() {}

	explicit GsSymbolicExpressionVecConstRevIter(iterator_type x) 
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

	GsSymbolicExpressionVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsSymbolicExpressionVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsSymbolicExpressionVecConstRevIter operator++(int) 
		{ GsSymbolicExpressionVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsSymbolicExpressionVecConstRevIter operator--(int) 
		{ GsSymbolicExpressionVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsSymbolicExpressionVecConstRevIter  operator+(
	const GsSymbolicExpressionVecConstRevIter &iter,
	GsSymbolicExpressionVecConstRevIter::difference_type n
) 
{
	return GsSymbolicExpressionVecConstRevIter(iter.Class()-n);
}

inline GsSymbolicExpressionVecConstRevIter &operator+=(
	GsSymbolicExpressionVecConstRevIter &iter,
	GsSymbolicExpressionVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsSymbolicExpressionVecConstRevIter operator-(
	const GsSymbolicExpressionVecConstRevIter &iter,
	GsSymbolicExpressionVecConstRevIter::difference_type n
) 
{
	return GsSymbolicExpressionVecConstRevIter( iter.Class() + n );
}

inline GsSymbolicExpressionVecConstRevIter &operator-=(
	GsSymbolicExpressionVecConstRevIter &iter,
	GsSymbolicExpressionVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolicExpressionVecConstRevIter &lop,
	const GsSymbolicExpressionVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsSymbolicExpressionVectorRep
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsSymbolicExpression value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsSymbolicExpression* iterator;
	typedef const GsSymbolicExpression* const_iterator;
	typedef GsSymbolicExpressionVecRevIter reverse_iterator;
	typedef GsSymbolicExpressionVecConstRevIter const_reverse_iterator;

	typedef GsSymbolicExpression &reference;
	typedef const GsSymbolicExpression &const_reference;

	// Constructors
	GsSymbolicExpressionVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsSymbolicExpressionVectorRep(size_t);
	GsSymbolicExpressionVectorRep(size_t,const value_type &x);
	GsSymbolicExpressionVectorRep(const_iterator begin, const_iterator end);
	GsSymbolicExpressionVectorRep(const GsSymbolicExpressionVectorRep &other);

	// Destructor
	~GsSymbolicExpressionVectorRep();

	// Assignment
	GsSymbolicExpressionVectorRep &operator = (const GsSymbolicExpressionVectorRep&);

	// Copy
	GsSymbolicExpressionVectorRep *copy() { return new GsSymbolicExpressionVectorRep( *this ); }

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

	bool operator==(const GsSymbolicExpressionVectorRep &x) const;
	bool operator<(const GsSymbolicExpressionVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsSymbolicExpressionVector
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicExpressionVector : public GsHandleCopyOnWrite<GsSymbolicExpressionVectorRep>
{
public:
	typedef GsSymbolicExpressionVectorRep::value_type value_type;
	typedef GsSymbolicExpressionVectorRep::size_type size_type;
	typedef GsSymbolicExpressionVectorRep::difference_type difference_type;

	typedef GsSymbolicExpressionVectorRep::iterator iterator;
	typedef GsSymbolicExpressionVectorRep::const_iterator const_iterator;
	typedef GsSymbolicExpressionVectorRep::reverse_iterator reverse_iterator;
	typedef GsSymbolicExpressionVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsSymbolicExpressionVectorRep::reference reference;
	typedef GsSymbolicExpressionVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsSymbolicExpressionVectorRep* const_rep;
public:

	// Constructors
	GsSymbolicExpressionVector();
	GsSymbolicExpressionVector(size_t);
	GsSymbolicExpressionVector(size_t,const value_type &x);
	GsSymbolicExpressionVector(const_iterator begin, const_iterator end);
	GsSymbolicExpressionVector(const GsSymbolicExpressionVector &other);

	// Destructor
	virtual ~GsSymbolicExpressionVector() {}

	// Assignment
	GsSymbolicExpressionVector &operator = (const GsSymbolicExpressionVector&);

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

	bool operator==(const GsSymbolicExpressionVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsSymbolicExpressionVector &x) const { return (!(*this == x)); }
	bool operator<(const GsSymbolicExpressionVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsSymbolicExpressionVector &x) const { return (x < *this); }
	bool operator<=(const GsSymbolicExpressionVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsSymbolicExpressionVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsSymbolicExpressionVector> GsDtType;
	typedef GsDtSymbolicExpression GsDtTypeElem;
	inline static const char* typeName() { return "GsSymbolicExpressionVector"; }
	GsString toString() const;
	int compare( const GsSymbolicExpressionVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSSYMEXPR_GSSYMBOLICEXPRESSIONVECTOR_H) */


