/****************************************************************
**
**	gssymexpr/GsSymbolicVariableNameAndArgsVector.h	- Header for GsSymbolicVariableNameAndArgsVector
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
****************************************************************/


#if ! defined(IN_GSSYMEXPR_GSSYMBOLICVARIABLENAMEANDARGSVECTOR_H)
#define IN_GSSYMEXPR_GSSYMBOLICVARIABLENAMEANDARGSVECTOR_H

#include <iterator>
#include <stddef.h>
#include <gssymexpr/GsSymbolicVariableNameAndArgs.h>
#include <gscore/GsString.h>
#include <gscore/gsdt_fwd.h>
#include <gscore/GsRefCount.h>
#include <gscore/GsHandle.h>

CC_BEGIN_NAMESPACE( Gs )

/*
**	Classes declared in this header
*/

class GsSymbolicVariableNameAndArgsVector;
class GsSymbolicVariableNameAndArgsVectorRep;
class GsSymbolicVariableNameAndArgsVecRevIter;
class GsSymbolicVariableNameAndArgsVecConstRevIter;

/*
**  class GsSymbolicVariableNameAndArgsVecRevIter
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableNameAndArgsVecRevIter
{
public:
	// FIX- The compiler doesn't like this for some reason
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbolicVariableNameAndArgs value_type;
	typedef ptrdiff_t difference_type;
	typedef GsSymbolicVariableNameAndArgs *pointer;
	typedef GsSymbolicVariableNameAndArgs &reference;
	typedef GsSymbolicVariableNameAndArgs *iterator_type;

	GsSymbolicVariableNameAndArgsVecRevIter() 
		: m_current() {}


	explicit GsSymbolicVariableNameAndArgsVecRevIter( iterator_type x ) 
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

	GsSymbolicVariableNameAndArgsVecRevIter &operator++() 
	{
		--m_current;
		return *this;
	}

	GsSymbolicVariableNameAndArgsVecRevIter &operator--() 
	{
		++m_current;
		return *this;
	}

#if ! defined(CC_NO_POSTFIX)
	GsSymbolicVariableNameAndArgsVecRevIter operator++(int) 
	{
		GsSymbolicVariableNameAndArgsVecRevIter tmp(m_current);
		--m_current;
		return tmp;
	}

	GsSymbolicVariableNameAndArgsVecRevIter operator--(int) 
	{
		GsSymbolicVariableNameAndArgsVecRevIter tmp(m_current);
		++m_current;
		return tmp;
	}
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};

inline GsSymbolicVariableNameAndArgsVecRevIter operator+(
	const GsSymbolicVariableNameAndArgsVecRevIter &iter,
	GsSymbolicVariableNameAndArgsVecRevIter::difference_type n
) 
{
	return GsSymbolicVariableNameAndArgsVecRevIter(iter.Class()-n);
}

inline GsSymbolicVariableNameAndArgsVecRevIter &operator+=(
	GsSymbolicVariableNameAndArgsVecRevIter &iter,
	GsSymbolicVariableNameAndArgsVecRevIter::difference_type	n
) 
{
	iter.Class() -= n;
	return iter;
}

inline GsSymbolicVariableNameAndArgsVecRevIter operator-(
	const GsSymbolicVariableNameAndArgsVecRevIter &iter,
	GsSymbolicVariableNameAndArgsVecRevIter::difference_type n
) 
{
	return GsSymbolicVariableNameAndArgsVecRevIter( iter.Class() + n );
}

inline GsSymbolicVariableNameAndArgsVecRevIter &operator-=(
	GsSymbolicVariableNameAndArgsVecRevIter &iter,
	GsSymbolicVariableNameAndArgsVecRevIter::difference_type n
) 
{
	iter.Class()+=n; return iter;
}

inline bool operator == (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolicVariableNameAndArgsVecRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}


/*
**  class GsSymbolicVariableNameAndArgsVecConstRevIter
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableNameAndArgsVecConstRevIter
{
public:
	//typedef CC_NS(std,random_access_iterator_tag) iterator_category;
	typedef GsSymbolicVariableNameAndArgs value_type;
	typedef ptrdiff_t difference_type;
	typedef const GsSymbolicVariableNameAndArgs *pointer;
	typedef const GsSymbolicVariableNameAndArgs &reference;
	typedef const GsSymbolicVariableNameAndArgs *iterator_type;

	GsSymbolicVariableNameAndArgsVecConstRevIter() 
		: m_current() {}

	explicit GsSymbolicVariableNameAndArgsVecConstRevIter(iterator_type x) 
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

	GsSymbolicVariableNameAndArgsVecConstRevIter &operator++() 
		{ --m_current; return *this; }

	GsSymbolicVariableNameAndArgsVecConstRevIter &operator--() 
		{ ++m_current; return *this; }

#if ! defined(CC_NO_POSTFIX)
	GsSymbolicVariableNameAndArgsVecConstRevIter operator++(int) 
		{ GsSymbolicVariableNameAndArgsVecConstRevIter tmp(m_current); --m_current; return tmp; }

	GsSymbolicVariableNameAndArgsVecConstRevIter operator--(int) 
		{ GsSymbolicVariableNameAndArgsVecConstRevIter tmp(m_current); ++m_current; return tmp; }
#endif /* CC_NO_POSTFIX */

	iterator_type &Class()  { return m_current; }

