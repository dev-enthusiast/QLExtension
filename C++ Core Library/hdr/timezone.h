/****************************************************************
**
** timezone.h - timezone API
**
**	$Header: /home/cvs/src/time/src/timezone.h,v 1.6 2002/01/02 17:08:27 khodod Exp $
**
****************************************************************/


#if !defined( IN_TIME_TIMEZONE_H )
#define IN_TIME_TIMEZONE_H

#include <time_base.h>
#include <time.h>                       // system header that defines time_t


/*
 *  Time zone library entry points.
 */

EXPORT_C_TIMEZONE char *gs_asctime(register const struct tm *timeptr);
EXPORT_C_TIMEZONE char *gs_asctime_r(register const struct tm *timeptr,
                                     char *buf);
EXPORT_C_TIMEZONE char *gs_ctime(const time_t *timep);
EXPORT_C_TIMEZONE char *gs_ctime_r(const time_t *timep, char *buf);
EXPORT_C_TIMEZONE double gs_difftime(time_t time1, time_t time0);
EXPORT_C_TIMEZONE struct tm *gs_gmtime(const time_t *timep);
EXPORT_C_TIMEZONE struct tm *gs_gmtime_r(const time_t *timep, struct tm *tm);
EXPORT_C_TIMEZONE struct tm *gs_localtime(const time_t *timep);
EXPORT_C_TIMEZONE struct tm *gs_localtime_r(const time_t *timep,
                                            struct tm *tm);
EXPORT_C_TIMEZONE time_t gs_mktime(struct tm *tmp);
EXPORT_C_TIMEZONE char *gs_tzname[2];
EXPORT_C_TIMEZONE void gs_tzset(void);
EXPORT_C_TIMEZONE size_t gs_strftime(char *s, size_t maxsize,
                                     const char *format, const struct tm *t);

/*
 *  Non-POSIX functions.
 */

EXPORT_C_TIMEZONE time_t gs_timegm(struct tm *timep);


/*
 *  Opaque object which represents a time zone.
 */

struct _TimeZone;
typedef struct _TimeZone TIME_ZONE;

EXPORT_C_TIMEZONE const TIME_ZONE *MakeTimeZone(const char *zone_name);


/*
 *  Time conversion functions that take a time zone parameter for increased
 *  performance.
 */

EXPORT_C_TIMEZONE char *CTimeInZone(const time_t * const timep,
                                    const TIME_ZONE *zone);
EXPORT_C_TIMEZONE struct tm *LocalTimeInZone(const time_t * const timep,
                                             const TIME_ZONE *zone);
EXPORT_C_TIMEZONE time_t MkTimeInZone(struct tm * const tmp,
                                      const TIME_ZONE *zone);

#endif
