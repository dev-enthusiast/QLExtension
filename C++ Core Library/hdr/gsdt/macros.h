/* $Header: /home/cvs/src/gsdtlite/src/gsdt/macros.h,v 1.3 2001/11/27 22:45:04 procmon Exp $ */
/****************************************************************
**
**	gsdt/macros.h	- Macros for GSDT classes
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdtlite/src/gsdt/macros.h,v 1.3 2001/11/27 22:45:04 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSDT_MACROS_H )
#define IN_GSDT_MACROS_H

/*
**	Macros for declaring types
*/

#define GSDT_DECLARE_CASTS(T)							\
public:													\
	static const T* Cast( const GsDt* Obj )				\
	{													\
		if( Obj && Obj->isKindOf( T::staticType() ))	\
			return (const T*) Obj;						\
		CastErr( T::staticType(), Obj );				\
		return NULL;									\
	}													\
	static T* Cast( GsDt *Obj )							\
	{													\
		return (T*)Cast( (const GsDt*) Obj );			\
	}													\
	static const T& Cast( const GsDt& Obj )				\
	{													\
		const T* Ptr = T::Cast( &Obj );					\
		if( !Ptr )										\
			GSX_THROW( GsXBadCast, GsErrGetString() );	\
		return *Ptr;									\
	}													\
	static T& Cast( GsDt& Obj )							\
	{													\
		T* Ptr = T::Cast( &Obj );						\
		if( !Ptr )										\
			GSX_THROW( GsXBadCast, GsErrGetString() );	\
		return *Ptr;									\
	}


#define	GSDT_DECLARE_COMMON(T)							\
	GSDT_DECLARE_CASTS(T)								\
	friend GsType* initType##T();						\
public:													\
	static	GsType& staticType();						\
	virtual const GsType& type() const;					\
	virtual bool isKindOf( const GsType& Type ) const;

// Use this for abstract classes with pure virtual functions
#define	GSDT_DECLARE_ABSTRACT_TYPE( name )		\
protected:										\
	static void initType() {}					\
    GSDT_DECLARE_COMMON( name )

// Use this for concrete classes
#define	GSDT_DECLARE_TYPE( name )				\
private:										\
	static void initType();						\
    GSDT_DECLARE_COMMON( name )


/*
**	Macros for implementing types
*/

#define	GSDT_IMPLEMENT_FUNCS( T )					\
static GsType* staticType##T = 0;					\
inline GsType* initType##T()						\
{													\
	if( !staticType##T )							\
	{												\
		T::initType();								\
		staticType##T = GsType::LookupOrDie( #T );	\
	}												\
	return staticType##T;							\
}													\
GsType& T::staticType()								\
{													\
	return *initType##T();							\
}													\
const GsType& T::type() const						\
{													\
	return *initType##T();							\
}

// isKindOf for no parent class (only used by GsDt)
#define GSDT_IMPLEMENT_KIND0( T )					\
bool T::isKindOf(									\
	const GsType& Type								\
) const												\
{													\
	return *initType##T() == Type;					\
}

// isKindOf for one parent class
#define GSDT_IMPLEMENT_KIND1( T, Parent )			\
bool T::isKindOf(									\
	const GsType& Type								\
) const												\
{													\
	return 		*initType##T() == Type				\
			||	Parent::isKindOf( Type );			\
}


// Everything relying only on the the base class
#define	GSDT_IMPLEMENT_COMMON( Class )	\
    GSDT_IMPLEMENT_FUNCS( Class )

#define	GSDT_IMPLEMENT_ABSTRACT_TYPE0( Class )			\
    GSDT_IMPLEMENT_COMMON( Class )						\
    GSDT_IMPLEMENT_KIND0( Class )						\

#define	GSDT_IMPLEMENT_ABSTRACT_TYPE1( Class, Parent )	\
    GSDT_IMPLEMENT_COMMON( Class )						\
    GSDT_IMPLEMENT_KIND1( Class, Parent )				\

#define	GSDT_IMPLEMENT_TYPE1( Class, Parent )			\
    GSDT_IMPLEMENT_COMMON( Class )						\
    GSDT_IMPLEMENT_KIND1( Class, Parent )

#endif /* IN_GSDT_MACROS_H */



