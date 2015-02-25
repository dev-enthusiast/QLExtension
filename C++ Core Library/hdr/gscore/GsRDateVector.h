/****************************************************************
**
**	gscore/GsRDateVector.h	- Header for GsRDateVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSRDATEVECTOR_H)
#define IN_GSCORE_GSRDATEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsRDate.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsRDateVector;
class GsRDateVectorRep;
class GsRDateVecRevIter;
class GsRDateVecConstRevIter;

/*
**  class GsRDateVecRevIter
*/

class EXPORT_CLASS_GSDATE GsRDateVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsRDate value_type;
	typedef ptrdiff_t difference_type;
	typedef GsRDate *pointer;
	typedef GsRDate &reference;
	typedef GsRDate *iterator_type;

	GsRDateVecRevIter() 
		: m_current() {}


	explicit GsRDateVecRevIter( iterator_type x ) 
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

	GsRDateVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsRDateVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsRDateVecRevIter operator++(int) 
	{
		GsRDateVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsRDateVecRevIter operator--(int) 
	{
		GsRDateVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsRDateVecRevIter operator+(
	const GsRDateVecRevIter &iter,
	GsRDateVecRevIter::difference_type n
) 
{
	return GsRDateVecRevIter(iter.Class()-n);
}

inline GsRDateVecRevIter &operator+=(
	GsRDateVecRevIter &iter,
	GsRDateVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsRDateVecRevIter operator-(
	const GsRDateVecRevIter &iter,
	GsRDateVecRevIter::difference_type n
) 
{
	return GsRDateVecRevIter( iter.Class() + n );
}

inline GsRDateVecRevIter &operator-=(
	GsRDateVecRevIter &iter,
	GsRDateVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsRDateVecRevIter &lop,
	const GsRDateVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsRDateVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsRDateVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsRDate value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsRDate *pointer;
	typedef const GsRDate &reference;
	typedef const GsRDate *iterator_type;

	GsRDateVecConstRevIter() 
		: m_current() {}

	explicit GsRDateVecConstRevIter(iterator_type x) 
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

	GsRDateVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsRDateVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsRDateVecConstRevIter operator++(int) 
		{ GsRDateVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsRDateVecConstRevIter operator--(int) 
		{ GsRDateVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsRDateVecConstRevIter  operator+(
	const GsRDateVecConstRevIter &iter,
	GsRDateVecConstRevIter::difference_type n
) 
{
	return GsRDateVecConstRevIter(iter.Class()-n);
}

inline GsRDateVecConstRevIter &operator+=(
	GsRDateVecConstRevIter &iter,
	GsRDateVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsRDateVecConstRevIter operator-(
	const GsRDateVecConstRevIter &iter,
	GsRDateVecConstRevIter::difference_type n
) 
{
	return GsRDateVecConstRevIter( iter.Class() + n );
}

inline GsRDateVecConstRevIter &operator-=(
	GsRDateVecConstRevIter &iter,
	GsRDateVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsRDateVecConstRevIter &lop,
	const GsRDateVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsRDateVectorRep
*/

class EXPORT_CLASS_GSDATE GsRDateVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsRDate value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsRDate* iterator;
	typedef const GsRDate* const_iterator;
	typedef GsRDateVecRevIter reverse_iterator;
	typedef GsRDateVecConstRevIter const_reverse_iterator;

	typedef GsRDate &reference;
	typedef const GsRDate &const_reference;

	// Constructors
	GsRDateVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsRDateVectorRep(size_t);
	GsRDateVectorRep(size_t,const value_type &x);
	GsRDateVectorRep(const_iterator begin, const_iterator end);
	GsRDateVectorRep(const GsRDateVectorRep &other);

	// Destructor
	~GsRDateVectorRep();

	// Assignment
	GsRDateVectorRep &operator = (const GsRDateVectorRep&);

	// Copy
	GsRDateVectorRep *copy() { return new GsRDateVectorRep( *this ); }

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

	bool operator==(const GsRDateVectorRep &x) const;
	bool operator<(const GsRDateVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsRDateVector
*/

class EXPORT_CLASS_GSDATE GsRDateVector : public GsHandleCopyOnWrite<GsRDateVectorRep>
{
public:
	typedef GsRDateVectorRep::value_type value_type;
	typedef GsRDateVectorRep::size_type size_type;
	typedef GsRDateVectorRep::difference_type difference_type;

	typedef GsRDateVectorRep::iterator iterator;
	typedef GsRDateVectorRep::const_iterator const_iterator;
	typedef GsRDateVectorRep::reverse_iterator reverse_iterator;
	typedef GsRDateVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsRDateVectorRep::reference reference;
	typedef GsRDateVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsRDateVectorRep* const_rep;
public:

	// Constructors
	GsRDateVector();
	GsRDateVector(size_t);
	GsRDateVector(size_t,const value_type &x);
	GsRDateVector(const_iterator begin, const_iterator end);
	GsRDateVector(const GsRDateVector &other);

	// Destructor
	virtual ~GsRDateVector() {}

	// Assignment
	GsRDateVector &operator = (const GsRDateVector&);

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

	bool operator==(const GsRDateVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsRDateVector &x) const { return (!(*this == x)); }
	bool operator<(const GsRDateVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsRDateVector &x) const { return (x < *this); }
	bool operator<=(const GsRDateVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsRDateVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsRDateVector> GsDtType;
	typedef GsDtRDate GsDtTypeElem;
	inline static const char* typeName() { return "GsRDateVector"; }
	GsString toString() const;
	int compare( const GsRDateVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSRDATEVECTOR_H) */


