/****************************************************************
**
**	gscore/GsNoCopy - prevent copying
**  GsNoCopy - the template - privately inherit from this, templated 
**  on yourself to prevent your class from being copied
**  ie. class Foo : private GsNoCopy<Foo>
**
**  This has an advantage over the non-templated version: it can be 
**  inherited mutliple times.
**  ie. class Base : private GsNoCopy<Base>
**      class D1 : protected Base
**      class D2 : private D1, private GsNoCopy<D2> 
**       n.b. GsNoCopy<D2> is not GsNoCopy<Base>, so no ambiguous base class problems
**
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/extension/src/extension/GsNoCopy.h,v 1.3 2001/11/27 21:41:20 procmon Exp $
**
****************************************************************/


// -*-c++-*-
#ifndef IN_EXTENSION_GSNOCOPY_H
#define IN_EXTENSION_GSNOCOPY_H

// Written by KEN, moved to gscore by PW, moved to extension by KEN

/*
 *
 * class GsNoCopy1;
 *
 *   Privately inherit from GsNoCopy1 to prevent your class from being copied.
 *   This class has no data members or code.  Using inheritance allows the empty base
 *   optimization (the resulting object is not larger).
 *   It does not have a virtual destructor, nor should it.
 *
 *   Example:
 *
 *     struct Foo : private GsNoCopy1 { Foo(int) {} };
 *
 *     Foo f1(1);   // constructs a Foo
 *     Foo f2 = f1; // doesn't compile - GsNoCopy1::GsNoCopy1(GsNoCopy1 const&) is inaccessible.
 *     Foo f3(f2);  // doesn't compile - same reason.
 *     Foo f4(2);
 *     f1 = f4;     // doesn't compile - GsNoCopy1& GsNoCopy1::operator=(GsNoCopy1 const&) is inaccessible.
 *     Foo func(int i) { return Foo(i);} // doesn't compile - a return value has copy construction semantics.
 *                                       //                   (even though there may be elision)
 *     void func2(Foo arg); // doesn't compile - parameters have copy construction semantics.
 *
 *   Shortcomings:
 *     Only 1 of these may exist in a class hierarchy.
 *     To be picky: there may be more than one, but you must be carefule that thay remain
 *     unambiguous, and this usually means everyone must use virtual inheritance.  Even
 *     using virtual inheritance is complicated because then all virtually inherited GsNoCopy1's
 *     must have the same access level (ie all public, all protected, or all private).  This
 *     gets even stickier in all but the most simple examples.
 *
 *   Misc:
 *     It is possible to use a GsNoCopy1-derived member or base class and still provide copy
 *     construction and assignment operations.  You just have to be explicit.
 *
 *     struct Foo : private GsNoCopy1 { Foo(int) {} };
 *
 *     class Bar
 *     {
 *     public:
 *       Bar() : f1(0) {} // default constructor
 *       Bar(Bar const& rhs) : f1(0) {} // copy constructor - note that we can't do "f1(rhs.f1)"
 *       Bar& operator=(Bar const& rhs) { return *this;} // assignment operator - note that we can't do "f1 = rhs.f1"
 *     private:
 *       Foo f1;
 *     };
 *
 *     Do the same for base classes, except change the comments to:
 *       // copy constructor - note that we can't do "b1(rhs)"
 *       // assignment operator - note that we can't do "b1::operator=(rhs)"
 *
 *
 *  class GsNoCopy <typename T_>
 *
 *    Privately inherit from GsNoCopy1<T> to prevent instances of class T from being copied.
 *    This does all the glorious things that GsNoCopy1 will do, but does not suffer from the shortcomings.
 *
 *    Example:
 *      (all the examples for GsNoCopy1 apply)
 *
 *      struct Foo : private GsNoCopy<Foo> { Foo(int) {} };
 *      struct Bar : public Foo, private GsNoCopy<Bar> {}; // even though we are no copyable due to Foo, we can be verbose.
 *
 *      struct Baz : public Other, private GsNoCopy<Baz> {}; // if we don't know if Other is GsNoCopy, this is always safe
 *
 * $Header: /home/cvs/src/extension/src/extension/GsNoCopy.h,v 1.3 2001/11/27 21:41:20 procmon Exp $
 */

CC_BEGIN_NAMESPACE( extension )

// ------------------------------------------- GsNoCopy

// GsNoCopy1 - privately inherit from this to prevent your class from being copied
// Be careful - only 1 of these can exist in a class hierarchy.
//   (well, almost.  There may be more than one, but you must be careful that they remain unambiguous.)
class GsNoCopy1
{
public:
  // use default destructor - should be "inline ~GsNoCopy1() throw() {}"

protected:
	GsNoCopy1() throw() {} // protected so that this class can only be used by inheritance

private:
	GsNoCopy1( GsNoCopy1 const& );            // unimplemented - if anyone tries to copy this, they should get a compile/link error
	GsNoCopy1& operator=( GsNoCopy1 const& ); // unimplemented - if anyone tries to assign this, they should get a compile/link error
};

// GsNoCopy - the template - privately inherit from this, templated on yourself to prevent your class from being copied
// ie. class Foo : private GsNoCopy<Foo>
//
// This has 1 advantage over the non-templated version: it can be inherited mutliple times.
// ie. class Base : private GsNoCopy<Base>
//     class D1 : protected Base
//     class D2 : private D1, private GsNoCopy<D2> // GsNoCopy<D2> is not GsNoCopy<Base>, so no ambiguous base class problems
template <class T_>
class GsNoCopy
{
public:
	// use default destructor - should be "inline ~GsNoCopy() throw() {}"

protected:
	GsNoCopy() throw() {} // protected so that this class can only be used by inheritance

private:
	GsNoCopy( GsNoCopy<T_> const& ); // unimplemented
	// - if anyone tries to copy this, they should get a compile/link error

	GsNoCopy<T_>& operator=( GsNoCopy<T_> const& ); // unimplemented
	// - if anyone tries to assign this, they should get a compile/link error
};

CC_END_NAMESPACE

#endif
