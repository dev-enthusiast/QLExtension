/* $Header: /home/cvs/src/portable/src/ccstream.h,v 1.12 2012/06/27 13:57:41 e19351 Exp $ */
/****************************************************************
**
**	ccstream.h	- stream macros to support mutliple compilers
**
**	Include this file instead of <iostream> or <iostream.h> and
**	use CC_ISTREAM instead of istream.
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.12 $
**
****************************************************************/

#if !defined(IN_CCSTREAM_H)
#define IN_CCSTREAM_H

#include <sstream> // this should be pushed out to the files that need this, not included by everyone who just wanted to write to cout.

#if ! defined(CC_NO_STD_STREAMS)

#  include <iostream>
#  include <iomanip>

#else /* CC_NO_STD_STREAMS */

#  include <iostream.h>
#  include <iomanip.h>

#endif /* CC_NO_STD_STREAMS */

#endif

