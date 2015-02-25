/****************************************************************
**
**	gscore/GsException.h
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsException.h,v 1.41 2011/05/16 20:06:08 khodod Exp $
**
****************************************************************/

#ifndef IN_GSCORE_GSEXCEPTION_H
#define IN_GSCORE_GSEXCEPTION_H

#include <exception>
#include <gscore/base.h>
#include <gscore/GsString.h>

#include <gsbase_exception.h> //in kool_ade

CC_BEGIN_NAMESPACE( Gs )


/*
**	GSX_THROW macro
**
**	This macros constructs and throws an exception with a constructor
**	prototype matching Ctor( const string& Text, const char* File, int Line ).
**
**	It must be a macro to provide proper behaviour of __FILE__ & __LINE__
*/

#define GSX_THROW( XClass_, XText_ ) throw XClass_( XText_ , __FILE__, __LINE__ )


/*
**	Gs standard exception class
**
**	Catchers of GsException or derived classes will mainly be interested in
**		char const* getFile();
**		int			getLine();
**	Rethrowers may want to call
**		void		appendText( string const& MoreText );
**	Generators of an exception will generally this:
**		GSX_THROW( GsXRangeError, "Blah" );
**	Implementers of derived exceptions must implement
**		char const* getName();
*/

class EXPORT_CLASS_GSBASE GsException : public GsBaseException 
{
public:
	
	GsException( CC_NS(std,string) const& Text, char const* File, int Line );
	// default copy constructor
	virtual ~GsException() throw() {}
	// default assignment operator

	char const* what() const throw(); // from std::exception
	char const* brief() const;

	// Name of the exception (Domain Error, Range Error, ...)
	virtual char const* getName() const = 0;

	// Prepend additional error information to m_text ala ErrMore().
	// m_text becomes moreText + "\n" + m_text
	void prependText( CC_NS(std,string) const& moreText );

	// In general, what() should be used to format the error message
	CC_NS(std,string) const&	getText() const	{ return m_text; }
	char const*		getFile() const	{ return m_file; }
	int				getLine() const	{ return m_line; }

protected:
	CC_NS(std,string)    m_text;		// Arbitrary text
	const char*	m_file;		// Intended to be set to __FILE__ (not owned by this object)
	int			m_line;		// Intended to be set to __LINE__

	mutable
	CC_NS(std,string)    m_lastWhat;	// Last formatted what()/brief() call
};

/*
**	Usage: Inside class body
**	Example:
**		class EXPORT_CLASS_GSBASE GsXEval : public GsException
**		{
**			GSX_DECLARE_EXCEPTION(GsXEval,GsException);
**		};
**
*/

#define GSX_DECLARE_EXCEPTION(T,P)										\
public:																	\
	T( CC_NS(std, string) const& Text, char const* file, int line );	\
    ~T() throw();                                                       \
	char const* getName() const

/*
**	Usage: Declares entire class
**	Example:
**		GSX_DECLARE_EXCEPTION(EXPORT_CLASS_GSBASE,GsXEval,GsException);
*/

#define GSX_DECLARE_EXCEPTION_ALL(X,T,P)		\
class X T : public P							\
{												\
	GSX_DECLARE_EXCEPTION( T, P );				\
}


/*
**	Some common exceptions
*/

GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXInvalidArgument,	  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXInvalidOperation,  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXBadCast,			  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXBadDll,			  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXBadIndex,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXBadSize,			  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXDomainError,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXLengthError,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXRangeError,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXStreamRead,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXStreamWrite,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXUnsupported,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXNotImplemented,	  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXNotFound,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXSecDbError,		  GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXAllocationError,   GsException );
GSX_DECLARE_EXCEPTION_ALL( EXPORT_CLASS_GSBASE, GsXRelationalDbError, GsException );

CC_END_NAMESPACE

#endif /* IN_GSCORE_GSEXCEPTION_H */

