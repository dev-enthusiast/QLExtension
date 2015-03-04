/****************************************************************
**
**	GsNodeValues.cpp	- 
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Log: GsNodeValues.cpp,v $
**	Revision 1.5  2001/03/13 21:21:32  simpsk
**	Minimize includes.
**
**	Revision 1.4  2001/02/02 16:01:50  simpsk
**	Add some utility functions for GsNodeValues.
**	
**	Revision 1.3  2000/07/24 17:20:45  singhki
**	Move NullValue out of inline function into a static member
**	
**	Revision 1.2  2000/07/06 12:57:21  thompcl
**	Removed use of GS namespace
**	
**	Revision 1.1  2000/06/29 17:31:46  thompcl
**	added the GsNodeValues type, to speed up Each vectorization.
**	
**	****************************************************************/

#define BUILDING_SECDB
#include <portable.h>

#include <secdb/GsNodeValues.h>

#include <datatype.h> // for DT_VALUE

/*
** Static members
*/

DT_VALUE GsNodeValues::s_NullValue;


