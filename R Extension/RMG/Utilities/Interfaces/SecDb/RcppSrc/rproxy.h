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
 ******************************************************************************/

#ifndef _RPROXY_H_
#define _RPROXY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* tracing */
#ifdef NDEBUG
#define RPROXY_ERR(x) R_Proxy_ ## x
#define RPROXY_TRACE(x)
#else
#define RPROXY_ERR(x) R_Proxy_ ## x
#define RPROXY_TRACE(x) R_Proxy_ ## x
#endif


/* exported functions for implementation */

/* 05-05-12 | baier | tracing function new (replaces RPROXYTRACE) */
int R_Proxy_printf(char const* pFormat,...);

#ifdef __cplusplus
}
#endif

#endif
