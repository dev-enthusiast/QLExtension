/****************************************************************
**
**	gscore/GsStringVector.h	- Header for GsStringVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSSTRINGVECTOR_H)
#define IN_GSCORE_GSSTRINGVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsString.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsStringVector;
class GsStringVectorRep;
class GsStringVecRevIter;
class GsStringVecConstRevIter;

/*
**  class GsStringVecRevIter
*/

class EXPORT_CLASS_GSBASE GsStringVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsString value_type;
	typedef ptrdiff_t difference_type;
	typedef GsString *pointer;
	typedef GsString &reference;
	typedef GsString *iterator_type;

	GsStringVecRevIter() 
		: m_current() {}


	explicit GsStringVecRevIter( iterator_type x ) 
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

	GsStringVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsStringVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsStringVecRevIter operator++(int) 
	{
		GsStringVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsStringVecRevIter operator--(int) 
	{
		GsStringVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsStringVecRevIter operator+(
	const GsStringVecRevIter &iter,
	GsStringVecRevIter::difference_type n
) 
{
	return GsStringVecRevIter(iter.Class()-n);
}

inline GsStringVecRevIter &operator+=(
	GsStringVecRevIter &iter,
	GsStringVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsStringVecRevIter operator-(
	const GsStringVecRevIter &iter,
	GsStringVecRevIter::difference_type n
) 
{
	return GsStringVecRevIter( iter.Class() + n );
}

inline GsStringVecRevIter &operator-=(
	GsStringVecRevIter &iter,
	GsStringVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsStringVecRevIter &lop,
	const GsStringVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsStringVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsStringVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsString value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsString *pointer;
	typedef const GsString &reference;
	typedef const GsString *iterator_type;

	GsStringVecConstRevIter() 
		: m_current() {}

	explicit GsStringVecConstRevIter(iterator_type x) 
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

	GsStringVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsStringVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsStringVecConstRevIter operator++(int) 
		{ GsStringVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsStringVecConstRevIter operator--(int) 
		{ GsStringVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsStringVecConstRevIter  operator+(
	const GsStringVecConstRevIter &iter,
	GsStringVecConstRevIter::difference_type n
) 
{
	return GsStringVecConstRevIter(iter.Class()-n);
}

inline GsStringVecConstRevIter &operator+=(
	GsStringVecConstRevIter &iter,
	GsStringVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsStringVecConstRevIter operator-(
	const GsStringVecConstRevIter &iter,
	GsStringVecConstRevIter::difference_type n
) 
{
	return GsStringVecConstRevIter( iter.Class() + n );
}

inline GsStringVecConstRevIter &operator-=(
	GsStringVecConstRevIter &iter,
	GsStringVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsStringVecConstRevIter &lop,
	const GsStringVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsStringVectorRep
*/

class EXPORT_CLASS_GSBASE GsStringVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsString value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsString* iterator;
	typedef const GsString* const_iterator;
	typedef GsStringVecRevIter reverse_iterator;
	typedef GsStringVecConstRevIter const_reverse_iterator;

	typedef GsString &reference;
	typedef const GsString &const_reference;

	// Constructors
	GsStringVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsStringVectorRep(size_t);
	GsStringVectorRep(size_t,const value_type &x);
	GsStringVectorRep(const_iterator begin, const_iterator end);
	GsStringVectorRep(const GsStringVectorRep &other);

	// Destructor
	~GsStringVectorRep();

	// Assignment
	GsStringVectorRep &operator = (const GsStringVectorRep&);

	// Copy
	GsStringVectorRep *copy() { return new GsStringVectorRep( *this ); }

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

	bool operator==(const GsStringVectorRep &x) const;
	bool operator<(const GsStringVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsStringVector
*/

class EXPORT_CLASS_GSBASE GsStringVector : public GsHandleCopyOnWrite<GsStringVectorRep>
{
public:
	typedef GsStringVectorRep::value_type value_type;
	typedef GsStringVectorRep::size_type size_type;
	typedef GsStringVectorRep::difference_type difference_type;

	typedef GsStringVectorRep::iterator iterator;
	typedef GsStringVectorRep::const_iterator const_iterator;
	typedef GsStringVectorRep::reverse_iterator reverse_iterator;
	typedef GsStringVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsStringVectorRep::reference reference;
	typedef GsStringVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsStringVectorRep* const_rep;
public:

	// Constructors
	GsStringVector();
	GsStringVector(size_t);
	GsStringVector(size_t,const value_type &x);
	GsStringVector(const_iterator begin, const_iterator end);
	GsStringVector(const GsStringVector &other);

	// Destructor
	virtual ~GsStringVector() {}

	// Assignment
	GsStringVector &operator = (const GsStringVector&);

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

	bool operator==(const GsStringVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsStringVector &x) const { return (!(*this == x)); }
	bool operator<(const GsStringVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsStringVector &x) const { return (x < *this); }
	bool operator<=(const GsStringVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsStringVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsStringVector> GsDtType;
	typedef GsDtString GsDtTypeElem;
	inline static const char* typeName() { return "GsStringVector"; }
	GsString toString() const;
	int compare( const GsStringVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSSTRINGVECTOR_H) */


