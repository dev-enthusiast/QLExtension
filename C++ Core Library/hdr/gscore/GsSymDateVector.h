/****************************************************************
**
**	gscore/GsSymDateVector.h	- Header for GsSymDateVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSSYMDATEVECTOR_H)
#define IN_GSCORE_GSSYMDATEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsSymDate.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsSymDateVector;
class GsSymDateVectorRep;
class GsSymDateVecRevIter;
class GsSymDateVecConstRevIter;

/*
**  class GsSymDateVecRevIter
*/

class EXPORT_CLASS_GSDATE GsSymDateVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymDate value_type;
	typedef ptrdiff_t difference_type;
	typedef GsSymDate *pointer;
	typedef GsSymDate &reference;
	typedef GsSymDate *iterator_type;

	GsSymDateVecRevIter() 
		: m_current() {}


	explicit GsSymDateVecRevIter( iterator_type x ) 
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

	GsSymDateVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsSymDateVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsSymDateVecRevIter operator++(int) 
	{
		GsSymDateVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsSymDateVecRevIter operator--(int) 
	{
		GsSymDateVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsSymDateVecRevIter operator+(
	const GsSymDateVecRevIter &iter,
	GsSymDateVecRevIter::difference_type n
) 
{
	return GsSymDateVecRevIter(iter.Class()-n);
}

inline GsSymDateVecRevIter &operator+=(
	GsSymDateVecRevIter &iter,
	GsSymDateVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsSymDateVecRevIter operator-(
	const GsSymDateVecRevIter &iter,
	GsSymDateVecRevIter::difference_type n
) 
{
	return GsSymDateVecRevIter( iter.Class() + n );
}

inline GsSymDateVecRevIter &operator-=(
	GsSymDateVecRevIter &iter,
	GsSymDateVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymDateVecRevIter &lop,
	const GsSymDateVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsSymDateVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsSymDateVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymDate value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsSymDate *pointer;
	typedef const GsSymDate &reference;
	typedef const GsSymDate *iterator_type;

	GsSymDateVecConstRevIter() 
		: m_current() {}

	explicit GsSymDateVecConstRevIter(iterator_type x) 
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

	GsSymDateVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsSymDateVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsSymDateVecConstRevIter operator++(int) 
		{ GsSymDateVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsSymDateVecConstRevIter operator--(int) 
		{ GsSymDateVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsSymDateVecConstRevIter  operator+(
	const GsSymDateVecConstRevIter &iter,
	GsSymDateVecConstRevIter::difference_type n
) 
{
	return GsSymDateVecConstRevIter(iter.Class()-n);
}

inline GsSymDateVecConstRevIter &operator+=(
	GsSymDateVecConstRevIter &iter,
	GsSymDateVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsSymDateVecConstRevIter operator-(
	const GsSymDateVecConstRevIter &iter,
	GsSymDateVecConstRevIter::difference_type n
) 
{
	return GsSymDateVecConstRevIter( iter.Class() + n );
}

inline GsSymDateVecConstRevIter &operator-=(
	GsSymDateVecConstRevIter &iter,
	GsSymDateVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymDateVecConstRevIter &lop,
	const GsSymDateVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsSymDateVectorRep
*/

class EXPORT_CLASS_GSDATE GsSymDateVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsSymDate value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsSymDate* iterator;
	typedef const GsSymDate* const_iterator;
	typedef GsSymDateVecRevIter reverse_iterator;
	typedef GsSymDateVecConstRevIter const_reverse_iterator;

	typedef GsSymDate &reference;
	typedef const GsSymDate &const_reference;

	// Constructors
	GsSymDateVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsSymDateVectorRep(size_t);
	GsSymDateVectorRep(size_t,const value_type &x);
	GsSymDateVectorRep(const_iterator begin, const_iterator end);
	GsSymDateVectorRep(const GsSymDateVectorRep &other);

	// Destructor
	~GsSymDateVectorRep();

	// Assignment
	GsSymDateVectorRep &operator = (const GsSymDateVectorRep&);

	// Copy
	GsSymDateVectorRep *copy() { return new GsSymDateVectorRep( *this ); }

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

	bool operator==(const GsSymDateVectorRep &x) const;
	bool operator<(const GsSymDateVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsSymDateVector
*/

class EXPORT_CLASS_GSDATE GsSymDateVector : public GsHandleCopyOnWrite<GsSymDateVectorRep>
{
public:
	typedef GsSymDateVectorRep::value_type value_type;
	typedef GsSymDateVectorRep::size_type size_type;
	typedef GsSymDateVectorRep::difference_type difference_type;

	typedef GsSymDateVectorRep::iterator iterator;
	typedef GsSymDateVectorRep::const_iterator const_iterator;
	typedef GsSymDateVectorRep::reverse_iterator reverse_iterator;
	typedef GsSymDateVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsSymDateVectorRep::reference reference;
	typedef GsSymDateVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsSymDateVectorRep* const_rep;
public:

	// Constructors
	GsSymDateVector();
	GsSymDateVector(size_t);
	GsSymDateVector(size_t,const value_type &x);
	GsSymDateVector(const_iterator begin, const_iterator end);
	GsSymDateVector(const GsSymDateVector &other);

	// Destructor
	virtual ~GsSymDateVector() {}

	// Assignment
	GsSymDateVector &operator = (const GsSymDateVector&);

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

	bool operator==(const GsSymDateVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsSymDateVector &x) const { return (!(*this == x)); }
	bool operator<(const GsSymDateVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsSymDateVector &x) const { return (x < *this); }
	bool operator<=(const GsSymDateVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsSymDateVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsSymDateVector> GsDtType;
	typedef GsDtSymDate GsDtTypeElem;
	inline static const char* typeName() { return "GsSymDateVector"; }
	GsString toString() const;
	int compare( const GsSymDateVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSSYMDATEVECTOR_H) */


