/****************************************************************
**
**	gscore/GsSymbolVector.h	- Header for GsSymbolVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSSYMBOLVECTOR_H)
#define IN_GSCORE_GSSYMBOLVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsSymbol.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsSymbolVector;
class GsSymbolVectorRep;
class GsSymbolVecRevIter;
class GsSymbolVecConstRevIter;

/*
**  class GsSymbolVecRevIter
*/

class EXPORT_CLASS_GSBASE GsSymbolVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbol value_type;
	typedef ptrdiff_t difference_type;
	typedef GsSymbol *pointer;
	typedef GsSymbol &reference;
	typedef GsSymbol *iterator_type;

	GsSymbolVecRevIter() 
		: m_current() {}


	explicit GsSymbolVecRevIter( iterator_type x ) 
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

	GsSymbolVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsSymbolVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsSymbolVecRevIter operator++(int) 
	{
		GsSymbolVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsSymbolVecRevIter operator--(int) 
	{
		GsSymbolVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsSymbolVecRevIter operator+(
	const GsSymbolVecRevIter &iter,
	GsSymbolVecRevIter::difference_type n
) 
{
	return GsSymbolVecRevIter(iter.Class()-n);
}

inline GsSymbolVecRevIter &operator+=(
	GsSymbolVecRevIter &iter,
	GsSymbolVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsSymbolVecRevIter operator-(
	const GsSymbolVecRevIter &iter,
	GsSymbolVecRevIter::difference_type n
) 
{
	return GsSymbolVecRevIter( iter.Class() + n );
}

inline GsSymbolVecRevIter &operator-=(
	GsSymbolVecRevIter &iter,
	GsSymbolVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolVecRevIter &lop,
	const GsSymbolVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsSymbolVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsSymbolVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbol value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsSymbol *pointer;
	typedef const GsSymbol &reference;
	typedef const GsSymbol *iterator_type;

	GsSymbolVecConstRevIter() 
		: m_current() {}

	explicit GsSymbolVecConstRevIter(iterator_type x) 
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

	GsSymbolVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsSymbolVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsSymbolVecConstRevIter operator++(int) 
		{ GsSymbolVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsSymbolVecConstRevIter operator--(int) 
		{ GsSymbolVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsSymbolVecConstRevIter  operator+(
	const GsSymbolVecConstRevIter &iter,
	GsSymbolVecConstRevIter::difference_type n
) 
{
	return GsSymbolVecConstRevIter(iter.Class()-n);
}

inline GsSymbolVecConstRevIter &operator+=(
	GsSymbolVecConstRevIter &iter,
	GsSymbolVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsSymbolVecConstRevIter operator-(
	const GsSymbolVecConstRevIter &iter,
	GsSymbolVecConstRevIter::difference_type n
) 
{
	return GsSymbolVecConstRevIter( iter.Class() + n );
}

inline GsSymbolVecConstRevIter &operator-=(
	GsSymbolVecConstRevIter &iter,
	GsSymbolVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolVecConstRevIter &lop,
	const GsSymbolVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsSymbolVectorRep
*/

class EXPORT_CLASS_GSBASE GsSymbolVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsSymbol value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsSymbol* iterator;
	typedef const GsSymbol* const_iterator;
	typedef GsSymbolVecRevIter reverse_iterator;
	typedef GsSymbolVecConstRevIter const_reverse_iterator;

	typedef GsSymbol &reference;
	typedef const GsSymbol &const_reference;

	// Constructors
	GsSymbolVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsSymbolVectorRep(size_t);
	GsSymbolVectorRep(size_t,const value_type &x);
	GsSymbolVectorRep(const_iterator begin, const_iterator end);
	GsSymbolVectorRep(const GsSymbolVectorRep &other);

	// Destructor
	~GsSymbolVectorRep();

	// Assignment
	GsSymbolVectorRep &operator = (const GsSymbolVectorRep&);

	// Copy
	GsSymbolVectorRep *copy() { return new GsSymbolVectorRep( *this ); }

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

	bool operator==(const GsSymbolVectorRep &x) const;
	bool operator<(const GsSymbolVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsSymbolVector
*/

class EXPORT_CLASS_GSBASE GsSymbolVector : public GsHandleCopyOnWrite<GsSymbolVectorRep>
{
public:
	typedef GsSymbolVectorRep::value_type value_type;
	typedef GsSymbolVectorRep::size_type size_type;
	typedef GsSymbolVectorRep::difference_type difference_type;

	typedef GsSymbolVectorRep::iterator iterator;
	typedef GsSymbolVectorRep::const_iterator const_iterator;
	typedef GsSymbolVectorRep::reverse_iterator reverse_iterator;
	typedef GsSymbolVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsSymbolVectorRep::reference reference;
	typedef GsSymbolVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsSymbolVectorRep* const_rep;
public:

	// Constructors
	GsSymbolVector();
	GsSymbolVector(size_t);
	GsSymbolVector(size_t,const value_type &x);
	GsSymbolVector(const_iterator begin, const_iterator end);
	GsSymbolVector(const GsSymbolVector &other);

	// Destructor
	virtual ~GsSymbolVector() {}

	// Assignment
	GsSymbolVector &operator = (const GsSymbolVector&);

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

	bool operator==(const GsSymbolVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsSymbolVector &x) const { return (!(*this == x)); }
	bool operator<(const GsSymbolVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsSymbolVector &x) const { return (x < *this); }
	bool operator<=(const GsSymbolVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsSymbolVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsSymbolVector> GsDtType;
	typedef GsDtSymbol GsDtTypeElem;
	inline static const char* typeName() { return "GsSymbolVector"; }
	GsString toString() const;
	int compare( const GsSymbolVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSSYMBOLVECTOR_H) */


