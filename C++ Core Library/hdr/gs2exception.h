#ifndef IN_GS2EXCEPTION_H
#define IN_GS2EXCEPTION_H

#ifndef IN_GSBASE_EXCEPTION_H
#include <gsbase_exception.h>
#endif

class GS2Exception : public GsBaseException
{
   public:
      virtual const char* msg() const = 0;
      virtual const char* name() const = 0;
      virtual const char* file() const = 0;
      virtual int line() const = 0;

      virtual const char* what() const throw() {
                return msg();
        }
};

#endif
