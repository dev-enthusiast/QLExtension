/****************************************************************
**
**	GsXImplementation.h - C-Preprocessor implementation of GsX
**						  exception classes
**
** $Header: /home/cvs/src/gsbase/src/gscore/GsXImplementation.h,v 1.11 2011/05/16 20:06:09 khodod Exp $
**
**	Usage:
**		#define BUILDING_xxx
**		#include <portable.h>
**		#include <...>
**		CC_BEGIN_NAMESPACE( Gs )
**		#define GSXI_NAME   "DomainError"
**		#define GSXI_TYPE   GsXDomainError
**		#define GSXI_PARENT GsException
**		// Optional flags
**		#define GSXI_NO_DTOR 1
**		#include <GsXImplementation.h>
**		CC_END_NAMESPACE
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Log: GsXImplementation.h,v $
**	Revision 1.11  2011/05/16 20:06:09  khodod
**	GCC fixes.
**	AWR: #177463
**
**	Revision 1.10  2001/11/27 22:41:12  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.8  2001/07/23 18:09:42  simpsk
**	remove dangerous behavior-modification from GsException::raise.
**	
**	Revision 1.7  2001/01/26 18:07:47  brownb
**	use CC_NS macro with string
**	
**	Revision 1.6  1999/03/18 16:34:19  gribbg
**	Change GsString to inherit from string.
**	Various other fixes to increase Solaris portability.
**	
**	Revision 1.5  1999/02/11 02:20:26  gribbg
**	Remove throwspecs, per code review
**	
**	Revision 1.4  1998/10/20 21:31:03  gribbg
**	More minor changes for George
**	
**	Revision 1.3  1998/10/09 22:13:21  procmon
**	added Header line
**	
**	Revision 1.2  1998/09/29 16:26:10  gribbg
**	Use macros to declare/implement GsException classes
**	
**	Revision 1.1  1998/09/27 21:21:17  gribbg
**	Add GsXEval exceptions
**	
**	
****************************************************************/


#ifndef GSXI_NAME
#error	GsXImplementation.h - Expected GSXI_NAME to be defined
#endif
 
#ifndef GSXI_TYPE
#error	GsXImplementation.h - Expected GSXI_TYPE to be defined
#endif 

#ifndef GSXI_PARENT
#error	GsXImplementation.h - Expected GSXI_PARENT to be defined
#endif 


/****************************************************************
**	Routine: GSXI_TYPE::GSXI_TYPE
**	Returns: 
**	Action : constructor
****************************************************************/

GSXI_TYPE::GSXI_TYPE(
	GsString const&	Text,
	char const*		File,
	int				Line
)
:	GSXI_PARENT( Text, File, Line )
{
}

/****************************************************************
**	Routine: GSXI_TYPE::getName
**	Returns: name of exception
**	Action : Identify type of exception
****************************************************************/

char const* GSXI_TYPE::getName() const
{
	return GSXI_NAME;
}

/****************************************************************
**	Routine: GSXI_TYPE::~GSXI_TYPE
**	Returns: 
**	Action : destructor
****************************************************************/

GSXI_TYPE::~GSXI_TYPE() throw()
{
}
#undef GSXI_NAME
#undef GSXI_TYPE
#undef GSXI_PARENT
#undef GSXI_NO_DTOR
