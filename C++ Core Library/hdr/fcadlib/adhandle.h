/****************************************************************
**
**	fcadlib/adhandle.h	- Adlib handle functions and macros
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/adhandle.h,v 1.8 2001/03/13 13:03:12 goodfj Exp $
**
****************************************************************/


#if !defined( IN_FCADLIB_ADHANDLE_H )
#define IN_FCADLIB_ADHANDLE_H


#include <fcadlib/base.h>
#include <vector>
#include <hash.h>
#include <gscore/GsString.h>
#include <gsdt/GsType.h>
#include <gsdt/GsDt.h>


CC_BEGIN_NAMESPACE( Gs )


EXPORT_CPP_FCADLIB GsDt* AdGetArgByHandle(
	const char*		Name,
	const char*		xStr,	// An Excel string: *xStr is length
	const GsType&	Type
);


/*
**	Object handles for use in spreadsheets
*/

class EXPORT_CLASS_FCADLIB ObjHandle
{
private:
	static HASH	*s_all;

	GsString		m_name;
	//char			*m_seqname;	// name with sequence counter
	GsDt			*m_obj;
	unsigned int	m_seq;		// sequence counter

	bool			m_wasAccessed;	// set to true if underlying object has been 'read'

	//static const unsigned int m_seqDigs;
	static const unsigned long m_maxSeq;

	void UpdateSeq();


public:
	ObjHandle( GsString HandleName );
	~ObjHandle();

	static bool validHandleName( const GsString &Name );
	// Overloaded validHandleName taking XLOPER has been removed,
	// as this is now a generic non-Excel file.

	static ObjHandle *create( const GsString &handleName);
		// handleName must be specified, otherwise the call will have no effect
		// rowoffset and coloffset no longer required.

	static ObjHandle *lookup( const GsString &Name );

	bool objWasAccessed( void ) { return m_wasAccessed; }
	void setAccessedFlag( bool flag = false ) { m_wasAccessed = flag; }

	static bool getAccessedList( GsStringVector& objList );
	static void setAccessedFlags( bool status = false );

	GsDt			*obj() { m_wasAccessed = true; return m_obj; }
	void			objSet( GsDt *obj ) { 
						delete m_obj; m_obj = obj; 
						UpdateSeq();
					}

	GsString		name();
};


CC_END_NAMESPACE


#endif
