/****************************************************************
**
**	gscore/GsDateVector.h	- Header for GsDateVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSDATEVECTOR_H)
#define IN_GSCORE_GSDATEVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsDate.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsDateVector;
class GsDateVectorRep;
class GsDateVecRevIter;
class GsDateVecConstRevIter;

/*
**  class GsDateVecRevIter
*/

class EXPORT_CLASS_GSDATE GsDateVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDate value_type;
	typedef ptrdiff_t difference_type;
	typedef GsDate *pointer;
	typedef GsDate &reference;
	typedef GsDate *iterator_type;

	GsDateVecRevIter() 
		: m_current() {}


	explicit GsDateVecRevIter( iterator_type x ) 
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

	GsDateVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsDateVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsDateVecRevIter operator++(int) 
	{
		GsDateVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsDateVecRevIter operator--(int) 
	{
		GsDateVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsDateVecRevIter operator+(
	const GsDateVecRevIter &iter,
	GsDateVecRevIter::difference_type n
) 
{
	return GsDateVecRevIter(iter.Class()-n);
}

inline GsDateVecRevIter &operator+=(
	GsDateVecRevIter &iter,
	GsDateVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsDateVecRevIter operator-(
	const GsDateVecRevIter &iter,
	GsDateVecRevIter::difference_type n
) 
{
	return GsDateVecRevIter( iter.Class() + n );
}

inline GsDateVecRevIter &operator-=(
	GsDateVecRevIter &iter,
	GsDateVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDateVecRevIter &lop,
	const GsDateVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsDateVecConstRevIter
*/

class EXPORT_CLASS_GSDATE GsDateVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsDate value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsDate *pointer;
	typedef const GsDate &reference;
	typedef const GsDate *iterator_type;

	GsDateVecConstRevIter() 
		: m_current() {}

	explicit GsDateVecConstRevIter(iterator_type x) 
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

	GsDateVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsDateVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsDateVecConstRevIter operator++(int) 
		{ GsDateVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsDateVecConstRevIter operator--(int) 
		{ GsDateVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsDateVecConstRevIter  operator+(
	const GsDateVecConstRevIter &iter,
	GsDateVecConstRevIter::difference_type n
) 
{
	return GsDateVecConstRevIter(iter.Class()-n);
}

inline GsDateVecConstRevIter &operator+=(
	GsDateVecConstRevIter &iter,
	GsDateVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsDateVecConstRevIter operator-(
	const GsDateVecConstRevIter &iter,
	GsDateVecConstRevIter::difference_type n
) 
{
	return GsDateVecConstRevIter( iter.Class() + n );
}

inline GsDateVecConstRevIter &operator-=(
	GsDateVecConstRevIter &iter,
	GsDateVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsDateVecConstRevIter &lop,
	const GsDateVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsDateVectorRep
*/

class EXPORT_CLASS_GSDATE GsDateVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsDate value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsDate* iterator;
	typedef const GsDate* const_iterator;
	typedef GsDateVecRevIter reverse_iterator;
	typedef GsDateVecConstRevIter const_reverse_iterator;

	typedef GsDate &reference;
	typedef const GsDate &const_reference;

	// Constructors
	GsDateVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsDateVectorRep(size_t);
	GsDateVectorRep(size_t,const value_type &x);
	GsDateVectorRep(const_iterator begin, const_iterator end);
	GsDateVectorRep(const GsDateVectorRep &other);

	// Destructor
	~GsDateVectorRep();

	// Assignment
	GsDateVectorRep &operator = (const GsDateVectorRep&);

	// Copy
	GsDateVectorRep *copy() { return new GsDateVectorRep( *this ); }

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

	bool operator==(const GsDateVectorRep &x) const;
	bool operator<(const GsDateVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsDateVector
*/

class EXPORT_CLASS_GSDATE GsDateVector : public GsHandleCopyOnWrite<GsDateVectorRep>
{
public:
	typedef GsDateVectorRep::value_type value_type;
	typedef GsDateVectorRep::size_type size_type;
	typedef GsDateVectorRep::difference_type difference_type;

	typedef GsDateVectorRep::iterator iterator;
	typedef GsDateVectorRep::const_iterator const_iterator;
	typedef GsDateVectorRep::reverse_iterator reverse_iterator;
	typedef GsDateVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsDateVectorRep::reference reference;
	typedef GsDateVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsDateVectorRep* const_rep;
public:

	// Constructors
	GsDateVector();
	GsDateVector(size_t);
	GsDateVector(size_t,const value_type &x);
	GsDateVector(const_iterator begin, const_iterator end);
	GsDateVector(const GsDateVector &other);

	// Destructor
	virtual ~GsDateVector() {}

	// Assignment
	GsDateVector &operator = (const GsDateVector&);

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

	bool operator==(const GsDateVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsDateVector &x) const { return (!(*this == x)); }
	bool operator<(const GsDateVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsDateVector &x) const { return (x < *this); }
	bool operator<=(const GsDateVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsDateVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsDateVector> GsDtType;
	typedef GsDtDate GsDtTypeElem;
	inline static const char* typeName() { return "GsDateVector"; }
	GsString toString() const;
	int compare( const GsDateVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSDATEVECTOR_H) */


