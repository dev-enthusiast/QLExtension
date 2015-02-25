/****************************************************************
**
**	LtFuncs.h	- LtSecDb addin functions
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/LtFuncs.h,v 1.2 2001/03/13 13:35:40 goodfj Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_LTFUNCS_H )
#define IN_LTSECDB_LTFUNCS_H

#include <ltsecdb/base.h>
#include <secdb.h>
#include <secdb/LtSecDbObj.h>
#include <gscore/GsDate.h>
#include <gsdt/GsDtDictionary.h>
#include <gscore/gsdt_fwd.h>

CC_BEGIN_NAMESPACE( Gs )

EXPORT_CPP_LTSECDB LtSecDbObj 
		LtAddByInference( const GsDtDictionary &TagsAndValues );

EXPORT_CPP_LTSECDB LtSecDbObj*
		LtSecurityGet( const GsString& SecurityName );

EXPORT_CPP_LTSECDB LtSecDbObj*
		LtSecurityAddByInference( const GsDtDictionary &TagsAndValues );

EXPORT_CPP_LTSECDB GsDtArray*
		LtSecuritiesAddByInference( const GsDtArray &VtNamesAndValues );

EXPORT_CPP_LTSECDB GsStringVector *
		LtSecurityNames( const GsString &SecurityType, const GsString &FirstName, const GsString &LastName );

EXPORT_CPP_LTSECDB double
		LtDoubleGet( const LtSecDbObj& Security, const GsString& ValueName ),
		LtDoubleGetWithArgs( const LtSecDbObj& Security, const GsString& ValueName, const GsDtArray& );
		
EXPORT_CPP_LTSECDB GsString
		LtStringGet( const LtSecDbObj& Security, const GsString& ValueName ),
		LtStringGetWithArgs( const LtSecDbObj& Security, const GsString& ValueName, const GsDtArray& Array  );

EXPORT_CPP_LTSECDB GsString
		LtSecNameStringGet( const GsString& SecurityName, const GsString& ValueName );

EXPORT_CPP_LTSECDB GsDtArray 
	*	LtArrayGet( const LtSecDbObj& Security, const GsString& ValueName ),
	*	LtArrayGetWithArgs(	const LtSecDbObj& Security, const GsString& ValueName, const GsDtArray& Array );

EXPORT_CPP_LTSECDB GsDtDictionary *
		LtDictionaryGet( const LtSecDbObj& Security, const GsString& ValueName );

EXPORT_CPP_LTSECDB GsDate
		LtDateGet( const LtSecDbObj& Security, const GsString& ValueName ),
		LtDateGetWithArgs( const LtSecDbObj& Security, const GsString& ValueName, const GsDtArray& Array );

EXPORT_CPP_LTSECDB GsString
		LtAsStringGet( const LtSecDbObj& Security, const GsString& ValueName );

EXPORT_CPP_LTSECDB GsDt * 
		LtGsDtGet( const LtSecDbObj& Security, const GsString& ValueName );

EXPORT_CPP_LTSECDB GsDt * 
		LtGsDtGetWithArgs( const LtSecDbObj& Security, const GsString& ValueName, const GsDtArray& Array );

EXPORT_CPP_LTSECDB GsMatrix *
		LtMatrixGet( const LtSecDbObj& Security, const GsString& ValueName );

EXPORT_CPP_LTSECDB GsVector *
		LtVectorGet( const LtSecDbObj& Security, const GsString& ValueName );

EXPORT_CPP_LTSECDB double
		TestLtFieldIsDouble( const LtSecDbObj& Security),
		TestLtFieldPlusArg( const LtSecDbObj& Security, double PlusArg );

EXPORT_CPP_LTSECDB double LtSecurityShow( const LtSecDbObj& Security );

EXPORT_CPP_LTSECDB GsDt
		*GsDtMax( const GsDt& lhs, const GsDt& rhs ),
		*GsDtMin( const GsDt& lhs, const GsDt& rhs );
		
CC_END_NAMESPACE

#endif 

