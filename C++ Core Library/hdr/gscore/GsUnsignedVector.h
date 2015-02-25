/****************************************************************
**
**	gscore/GsUnsignedVector.h	- Header for GsUnsignedVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSUNSIGNEDVECTOR_H)
#define IN_GSCORE_GSUNSIGNEDVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsUnsigned.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsUnsignedVector;
class GsUnsignedVectorRep;
class GsUnsignedVecRevIter;
class GsUnsignedVecConstRevIter;

/*
**  class GsUnsignedVecRevIter
*/

class EXPORT_CLASS_GSBASE GsUnsignedVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsUnsigned value_type;
	typedef ptrdiff_t difference_type;
	typedef GsUnsigned *pointer;
	typedef GsUnsigned &reference;
	typedef GsUnsigned *iterator_type;

	GsUnsignedVecRevIter() 
		: m_current() {}


	explicit GsUnsignedVecRevIter( iterator_type x ) 
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

	GsUnsignedVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsUnsignedVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsUnsignedVecRevIter operator++(int) 
	{
		GsUnsignedVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsUnsignedVecRevIter operator--(int) 
	{
		GsUnsignedVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsUnsignedVecRevIter operator+(
	const GsUnsignedVecRevIter &iter,
	GsUnsignedVecRevIter::difference_type n
) 
{
	return GsUnsignedVecRevIter(iter.Class()-n);
}

inline GsUnsignedVecRevIter &operator+=(
	GsUnsignedVecRevIter &iter,
	GsUnsignedVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsUnsignedVecRevIter operator-(
	const GsUnsignedVecRevIter &iter,
	GsUnsignedVecRevIter::difference_type n
) 
{
	return GsUnsignedVecRevIter( iter.Class() + n );
}

inline GsUnsignedVecRevIter &operator-=(
	GsUnsignedVecRevIter &iter,
	GsUnsignedVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsUnsignedVecRevIter &lop,
	const GsUnsignedVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsUnsignedVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsUnsignedVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsUnsigned value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsUnsigned *pointer;
	typedef const GsUnsigned &reference;
	typedef const GsUnsigned *iterator_type;

	GsUnsignedVecConstRevIter() 
		: m_current() {}

	explicit GsUnsignedVecConstRevIter(iterator_type x) 
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

	GsUnsignedVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsUnsignedVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsUnsignedVecConstRevIter operator++(int) 
		{ GsUnsignedVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsUnsignedVecConstRevIter operator--(int) 
		{ GsUnsignedVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsUnsignedVecConstRevIter  operator+(
	const GsUnsignedVecConstRevIter &iter,
	GsUnsignedVecConstRevIter::difference_type n
) 
{
	return GsUnsignedVecConstRevIter(iter.Class()-n);
}

inline GsUnsignedVecConstRevIter &operator+=(
	GsUnsignedVecConstRevIter &iter,
	GsUnsignedVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsUnsignedVecConstRevIter operator-(
	const GsUnsignedVecConstRevIter &iter,
	GsUnsignedVecConstRevIter::difference_type n
) 
{
	return GsUnsignedVecConstRevIter( iter.Class() + n );
}

inline GsUnsignedVecConstRevIter &operator-=(
	GsUnsignedVecConstRevIter &iter,
	GsUnsignedVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsUnsignedVecConstRevIter &lop,
	const GsUnsignedVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsUnsignedVectorRep
*/

class EXPORT_CLASS_GSBASE GsUnsignedVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsUnsigned value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsUnsigned* iterator;
	typedef const GsUnsigned* const_iterator;
	typedef GsUnsignedVecRevIter reverse_iterator;
	typedef GsUnsignedVecConstRevIter const_reverse_iterator;

	typedef GsUnsigned &reference;
	typedef const GsUnsigned &const_reference;

	// Constructors
	GsUnsignedVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsUnsignedVectorRep(size_t);
	GsUnsignedVectorRep(size_t,const value_type &x);
	GsUnsignedVectorRep(const_iterator begin, const_iterator end);
	GsUnsignedVectorRep(const GsUnsignedVectorRep &other);

	// Destructor
	~GsUnsignedVectorRep();

	// Assignment
	GsUnsignedVectorRep &operator = (const GsUnsignedVectorRep&);

	// Copy
	GsUnsignedVectorRep *copy() { return new GsUnsignedVectorRep( *this ); }

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

	bool operator==(const GsUnsignedVectorRep &x) const;
	bool operator<(const GsUnsignedVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsUnsignedVector
*/

class EXPORT_CLASS_GSBASE GsUnsignedVector : public GsHandleCopyOnWrite<GsUnsignedVectorRep>
{
public:
	typedef GsUnsignedVectorRep::value_type value_type;
	typedef GsUnsignedVectorRep::size_type size_type;
	typedef GsUnsignedVectorRep::difference_type difference_type;

	typedef GsUnsignedVectorRep::iterator iterator;
	typedef GsUnsignedVectorRep::const_iterator const_iterator;
	typedef GsUnsignedVectorRep::reverse_iterator reverse_iterator;
	typedef GsUnsignedVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsUnsignedVectorRep::reference reference;
	typedef GsUnsignedVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsUnsignedVectorRep* const_rep;
public:

	// Constructors
	GsUnsignedVector();
	GsUnsignedVector(size_t);
	GsUnsignedVector(size_t,const value_type &x);
	GsUnsignedVector(const_iterator begin, const_iterator end);
	GsUnsignedVector(const GsUnsignedVector &other);

	// Destructor
	virtual ~GsUnsignedVector() {}

	// Assignment
	GsUnsignedVector &operator = (const GsUnsignedVector&);

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

	bool operator==(const GsUnsignedVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsUnsignedVector &x) const { return (!(*this == x)); }
	bool operator<(const GsUnsignedVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsUnsignedVector &x) const { return (x < *this); }
	bool operator<=(const GsUnsignedVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsUnsignedVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsUnsignedVector> GsDtType;
	typedef GsDtUnsigned GsDtTypeElem;
	inline static const char* typeName() { return "GsUnsignedVector"; }
	GsString toString() const;
	int compare( const GsUnsignedVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSUNSIGNEDVECTOR_H) */


