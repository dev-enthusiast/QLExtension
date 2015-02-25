/*******************************************************************************
 *  RProxy: Connector implementation between application and R language
 *  Copyright (C) 1999--2006 Thomas Baier
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 * 
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, a copy is available at
 *  http://www.r-project.org/Licenses/
 *
 *  Conversion functions from SEXP to BDX and vice versa.
 *
 ******************************************************************************/

#ifndef _COM_UTIL_H_
#define _COM_UTIL_H_

#ifdef __cplusplus
      extern "C" {
#endif

#define RCOM_CLSNAME "COMObject"

typedef void* RCOM_OBJHANDLE;
#define RCOM_NULLHANDLE NULL

#undef Free
#undef ERROR
#include <windows.h>
#include <ole2.h>
#undef Free
#undef ERROR
#include <R.h>
#include <Rinternals.h>

/** get the COM object by handle */
LPDISPATCH com_getObject(RCOM_OBJHANDLE handle);
/** get the COM object index by handle */
RCOM_OBJHANDLE com_getHandle(SEXP handle);
/** add the COM object and return the new handle */
RCOM_OBJHANDLE com_addObject(LPDISPATCH object);
/** create an SEXP for a handle */
SEXP com_createSEXP(RCOM_OBJHANDLE handle);

#ifdef __cplusplus
}
#endif

#endif
