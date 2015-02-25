/* $Header: /home/cvs/src/gsdate/src/gscore/GsPTime.h,v 1.5 2001/11/27 22:43:10 procmon Exp $ */
/****************************************************************
**
**	GsPTime.h	- GsPTime class
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsdate/src/gscore/GsPTime.h,v 1.5 2001/11/27 22:43:10 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSPTIME_H )
#define IN_GSCORE_GSPTIME_H

#include	<gscore/base.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_GSDATE GsPTime
{
public:
	GsPTime() : m_val(0) {}
	explicit GsPTime( double v ) : m_val(v) {}
	~GsPTime() {}
	operator double() const { return m_val; }

private:
	double	m_val;
};

CC_END_NAMESPACE
#endif

