/****************************************************************
**
**	gscore/GsDoubleVector.h	- Header for GsDoubleVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDOUBLEVECTOR_H)
#define IN_GSCORE_GSDOUBLEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsDouble.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsDoubleVector;
class GsDoubleVectorRep;
class GsDoubleVecRevIter;
class GsDoubleVecConstRevIter;

/*
**  class GsDoubleVecRevIter
*/

class EXPORT_CLASS_GSBASE GsDoubleVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDouble value_type;
	typedef ptrdiff_t difference_type;
	typedef GsDouble *pointer;
	typedef GsDouble &reference;
	typedef GsDouble *iterator_type;

	GsDoubleVecRevIter() 
		: m_current() {}


	explicit GsDoubleVecRevIter( iterator_type x ) 
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

	GsDoubleVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsDoubleVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsDoubleVecRevIter operator++(int) 
	{
		GsDoubleVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsDoubleVecRevIter operator--(int) 
	{
		GsDoubleVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsDoubleVecRevIter operator+(
	const GsDoubleVecRevIter &iter,
	GsDoubleVecRevIter::difference_type n
) 
{
	return GsDoubleVecRevIter(iter.Class()-n);
}

inline GsDoubleVecRevIter &operator+=(
	GsDoubleVecRevIter &iter,
	GsDoubleVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsDoubleVecRevIter operator-(
	const GsDoubleVecRevIter &iter,
	GsDoubleVecRevIter::difference_type n
) 
{
	return GsDoubleVecRevIter( iter.Class() + n );
}

inline GsDoubleVecRevIter &operator-=(
	GsDoubleVecRevIter &iter,
	GsDoubleVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDoubleVecRevIter &lop,
	const GsDoubleVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsDoubleVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsDoubleVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDouble value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsDouble *pointer;
	typedef const GsDouble &reference;
	typedef const GsDouble *iterator_type;

	GsDoubleVecConstRevIter() 
		: m_current() {}

	explicit GsDoubleVecConstRevIter(iterator_type x) 
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

	GsDoubleVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsDoubleVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsDoubleVecConstRevIter operator++(int) 
		{ GsDoubleVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsDoubleVecConstRevIter operator--(int) 
		{ GsDoubleVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsDoubleVecConstRevIter  operator+(
	const GsDoubleVecConstRevIter &iter,
	GsDoubleVecConstRevIter::difference_type n
) 
{
	return GsDoubleVecConstRevIter(iter.Class()-n);
}

inline GsDoubleVecConstRevIter &operator+=(
	GsDoubleVecConstRevIter &iter,
	GsDoubleVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsDoubleVecConstRevIter operator-(
	const GsDoubleVecConstRevIter &iter,
	GsDoubleVecConstRevIter::difference_type n
) 
{
	return GsDoubleVecConstRevIter( iter.Class() + n );
}

inline GsDoubleVecConstRevIter &operator-=(
	GsDoubleVecConstRevIter &iter,
	GsDoubleVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDoubleVecConstRevIter &lop,
	const GsDoubleVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsDoubleVectorRep
*/

class EXPORT_CLASS_GSBASE GsDoubleVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsDouble value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsDouble* iterator;
	typedef const GsDouble* const_iterator;
	typedef GsDoubleVecRevIter reverse_iterator;
	typedef GsDoubleVecConstRevIter const_reverse_iterator;

	typedef GsDouble &reference;
	typedef const GsDouble &const_reference;

	// Constructors
	GsDoubleVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsDoubleVectorRep(size_t);
	GsDoubleVectorRep(size_t,const value_type &x);
	GsDoubleVectorRep(const_iterator begin, const_iterator end);
	GsDoubleVectorRep(const GsDoubleVectorRep &other);

	// Destructor
	~GsDoubleVectorRep();

	// Assignment
	GsDoubleVectorRep &operator = (const GsDoubleVectorRep&);

	// Copy
	GsDoubleVectorRep *copy() { return new GsDoubleVectorRep( *this ); }

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

	bool operator==(const GsDoubleVectorRep &x) const;
	bool operator<(const GsDoubleVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsDoubleVector
*/

class EXPORT_CLASS_GSBASE GsDoubleVector : public GsHandleCopyOnWrite<GsDoubleVectorRep>
{
public:
	typedef GsDoubleVectorRep::value_type value_type;
	typedef GsDoubleVectorRep::size_type size_type;
	typedef GsDoubleVectorRep::difference_type difference_type;

	typedef GsDoubleVectorRep::iterator iterator;
	typedef GsDoubleVectorRep::const_iterator const_iterator;
	typedef GsDoubleVectorRep::reverse_iterator reverse_iterator;
	typedef GsDoubleVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsDoubleVectorRep::reference reference;
	typedef GsDoubleVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsDoubleVectorRep* const_rep;
public:

	// Constructors
	GsDoubleVector();
	GsDoubleVector(size_t);
	GsDoubleVector(size_t,const value_type &x);
	GsDoubleVector(const_iterator begin, const_iterator end);
	GsDoubleVector(const GsDoubleVector &other);

	// Destructor
	virtual ~GsDoubleVector() {}

	// Assignment
	GsDoubleVector &operator = (const GsDoubleVector&);

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

	bool operator==(const GsDoubleVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsDoubleVector &x) const { return (!(*this == x)); }
	bool operator<(const GsDoubleVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsDoubleVector &x) const { return (x < *this); }
	bool operator<=(const GsDoubleVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsDoubleVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsDoubleVector> GsDtType;
	typedef GsDtDouble GsDtTypeElem;
	inline static const char* typeName() { return "GsDoubleVector"; }
	GsString toString() const;
	int compare( const GsDoubleVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDOUBLEVECTOR_H) */


