/****************************************************************
**
**	fcadlib/AdExpandData.h	- class AdExpandData
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/fcadlib/src/fcadlib/AdExpandData.h,v 1.5 2001/11/27 21:41:46 procmon Exp $
**
****************************************************************/

#if !defined( IN_FCADLIB_ADEXPANDDATA_H )
#define IN_FCADLIB_ADEXPANDDATA_H

#include 	<fcadlib/base.h>
#include 	<vector>
#include 	<ccstl.h>
#include 	<gscore/GsDateVector.h>
#include 	<fcadlib/AdExpandArgBase.h>

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_FCADLIB AdExpandData
{
public:

	AdExpandData() : m_ExpandMode(AD_EXPAND_SCALAR), m_Dates(0) {}
	~AdExpandData() 
	{
		if (m_Dates) 
			delete m_Dates;
	}

	size_t size() const;

	AD_EXPAND_MODE mode() const { return m_ExpandMode; }
	const GsDateVector& dates() const { return *m_Dates; }

	void insert(AdExpandArgBase* Arg);
	void insert(AdExpandArgBase* Arg, const GsDateVector& Dates);
	
private:

	AD_EXPAND_MODE 
			m_ExpandMode;

	GsDateVector* 
			m_Dates;

	CC_STL_VECTOR( AdExpandArgBase* )
			m_Args;
};



CC_END_NAMESPACE

#endif 
