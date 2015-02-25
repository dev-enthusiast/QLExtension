/* $Header: /home/cvs/src/gsdate/src/gscore/GsInvalidIncrementedDateConvention.h,v 1.7 2001/01/26 23:42:09 simpsk Exp $ */
/****************************************************************
**
**	GsInvalidIncrementedDateConvention.h	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.7 $
**
****************************************************************/

#if !defined( IN_GSINVALIDINCREMENTEDDATECONVENTION_H )
#define IN_GSINVALIDINCREMENTEDDATECONVENTION_H

#include	<gscore/base.h>
#include	<gscore/GsDate.h>
#include	<gscore/GsStream.h>
#include    <gscore/types.h> // for GsString
#include	<new>

//class std::bad_alloc;

CC_BEGIN_NAMESPACE( Gs )

class GsXInvalidArgument;

class EXPORT_CLASS_GSDATE GsInvalidIncrementedDateConvention
{
public:
	virtual ~GsInvalidIncrementedDateConvention() ;
	
	virtual const GsString& getName() const  = 0;
	virtual const GsString& getDescription() const  = 0;
	
	virtual GsInvalidIncrementedDateConvention* clone() const  = 0;
	
    virtual GsDate apply(int day, GsDate::MONTH month, int year) const 
		 = 0;

	// Streaming support.
	virtual void StreamStore( GsStreamOut &Stream ) const;
	virtual void StreamRead( GsStreamIn &Stream );
};

CC_END_NAMESPACE

#endif










