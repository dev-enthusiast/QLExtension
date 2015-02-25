/****************************************************************
**
**	gscore/GsDayCountVector.h	- Header for GsDayCountVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDAYCOUNTVECTOR_H)
#define IN_GSCORE_GSDAYCOUNTVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsDayCount.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsDayCountVector;
class GsDayCountVectorRep;
class GsDayCountVecRevIter;
class GsDayCountVecConstRevIter;

/*
**  class GsDayCountVecRevIter
*/

class EXPORT_CLASS_GSDATE GsDayCountVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDayCount value_type;
	typedef ptrdiff_t difference_type;
	typedef GsDayCount *pointer;
	typedef GsDayCount &reference;
	typedef GsDayCount *iterator_type;

	GsDayCountVecRevIter() 
		: m_current() {}


	explicit GsDayCountVecRevIter( iterator_type x ) 
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

	GsDayCountVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsDayCountVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsDayCountVecRevIter operator++(int) 
	{
		GsDayCountVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsDayCountVecRevIter operator--(int) 
	{
		GsDayCountVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsDayCountVecRevIter operator+(
	const GsDayCountVecRevIter &iter,
	GsDayCountVecRevIter::difference_type n
) 
{
	return GsDayCountVecRevIter(iter.Class()-n);
}

inline GsDayCountVecRevIter &operator+=(
	GsDayCountVecRevIter &iter,
	GsDayCountVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsDayCountVecRevIter operator-(
	const GsDayCountVecRevIter &iter,
	GsDayCountVecRevIter::difference_type n
) 
{
	return GsDayCountVecRevIter( iter.Class() + n );
}

inline GsDayCountVecRevIter &operator-=(
	GsDayCountVecRevIter &iter,
	GsDayCountVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDayCountVecRevIter &lop,
	const GsDayCountVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsDayCountVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsDayCountVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDayCount value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsDayCount *pointer;
	typedef const GsDayCount &reference;
	typedef const GsDayCount *iterator_type;

	GsDayCountVecConstRevIter() 
		: m_current() {}

	explicit GsDayCountVecConstRevIter(iterator_type x) 
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

	GsDayCountVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsDayCountVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsDayCountVecConstRevIter operator++(int) 
		{ GsDayCountVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsDayCountVecConstRevIter operator--(int) 
		{ GsDayCountVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsDayCountVecConstRevIter  operator+(
	const GsDayCountVecConstRevIter &iter,
	GsDayCountVecConstRevIter::difference_type n
) 
{
	return GsDayCountVecConstRevIter(iter.Class()-n);
}

inline GsDayCountVecConstRevIter &operator+=(
	GsDayCountVecConstRevIter &iter,
	GsDayCountVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsDayCountVecConstRevIter operator-(
	const GsDayCountVecConstRevIter &iter,
	GsDayCountVecConstRevIter::difference_type n
) 
{
	return GsDayCountVecConstRevIter( iter.Class() + n );
}

inline GsDayCountVecConstRevIter &operator-=(
	GsDayCountVecConstRevIter &iter,
	GsDayCountVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDayCountVecConstRevIter &lop,
	const GsDayCountVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsDayCountVectorRep
*/

class EXPORT_CLASS_GSDATE GsDayCountVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsDayCount value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsDayCount* iterator;
	typedef const GsDayCount* const_iterator;
	typedef GsDayCountVecRevIter reverse_iterator;
	typedef GsDayCountVecConstRevIter const_reverse_iterator;

	typedef GsDayCount &reference;
	typedef const GsDayCount &const_reference;

	// Constructors
	GsDayCountVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsDayCountVectorRep(size_t);
	GsDayCountVectorRep(size_t,const value_type &x);
	GsDayCountVectorRep(const_iterator begin, const_iterator end);
	GsDayCountVectorRep(const GsDayCountVectorRep &other);

	// Destructor
	~GsDayCountVectorRep();

	// Assignment
	GsDayCountVectorRep &operator = (const GsDayCountVectorRep&);

	// Copy
	GsDayCountVectorRep *copy() { return new GsDayCountVectorRep( *this ); }

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

	bool operator==(const GsDayCountVectorRep &x) const;
	bool operator<(const GsDayCountVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsDayCountVector
*/

class EXPORT_CLASS_GSDATE GsDayCountVector : public GsHandleCopyOnWrite<GsDayCountVectorRep>
{
public:
	typedef GsDayCountVectorRep::value_type value_type;
	typedef GsDayCountVectorRep::size_type size_type;
	typedef GsDayCountVectorRep::difference_type difference_type;

	typedef GsDayCountVectorRep::iterator iterator;
	typedef GsDayCountVectorRep::const_iterator const_iterator;
	typedef GsDayCountVectorRep::reverse_iterator reverse_iterator;
	typedef GsDayCountVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsDayCountVectorRep::reference reference;
	typedef GsDayCountVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsDayCountVectorRep* const_rep;
public:

	// Constructors
	GsDayCountVector();
	GsDayCountVector(size_t);
	GsDayCountVector(size_t,const value_type &x);
	GsDayCountVector(const_iterator begin, const_iterator end);
	GsDayCountVector(const GsDayCountVector &other);

	// Destructor
	virtual ~GsDayCountVector() {}

	// Assignment
	GsDayCountVector &operator = (const GsDayCountVector&);

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

	bool operator==(const GsDayCountVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsDayCountVector &x) const { return (!(*this == x)); }
	bool operator<(const GsDayCountVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsDayCountVector &x) const { return (x < *this); }
	bool operator<=(const GsDayCountVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsDayCountVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsDayCountVector> GsDtType;
	typedef GsDtDayCount GsDtTypeElem;
	inline static const char* typeName() { return "GsDayCountVector"; }
	GsString toString() const;
	int compare( const GsDayCountVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDAYCOUNTVECTOR_H) */


