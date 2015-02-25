/****************************************************************
**
**	GSSUBJECT.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsSubject.h,v 1.4 2001/11/27 22:41:08 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSBASE_GSSUBJECT_H )
#define IN_GSBASE_GSSUBJECT_H

#include <gscore/base.h>
#include <ccstl.h>
#include <set>

CC_BEGIN_NAMESPACE( Gs )



/****************************************************************
** Class	   : GsSubject 
** Description : 
****************************************************************/
class GsObserver;

class EXPORT_CLASS_GSBASE GsSubject
{
	typedef CC_STL_SET( GsObserver* ) list_type;
	typedef list_type::size_type size_type;

	list_type
			m_Observers;

protected:

	GsSubject();
	GsSubject( const GsSubject& Other );
	GsSubject &operator=( const GsSubject &Rhs );

public:

	virtual ~GsSubject();
	
	void attach( GsObserver* );
	void dettach( GsObserver* );

	void notify( void* UserData );

	size_type size() const { return m_Observers.size(); }
};



CC_END_NAMESPACE

#endif 
