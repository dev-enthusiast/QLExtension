/****************************************************************
**
**	GSOBSERVER.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsObserver.h,v 1.3 2001/11/27 22:41:07 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSOBSERVER_H )
#define IN_GSBASE_GSOBSERVER_H

#include <gscore/base.h>
#include <ccstl.h>
#include <list>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsObserver 
** Description : 
****************************************************************/

class GsSubject;
class EXPORT_CLASS_GSBASE GsObserver
{

protected:

	GsSubject*
			m_Subject;

	GsObserver( GsSubject* Subject = NULL );
	GsObserver( const GsObserver &Other );
	GsObserver &operator=( const GsObserver &Rhs );

	void replaceSubject( GsSubject* Subject );

public:

	virtual ~GsObserver();

	virtual void subjectChanged( GsSubject* Subject, void* UserData );
	virtual void subjectGone( GsSubject* Subject );
	
};



CC_END_NAMESPACE

#endif 
