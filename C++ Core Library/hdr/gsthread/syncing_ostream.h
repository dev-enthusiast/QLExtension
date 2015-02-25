/**************************************************************** -*-c++-*-
**
**	SYNCING_OSTREAM.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsthread/src/gsthread/syncing_ostream.h,v 1.5 2001/06/20 21:56:29 simpsk Exp $
**
****************************************************************/

#ifndef IN_GSTHREAD_SYNCING_OSTREAM_H
#define IN_GSTHREAD_SYNCING_OSTREAM_H

#include <gsthread/gsthread_base.h>

#include <iosfwd>  // for std::basic_ios
#include <ostream> // for std::ostream
#include <sstream> // for std::stringbuf

CC_BEGIN_NAMESPACE( Gs )

class GsMutex; // from <gsthread/thread_prim.h>

GsMutex& get_stream_lock( CC_NS(std,ios)& str ); // this /can/ throw, but not likely

class EXPORT_CLASS_GSTHREAD Synced_Streambuf
	: public CC_NS(std,stringbuf)
{
public:
    explicit Synced_Streambuf( CC_NS(std,ostream)& dest ) : m_dest(dest) {}

	// use default copy constructor - not sure this makes too much sense
	// no assignment operator due to reference member
	// use default destructor

protected:
	int sync(); // overrides std::streambuf::sync()

private:
	CC_NS(std,ostream)& m_dest;
};

class EXPORT_CLASS_GSTHREAD Synced_Ostream
	: public CC_NS(std,ostream)
{
public:
    explicit Synced_Ostream( CC_NS(std,ostream)& dest );

private:
    Synced_Streambuf m_buf;
};


CC_END_NAMESPACE

#endif // IN_GSTHREAD_SYNCING_OSTREAM_H
