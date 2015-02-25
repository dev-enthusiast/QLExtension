/****************************************************************
**
**	GSFUNCFIXEDINDEX.H	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fiinstr/src/fiinstr/GsFuncFixedIndex.h,v 1.2 1998/11/23 22:20:14 hammet Exp $
**
****************************************************************/

#if !defined( IN_GSFUNCFIXEDINDEX_H )
#define IN_GSFUNCFIXEDINDEX_H

#include <fiinstr/base.h>

#include <gscore/GsFunc.h>
#include <gscore/GsDate.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FIINSTR GsFuncFixedIndex	: public GsFunc<double, double>
{
public:

	GsFuncFixedIndex(double index);
	GsFuncFixedIndex(const GsFuncFixedIndex&);
	virtual ~GsFuncFixedIndex();

	virtual double operator()( double ) const;

private:

	double m_index;

};

CC_END_NAMESPACE

#endif 
