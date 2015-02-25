/****************************************************************
**
**	gscore/GsThreadComputation.h	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsbase/src/gscore/GsThreadComputation.h,v 1.7 2001/11/27 22:41:10 procmon Exp $
**
****************************************************************/

#if !defined( IN_GSCORE_GSTHREADCOMPUTATION_H )
#define IN_GSCORE_GSTHREADCOMPUTATION_H

CC_BEGIN_NAMESPACE( Gs )

// Note: No need to EXPORT_CLASS, since all are in-line
class GsThreadComputation
{	
protected:
	GsThreadComputation():m_status( OK ) {}
	virtual ~GsThreadComputation() {}
public:
	virtual void compute() = 0;

	enum Status { OK, Fail, Exception };
	Status getStatus() const { return m_status; }

	// .... data members ....
protected:
	mutable Status m_status;
};

CC_END_NAMESPACE
#endif

