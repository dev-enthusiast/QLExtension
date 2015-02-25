#ifndef PORTABLE_STRINGS_H
#define PORTABLE_STRINGS_H

#include <stddef.h> // for size_t

// fix - this should be in portable/base.h
#ifdef BUILDING_PORTABLE
#define EXPORT_C_PORTABLE EXPORT_C_EXPORT
#else
#define EXPORT_C_PORTABLE EXPORT_C_IMPORT
#endif

EXPORT_C_PORTABLE int strcasecmp( char const*, char const* );
EXPORT_C_PORTABLE int strncasecmp( char const*, char const*, size_t );

#endif