private:
	iterator_type m_current;
};


inline GsSymbolicVariableNameAndArgsVecConstRevIter  operator+(
	const GsSymbolicVariableNameAndArgsVecConstRevIter &iter,
	GsSymbolicVariableNameAndArgsVecConstRevIter::difference_type n
) 
{
	return GsSymbolicVariableNameAndArgsVecConstRevIter(iter.Class()-n);
}

inline GsSymbolicVariableNameAndArgsVecConstRevIter &operator+=(
	GsSymbolicVariableNameAndArgsVecConstRevIter &iter,
	GsSymbolicVariableNameAndArgsVecConstRevIter::difference_type n
) 
{
	iter.Class()-=n;
	return iter;
}

inline GsSymbolicVariableNameAndArgsVecConstRevIter operator-(
	const GsSymbolicVariableNameAndArgsVecConstRevIter &iter,
	GsSymbolicVariableNameAndArgsVecConstRevIter::difference_type n
) 
{
	return GsSymbolicVariableNameAndArgsVecConstRevIter( iter.Class() + n );
}

inline GsSymbolicVariableNameAndArgsVecConstRevIter &operator-=(
	GsSymbolicVariableNameAndArgsVecConstRevIter &iter,
	GsSymbolicVariableNameAndArgsVecConstRevIter::difference_type n
) 
{
	iter.Class()+=n;
	return iter;
}

inline bool operator == (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() == rop.Class();
}

inline bool operator != (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() != rop.Class();
}

inline bool operator < (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() < rop.Class();
}

inline bool operator > (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() > rop.Class();
}

inline bool operator <= (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() <= rop.Class();
}

inline bool operator >= (
	const GsSymbolicVariableNameAndArgsVecConstRevIter &lop,
	const GsSymbolicVariableNameAndArgsVecConstRevIter &rop
) 
{
	return lop.Class() >= rop.Class();
}

/*
**  class GsSymbolicVariableNameAndArgsVectorRep
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableNameAndArgsVectorRep : public GsRefCountCopyOnWrite
{
public:
	typedef GsSymbolicVariableNameAndArgs value_type;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

	typedef GsSymbolicVariableNameAndArgs* iterator;
	typedef const GsSymbolicVariableNameAndArgs* const_iterator;
	typedef GsSymbolicVariableNameAndArgsVecRevIter reverse_iterator;
	typedef GsSymbolicVariableNameAndArgsVecConstRevIter const_reverse_iterator;

	typedef GsSymbolicVariableNameAndArgs &reference;
	typedef const GsSymbolicVariableNameAndArgs &const_reference;

	// Constructors
	GsSymbolicVariableNameAndArgsVectorRep() : m_begin(0), m_end(0), m_max(0) {}
	GsSymbolicVariableNameAndArgsVectorRep(size_t);
	GsSymbolicVariableNameAndArgsVectorRep(size_t,const value_type &x);
	GsSymbolicVariableNameAndArgsVectorRep(const_iterator begin, const_iterator end);
	GsSymbolicVariableNameAndArgsVectorRep(const GsSymbolicVariableNameAndArgsVectorRep &other);

	// Destructor
	~GsSymbolicVariableNameAndArgsVectorRep();

	// Assignment
	GsSymbolicVariableNameAndArgsVectorRep &operator = (const GsSymbolicVariableNameAndArgsVectorRep&);

	// Copy
	GsSymbolicVariableNameAndArgsVectorRep *copy() { return new GsSymbolicVariableNameAndArgsVectorRep( *this ); }

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

	bool operator==(const GsSymbolicVariableNameAndArgsVectorRep &x) const;
	bool operator<(const GsSymbolicVariableNameAndArgsVectorRep &x) const;

private:
	iterator  m_begin;	// Pointer to the first address of
				// allocated memory
	iterator  m_end;	// Pointer to one past the last position
				// filled in the vector (length+1)
	iterator  m_max;	// Pointer to the last position of
				// allocated memory
};

/*
**  class GsSymbolicVariableNameAndArgsVector
*/

