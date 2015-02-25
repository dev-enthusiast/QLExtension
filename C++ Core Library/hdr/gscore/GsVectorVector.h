/****************************************************************
**
**	gscore/GsVectorVector.h	- Header for GsVectorVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSCORE_GSVECTORVECTOR_H)
#define IN_GSCORE_GSVECTORVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gscore/GsVector.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsVectorVector;
class GsVectorVectorRep;
class GsVectorVecRevIter;
class GsVectorVecConstRevIter;

/*
**  class GsVectorVecRevIter
*/

class EXPORT_CLASS_GSBASE GsVectorVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsVector value_type;
	typedef ptrdiff_t difference_type;
	typedef GsVector *pointer;
	typedef GsVector &reference;
	typedef GsVector *iterator_type;

	GsVectorVecRevIter() 
		: m_current() {}


	explicit GsVectorVecRevIter( iterator_type x ) 
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

	GsVectorVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsVectorVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsVectorVecRevIter operator++(int) 
	{
		GsVectorVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsVectorVecRevIter operator--(int) 
	{
		GsVectorVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsVectorVecRevIter operator+(
	const GsVectorVecRevIter &iter,
	GsVectorVecRevIter::difference_type n
) 
{
	return GsVectorVecRevIter(iter.Class()-n);
}

inline GsVectorVecRevIter &operator+=(
	GsVectorVecRevIter &iter,
	GsVectorVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsVectorVecRevIter operator-(
	const GsVectorVecRevIter &iter,
	GsVectorVecRevIter::difference_type n
) 
{
	return GsVectorVecRevIter( iter.Class() + n );
}

inline GsVectorVecRevIter &operator-=(
	GsVectorVecRevIter &iter,
	GsVectorVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsVectorVecRevIter &lop,
	const GsVectorVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsVectorVecConstRevIter
*/

class EXPORT_CLASS_GSBASE GsVectorVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsVector value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsVector *pointer;
	typedef const GsVector &reference;
	typedef const GsVector *iterator_type;

	GsVectorVecConstRevIter() 
		: m_current() {}

	explicit GsVectorVecConstRevIter(iterator_type x) 
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

	GsVectorVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsVectorVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsVectorVecConstRevIter operator++(int) 
		{ GsVectorVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsVectorVecConstRevIter operator--(int) 
		{ GsVectorVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsVectorVecConstRevIter  operator+(
	const GsVectorVecConstRevIter &iter,
	GsVectorVecConstRevIter::difference_type n
) 
{
	return GsVectorVecConstRevIter(iter.Class()-n);
}

inline GsVectorVecConstRevIter &operator+=(
	GsVectorVecConstRevIter &iter,
	GsVectorVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsVectorVecConstRevIter operator-(
	const GsVectorVecConstRevIter &iter,
	GsVectorVecConstRevIter::difference_type n
) 
{
	return GsVectorVecConstRevIter( iter.Class() + n );
}

inline GsVectorVecConstRevIter &operator-=(
	GsVectorVecConstRevIter &iter,
	GsVectorVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsVectorVecConstRevIter &lop,
	const GsVectorVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsVectorVectorRep
*/

class EXPORT_CLASS_GSBASE GsVectorVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsVector value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsVector* iterator;
	typedef const GsVector* const_iterator;
	typedef GsVectorVecRevIter reverse_iterator;
	typedef GsVectorVecConstRevIter const_reverse_iterator;

	typedef GsVector &reference;
	typedef const GsVector &const_reference;

	// Constructors
	GsVectorVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsVectorVectorRep(size_t);
	GsVectorVectorRep(size_t,const value_type &x);
	GsVectorVectorRep(const_iterator begin, const_iterator end);
	GsVectorVectorRep(const GsVectorVectorRep &other);

	// Destructor
	~GsVectorVectorRep();

	// Assignment
	GsVectorVectorRep &operator = (const GsVectorVectorRep&);

	// Copy
	GsVectorVectorRep *copy() { return new GsVectorVectorRep( *this ); }

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

	bool operator==(const GsVectorVectorRep &x) const;
	bool operator<(const GsVectorVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsVectorVector
*/

class EXPORT_CLASS_GSBASE GsVectorVector : public GsHandleCopyOnWrite<GsVectorVectorRep>
{
public:
	typedef GsVectorVectorRep::value_type value_type;
	typedef GsVectorVectorRep::size_type size_type;
	typedef GsVectorVectorRep::difference_type difference_type;

	typedef GsVectorVectorRep::iterator iterator;
	typedef GsVectorVectorRep::const_iterator const_iterator;
	typedef GsVectorVectorRep::reverse_iterator reverse_iterator;
	typedef GsVectorVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsVectorVectorRep::reference reference;
	typedef GsVectorVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsVectorVectorRep* const_rep;
public:

	// Constructors
	GsVectorVector();
	GsVectorVector(size_t);
	GsVectorVector(size_t,const value_type &x);
	GsVectorVector(const_iterator begin, const_iterator end);
	GsVectorVector(const GsVectorVector &other);

	// Destructor
	virtual ~GsVectorVector() {}

	// Assignment
	GsVectorVector &operator = (const GsVectorVector&);

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

	bool operator==(const GsVectorVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsVectorVector &x) const { return (!(*this == x)); }
	bool operator<(const GsVectorVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsVectorVector &x) const { return (x < *this); }
	bool operator<=(const GsVectorVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsVectorVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsVectorVector> GsDtType;
	typedef GsDtVector GsDtTypeElem;
	inline static const char* typeName() { return "GsVectorVector"; }
	GsString toString() const;
	int compare( const GsVectorVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSCORE_GSVECTORVECTOR_H) */


