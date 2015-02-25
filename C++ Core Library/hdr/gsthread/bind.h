/**************************************************************** -*-c++-*-
**
**	BIND.H	- helper function, not thread related.  Should move to the extension project.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/bind.h,v 1.7 2001/07/09 21:30:58 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_BIND_H
#define IN_GSTHREAD_BIND_H

/*
 *  class Bind0<typename Arg_, typename Ret_>
 *
 *    The Standard has very nice adaptors called bind1st and bind2nd, but unfortunately, no bind0th.
 *
 *    Bind0 is an Adaptable Generator. (ie. has a typedef result_type and has an operator(). see [AUSTERN] p113)
 *    It is a function object adaptor that allows a function pointer (and optional argument) to be treated
 *    as an Adaptable Generator. (ie. you can use it as an argument to generate_n and any other functions that take
 *    an Adaptable Generator)
 *
 *    Bind0, like std::binder1st and other adaptors, is rarely used directly.  Instead, there is a convenient
 *    helper function provided.  For Bind0, this helper is bind_func().
 *
 *  template <typename Arg_, typename Ret_>
 *  Bind0<Arg_, Ret_> bind_func(Ret_ (*func)(Arg_), Arg_ arg);
 *
 *  template <typename Ret_>
 *  Bind0<void, Ret_> bind_func(Ret_ (*func)(void));
 *
 *    bind_func takes a pointer to a function and optionally an argument, and returns a Bind0 that will invoke
 *    that function with the argument whenever the operator() method is called.
 *    By using type deduction, bind_func is much more convenient then Bind0.
 *
 *  example:
 *
 *    int func(int* counter) { return ++*counter;} // increment a counter and return the new value
 *
 *    int counter = 0;
 *    std::vector<size_t> my_vec;
 *    std::generate_n( std::back_inserter(my_vec), 100, bind_func( func, &counter ) );
 *
 *  This will create a vector of 100 elements with ascending values from 1 to 100.
 *  bind_func( func, &counter) will deduce its arguments as Arg_ = int*, Ret_ = int, and
 *  return a Bind0<int*, int>.
 *
 */

CC_BEGIN_NAMESPACE( Gs )

// ------------------------------------------------------------------------ bind

template <class Arg_, class Ret_>
		class Bind0Func
		{
		public:
			typedef Ret_ result_type;

			Bind0Func( Ret_ (*func_)(Arg_), Arg_ arg_ )
			: m_func( func_ ),
			  m_arg( arg_ ) // this could throw - we can't know
			{}

			Ret_ operator()() const { return m_func( m_arg );} // we can't know what this could throw

		private:
			Ret_ (*m_func)(Arg_);
			Arg_ m_arg;
		};

template <class Func_>
		class Bind0Gen
		{
		public:
			typedef typename Func_::result_type result_type;

			Bind0Gen( Func_ const& f, typename Func_::argument_type const& arg )
			: m_func( f ),
			  m_arg( arg )
			{}

			result_type operator()() const { return m_func( m_arg );}
			result_type operator()()       { return m_func( m_arg );}

		private:
			Func_ m_func;
			typename Func_::argument_type m_arg;
		};

// partial specialization - workaround
template <class Ret_>
		class Bind0VoidFunc
		{
		public:
			typedef Ret_ result_type;

			Bind0VoidFunc( Ret_ (*func_)(void) ) throw()
			: m_func( func_ )
			{}

			Ret_ operator()() const { return m_func();} // we can't know what this could throw

		private:
			Ret_ (*m_func)(void);
		};

template <class Arg_, class Ret_>
		Bind0Func<Arg_, Ret_> bind_func(Ret_ (*func)(Arg_), Arg_ const& val) // can throw if Arg_::Arg_() throws
{
	return Bind0Func<Arg_, Ret_>( func, val );
}

template <class Func_, class Arg_>
		Bind0Gen<Func_> bind_functor( Func_ const& func, Arg_ const& arg )
{
	return Bind0Gen<Func_>( func, arg );
}

template <class Ret_>
Bind0VoidFunc<Ret_> bind_func(Ret_ (*func)(void)) throw() // nothrow
{
	return Bind0VoidFunc<Ret_>( func );
}

CC_END_NAMESPACE

#endif
