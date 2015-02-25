/****************************************************************
**
**	LTADDINDATE.H	- LtAddinDate functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/LtAddinDate.h,v 1.3 2001/03/19 15:33:37 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_LTADDINDATE_H )
#define IN_LTSECDB_LTADDINDATE_H

#include <ltsecdb/base.h>
#include <gscore/GsDayCountISDA.h>
#include <gscore/GsSymDate.h>

CC_BEGIN_NAMESPACE( Gs )



EXPORT_CPP_LTSECDB  double
		LtDayCountFractionISDA( const GsDate& Start, const GsDate& End, const GsDayCountISDA& Dc );

EXPORT_CPP_LTSECDB  GsDate
		LtSymDate( const GsSymDate& SymDate, const GsString& Location );

CC_END_NAMESPACE

#endif 
