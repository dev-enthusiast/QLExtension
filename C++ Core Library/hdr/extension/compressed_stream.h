/****************************************************************
**
**	COMPRESSED_STREAM.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/extension/src/extension/compressed_stream.h,v 1.7 2001/07/18 18:54:24 simpsk Exp $
**
****************************************************************/

#ifndef IN_EXTENSION_COMPRESSED_STREAM_H
#define IN_EXTENSION_COMPRESSED_STREAM_H

#include <extension/base.h>
#include <sstream>

// FIX FIX this relies on internal zlib impl
// to forward declare. (we don't want to include zlib.h in here)
struct z_stream_s;

CC_BEGIN_NAMESPACE( extension )

class EXPORT_CLASS_EXTENSION compressed_streambuf : public CC_NS(std,stringbuf)
{
public:
	enum								// FIX these are hardcoded from zlib.h in order to prevent dependency upon including it
	{
		NO_COMPRESSION = 0,
		BEST_SPEED = 1,
		BEST_COMPRESSION = 9,
		DEFAULT_COMPRESSION = -1,
		BUF_SIZE = 16384
	};

	enum
	{
		NONE,
		INFLATE,
		DEFLATE
	};

	explicit compressed_streambuf( CC_NS(std,streambuf)& _strbuf, int level = DEFAULT_COMPRESSION );
	~compressed_streambuf();

	int sync();							// override std::streambuf::sync()
	int underflow();

private:
	unsigned char*				 m_buf;
	unsigned char*				 m_buf1;
	CC_NS(std,streambuf)&	     m_strbuf;
	z_stream_s*				     m_zstream;
	int							 m_state;
	int                          m_level;

	bool flush_out();					// flush output
	void init_state( int state );
	void end_state();
};

class EXPORT_CLASS_EXTENSION compressed_stream
{
public:
	explicit compressed_stream( CC_NS(std,ios)& _ios, int level = compressed_streambuf::DEFAULT_COMPRESSION )
	: m_ios( _ios ),
	  m_strbuf( *m_ios.rdbuf() ),
	  m_compressed_strbuf( m_strbuf, level )
	{
		m_strbuf.pubsync();				// flush any pending stuff
		m_ios.rdbuf( &m_compressed_strbuf );
	}
	~compressed_stream()
	{
		m_compressed_strbuf.pubsync();		// flush pending stuff
		m_ios.rdbuf( &m_strbuf );
	}

private:
	CC_NS(std,ios)& m_ios;
	CC_NS(std,streambuf)& m_strbuf;
	compressed_streambuf m_compressed_strbuf;
};

CC_END_NAMESPACE

#endif 
