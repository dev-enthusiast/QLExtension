#if !defined( IN_GSBASE_EXCEPTION_H )
#define IN_GSBASE_EXCEPTION_H 

#include <exception>

class GsBaseException : public CC_NS(std,exception)
{
public:
	virtual ~GsBaseException() throw() {}
};

#endif