class EXPORT_CLASS_GSSYMEXPR GsSymbolicVariableNameAndArgsVector : public GsHandleCopyOnWrite<GsSymbolicVariableNameAndArgsVectorRep>
{
public:
	typedef GsSymbolicVariableNameAndArgsVectorRep::value_type value_type;
	typedef GsSymbolicVariableNameAndArgsVectorRep::size_type size_type;
	typedef GsSymbolicVariableNameAndArgsVectorRep::difference_type difference_type;

	typedef GsSymbolicVariableNameAndArgsVectorRep::iterator iterator;
	typedef GsSymbolicVariableNameAndArgsVectorRep::const_iterator const_iterator;
	typedef GsSymbolicVariableNameAndArgsVectorRep::reverse_iterator reverse_iterator;
	typedef GsSymbolicVariableNameAndArgsVectorRep::const_reverse_iterator const_reverse_iterator;

	typedef GsSymbolicVariableNameAndArgsVectorRep::reference reference;
	typedef GsSymbolicVariableNameAndArgsVectorRep::const_reference const_reference;

private: // FIX: used in rend() rbegin() workaround only
	typedef const GsSymbolicVariableNameAndArgsVectorRep* const_rep;
public:

	// Constructors
	GsSymbolicVariableNameAndArgsVector();
	GsSymbolicVariableNameAndArgsVector(size_t);
	GsSymbolicVariableNameAndArgsVector(size_t,const value_type &x);
	GsSymbolicVariableNameAndArgsVector(const_iterator begin, const_iterator end);
	GsSymbolicVariableNameAndArgsVector(const GsSymbolicVariableNameAndArgsVector &other);

	// Destructor
	virtual ~GsSymbolicVariableNameAndArgsVector() {}

	// Assignment
	GsSymbolicVariableNameAndArgsVector &operator = (const GsSymbolicVariableNameAndArgsVector&);

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

	bool operator==(const GsSymbolicVariableNameAndArgsVector &x) const 
	{ return *( this->m_rep ) == *( x.m_rep ); }
	bool operator!=(const GsSymbolicVariableNameAndArgsVector &x) const { return (!(*this == x)); }
	bool operator<(const GsSymbolicVariableNameAndArgsVector &x) const
	{ return *( this->m_rep ) < *( x.m_rep ); }

	bool operator>(const GsSymbolicVariableNameAndArgsVector &x) const { return (x < *this); }
	bool operator<=(const GsSymbolicVariableNameAndArgsVector &x) const { return (!(x < *this)); }
	bool operator>=(const GsSymbolicVariableNameAndArgsVector &x) const { return (!(*this < x)); }

	// Things to support GsDtGenVector
	typedef GsDtGenVector<GsSymbolicVariableNameAndArgsVector> GsDtType;
	typedef GsDtSymbolicVariableNameAndArgs GsDtTypeElem;
	inline static const char* typeName() { return "GsSymbolicVariableNameAndArgsVector"; }
	GsString toString() const;
	int compare( const GsSymbolicVariableNameAndArgsVector& Other ) const; 
	bool isValid() const { return true; } 
};

CC_END_NAMESPACE

#endif /* defined(IN_GSSYMEXPR_GSSYMBOLICVARIABLENAMEANDARGSVECTOR_H) */


