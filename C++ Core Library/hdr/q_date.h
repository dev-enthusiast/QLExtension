/* $Header: /home/cvs/src/quant/src/q_date.h,v 1.11 2000/03/06 15:01:10 goodfj Exp $ */
/****************************************************************
**
**	Q_DATE.H	
**
**	Copyright 1994 - Goldman, Sachs & Co. - New York
**
**	$Revision: 1.11 $
**
****************************************************************/

#if !defined( IN_Q_DATE_H )
#define IN_Q_DATE_H

#if !defined( IN_Q_BASE_H )
#include <q_base.h>
#endif
#if !defined( IN_QTYPES_H )
#include <qtypes.h>
#endif

EXPORT_C_QBASIC DATE Q_atod(
	char	*what			// string
	);

EXPORT_C_QBASIC DATE	Q_Adate( int Amount, char *what, DATE InpDate, char *Ccy1, char *Ccy2 );
EXPORT_C_QBASIC int		Q_CountDayOfWeek(DATE fr, DATE to, char which);
EXPORT_C_QBASIC int		Q_CountWeekDays(DATE from, DATE to);
EXPORT_C_QBASIC int		Q_CountBusinessDays(DATE from, DATE to, char *ccy1, char *ccy2);
EXPORT_C_QBASIC int		Q_CountHolidays(DATE from, DATE to, char *ccy1, char *ccy2);
EXPORT_C_QBASIC int		Q_CountCalendarDays(DATE from, DATE to);
EXPORT_C_QBASIC int		Q_LastDayOfMonth(int mon, int year);
EXPORT_C_QBASIC DATE	Q_NextWeekDay(DATE from, int cnt);
EXPORT_C_QBASIC DATE	Q_NextDayOfWeek(DATE from, char which, int cnt);
EXPORT_C_QBASIC int		Q_LastDayOfMonth(int mon, int year);

#endif
