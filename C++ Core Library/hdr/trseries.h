/****************************************************************
**
**	TRIARCH.H --- Utilities for manipulating Triarch data
**
**	$Header: /home/cvs/src/tsdb/src/trseries.h,v 1.5 2001/11/27 23:35:05 procmon Exp $
**
****************************************************************/
#if !defined( IN_TRSERIES_H )
#define IN_TRSERIES_H

#ifndef IN_DATE_H
#include <date.h>
#endif

// TR_SOURCE is opaque to external users
typedef struct TR_SOURCE_STRUCT TR_SOURCE;

#define TR_MAX_FACTS	16				/* Max facts per observation in a series */
typedef struct
{
	char		RIC[32];				/* RIC we observed				*/
	char		Periodicity;			/* Periodicity of observations	*/
	int			NumFacts;				/* Numbers of facts				*/
	int			LogObservations;		/* Number of observations according to Reuters */
	int			NumObservations;		/* Actual number of observations */
	DATE		*Date;					/* Dates of observations		*/
	int			Fid[TR_MAX_FACTS];		/* FIDs for each fact			*/
	double		*Fact[TR_MAX_FACTS];	/* Arrays of facts				*/
} TR_SERIES;


#endif /* IN_TRSERIES_H */
