/**************************************************************** -*-c++-*-
**
**	MAKER.H	- simple utility function/class.  should move to the extension project.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/maker.h,v 1.7 2004/11/10 23:09:06 khodod Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_MAKER_H
#define IN_GSTHREAD_MAKER_H

/*
 *  class Maker<typename T_, typename Arg1_, typename Arg2_>
 *
 *    Maker is an Adaptable Unary Function. (see [AUSTERN] p114-115)
 *    It is a function object that creates (or Makes) new objects of type T_. (see Factory)
 *
 *    The T_ object is allocated from the free store (operator new), and is constructed
 *    with two arguments.  The first argument is of type Arg1_, and is supplied on each
 *    call of operator() by the caller.  The second argument is of type Arg2_, and is
 *    copied from a variable stored in the Maker object.
 *
 *    Maker, like many other function object classes, is rarelt used directly.  Instead,
 *    there is a convenient helper function provided.  For Maker, this helper is maker.
 *
 *  template <typename T_, typename Arg1_, typename Arg2_>
 *  Maker<T_, Arg1_, Arg2_> maker( Arg2_ val );
 *
 *    maker takes an argument of type Arg2_ that is to be passed as the second arguemnt to
 *    each new T_ that the Maker object will create.  Unfortunately, this is the only arguement
 *    deduction performed, and the remaining two types must be explicitly declared.
 *
 *  example:
 *
 *    struct Foo
 *    {
 *      Foo( int num, std::ostream* out )
 *      {
 *        *out << num << std::endl; // print out the number for this object
 *      }
 *      ...
 *    };
 *
 *    // create a vector of 100 random integers.
 *    std::vector<int> rand_vec;
 *    std::generate_n( std::back_inserter(my_vec), 100, rand );
 *
 *    // create 100 Foo objects for each number
 *    std::vector<Foo*> foo_ptrs( 100, 0 );
 *    std::transform( rand_vec.begin(), rand_vec.end(),
 *                    foo_ptrs.begin(),
 *                    maker<Foo*, int>( &std::cout ) );
 *
 *  This will create 100 Foo objects as Foo( rand(), &std::cout ).
 *  maker<Foo*, int>( &std::cout ) will create a Maker<Foo*, int, std::ostream*> object.
 *  (or, if you want to be picky, a Maker<Foo*, int, std::basic_ostream<char, std::char_traits<char> > >)
 *
 *  Note:
 *    Be careful about exception safety.  Returning pointers to newly created objects is rarely
 *    a good idea as if an exception is thrown during the evaluation of the expression, this object
 *    will not be destroyed.  In this example, we are safe only because we create the vector first, then
 *    assign elements.
 *    Be careful to avoid a construct like:
 *    std::transform( rand_vec.begin(), rand_vec.end(),
 *                    std::back_inserter( foo_ptrs ),
 *                    maker<Foo*, int>( &std::cout ) );
 *    As std::vector::push_back could fail, and the Foo object would be leaked.
 *    (Perhapse a Safe_Maker should be written that returns an std::auto_ptr<T_> or some other
 *     guardian type instead)
 *    
 */

#include <functional> // for std::unary_function

CC_BEGIN_NAMESPACE( Gs )

// -------------------------------------------------------------- maker

template <class T_, class Arg1_, class Arg2_>
		class Maker
			: public CC_NS(std,unary_function)<T_*, Arg1_>
{
private:
    typedef CC_NS(std,unary_function)<T_*, Arg1_> Super;

public:
	typedef typename Super::argument_type argument_type;
	typedef typename Super::result_type   result_type;

	Maker( Arg2_ arg ) : m_arg2( arg ) {} // can throw if Arg2_ copy constructor throws - converting constructor
	T_* operator()( Arg1_ arg1 ) const { return new T_( arg1, m_arg2 ); } // can throw if new fails or if T_ constructor throws

	// use default copy constructor - throws if Arg2_'s copy constructor throws
	// use default destructor - ~Arg2_() shouldn't throw
	// use default assignment operator

  private:
	Arg2_ const m_arg2;
};

#if 0
// if we ever get rid of SW4.2, we can have this back
template <class T_, class Arg1_, class Arg2_>
Maker<T_, Arg1_, Arg2_> maker( Arg2_ val ) // can throw if val's copy constructor throws
{
	return Maker<T_, Arg1_, Arg2_>(val);
}
#endif

CC_END_NAMESPACE

#endif
