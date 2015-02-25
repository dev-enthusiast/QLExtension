/****************************************************************
**
**	fcadlib/adexcelsup.h	- Random Excel support functions
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adexcelsup.h,v 1.5 2012/03/01 20:14:29 e19351 Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADEXCELSUP_H )
#define IN_FCADLIB_ADEXCELSUP_H

#include	<string>
#include	<windows.h>
#include	<fcadlib/base.h>
#include	<fcadlib/xlcall.h>

CC_BEGIN_NAMESPACE( Gs )


/*
**	XlStr - An XLOPER that is a string and cleans up when deleted or assigned
**	Not to be returned from a addin function
*/

class XlStr
{
public:
	XLOPER	x;

	void assign( const char *s )
	{
		int len = strlen( s );
		if( len > 255 )
			len = 255;
		x.val.str[0] = len;
		strncpy( x.val.str+1, s, len );
	}
	XlStr( const char *s = "" )
	{
		x.val.str = new char[ 256 ];
		x.xltype  = xltypeStr;
		assign( s );
	}
	~XlStr()
	{
		delete [] x.val.str;
	}
	void operator=( const char *s )
	{
		assign( s );
	}
	XLOPER *operator&()
	{
		return &x;
	}
};



/*
**	XlNum - An XLOPER that is a number and handles assignment
**	Not to be returned from a addin function
*/

class XlNum
{
public:
	XLOPER	x;

	XlNum( double d = 0 )
	{
		x.val.num = d;
		x.xltype  = xltypeNum;
	}
	void operator=( double d )
	{
		x.val.num = d;
	}
	XLOPER *operator&()
	{
		return &x;
	}
};


/*
**	XlNil - An XLOPER that is an xltypeNil
**	Not to be returned from a addin function
*/

class XlNil
{
public:
	XLOPER	x;

	XlNil()				{ x.xltype = xltypeNil; }
	XLOPER *operator&()	{ return &x; }
};


/*
**	XlMissing - An XLOPER that is an xltypeMissing
**	Not to be returned from a addin function
*/

class XlMissing
{
public:
	XLOPER	x;

	XlMissing()			{ x.xltype = xltypeMissing; }
	XLOPER *operator&()	{ return &x; }
};



EXPORT_CPP_FCADLIBX void
		AdXlCoerceFree( XLOPER	&Arg );

EXPORT_CPP_FCADLIBX GsStatus
		AdXlCoerce( XLOPER &Result, const XLOPER &Input, int OkTypes );

EXPORT_CPP_FCADLIBX char
		*AdXlCallingCell( char *Buffer, size_t BufSize, unsigned RowOffset=0, unsigned ColOffset=0 );

CC_END_NAMESPACE
#endif

