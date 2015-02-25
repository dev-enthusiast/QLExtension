/****************************************************************
**
**	LTSDBWHEREPREDICATE.H	- 
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltsecdb/src/ltsecdb/LtSdbWherePredicate.h,v 1.4 2001/01/26 19:51:58 johnsoc Exp $
**
****************************************************************/

#if !defined( IN_LTSECDB_LTSDBWHEREPREDICATE_H )
#define IN_LTSECDB_LTSDBWHEREPREDICATE_H

#include <ltsecdb/base.h>
#include <secdb/LtSecDbObj.h>
//#include <gscore/gsdt_fwd.h>
#include <gscore/GsFunc.h>
#include <gscore/GsString.h>
#include <gscore/GsDate.h>
#include <gsdt/GsDtArray.h>
#include <gsdt/GsDtFuncHandle.h>
#include <gsquant/GsFuncSpecial.h>
#include <secdbcc.h>

CC_BEGIN_NAMESPACE( Gs )

// This templated class performs a single diddle on a single VT and
// evaluates a single value type on some other object in a diddle
// scope where that diddle has been applied.  For more complex
// diddles, we can either write a more advanced class, or simply put a
// special value type on an object that, when diddled to e.g. a
// structure, passes the diddle on to a series of other value types on
// one or more objects that it refers to.
 
template< class DidT, class EvalT >
class LtSdbWherePredicate : public GsFunc< DidT, EvalT >
{
public:

	LtSdbWherePredicate( const GsString &DidVtName, const GsDtArray &DidVtArgs, 
						 LtSecDbObj &DidDbObj,
						 const GsString &EvalVtName, const GsDtArray &EvalVtArgs,
						 LtSecDbObj &EvalDbObj );

	~LtSdbWherePredicate();

	virtual EvalT operator()( DidT DiddleVal ) const;

	// accessors for SecDbDiddleApply class to apply/remove the diddle

    SDB_OBJECT*    DiddleObjectGet() const    { return m_DidDbObj; }
    SDB_VALUE_TYPE DiddleValueTypeGet() const { return m_DidVt;    }
    size_t         DiddleArgcGet() const      { return m_DidArgC;  }
    DT_VALUE**     DiddleArgvGet() const      { return m_DidArgv;  }
	
private:

	LtSdbWherePredicate &operator=( const LtSdbWherePredicate & );

	// Private conversion functions that can be overloaded for cases
	// when DidT::GsDtType and/or EvalT::GsDtType do not exist.

	static void DiddleValToDtValue( DidT DiddleVal, DT_VALUE &DtDidValue );
	static void DiddleValToDtGsDt(  DidT DiddleVal, DT_VALUE &DtDidValue );
	static void EvalValFromDtValue( const DT_VALUE &DtEvalValue, EvalT &EvalVal  );
	
	SDB_VALUE_TYPE   
			m_DidVt;  

	size_t	m_DidArgC;

	DT_VALUE
			**m_DidArgv;

	LtSecDbObj 
			m_DidDbObj;

	SDB_VALUE_TYPE   
			m_EvalVt; 
	
	size_t	m_EvalArgC;

	DT_VALUE
			**m_EvalArgv;
	
	LtSecDbObj 
			m_EvalDbObj;
};



typedef LtSdbWherePredicate< double, double > LtSdbWherePredicateDoubleDouble;
typedef LtSdbWherePredicate< GsDate, double > LtSdbWherePredicateDateDouble;
typedef LtSdbWherePredicate< GsFuncHandle< double, double >, double > LtSdbWherePredicateFuncDouble;

typedef LtSdbWherePredicate< GsVector, GsVector > LtSdbWherePredicateVectorVector;

typedef GsFuncHandle< GsVector, GsVector > GsFuncVectorVector;


// Addins:

EXPORT_CPP_LTSECDB GsFuncScFunc LtSdbWhereDoubleDoubleCreate(
	const GsString &DidVtName, 		// VT to diddle
	const GsDtArray &DidVtArgs, 	// Args to VT to diddle
	const LtSecDbObj &DidDbObj,		// Object to diddle
	const GsString &EvalVtName, 	// VT to evaluate
	const GsDtArray &EvalVtArgs,	// Args to VT to evaluate	
	const LtSecDbObj &EvalDbObj		// Object to evaluate VT on
);


EXPORT_CPP_LTSECDB GsFuncScDateFunc LtSdbWhereDateDoubleCreate(
	const GsString &DidVtName, 		// VT to diddle
	const GsDtArray &DidVtArgs, 	// Args to VT to diddle
	const LtSecDbObj &DidDbObj,		// Object to diddle
	const GsString &EvalVtName, 	// VT to evaluate
	const GsDtArray &EvalVtArgs,	// Args to VT to evaluate	
	const LtSecDbObj &EvalDbObj		// Object to evaluate VT on
);

EXPORT_CPP_LTSECDB GsFuncScFunctional LtSdbWhereFuncDoubleCreate(
	const GsString &DidVtName, 		// VT to diddle
	const GsDtArray &DidVtArgs, 	// Args to VT to diddle
	const LtSecDbObj &DidDbObj,		// Object to diddle
	const GsString &EvalVtName, 	// VT to evaluate
	const GsDtArray &EvalVtArgs,	// Args to VT to evaluate	
	const LtSecDbObj &EvalDbObj		// Object to evaluate VT on
);

EXPORT_CPP_LTSECDB GsFuncVectorVector *LtSdbWhereVectorVector(
	const GsString &DidVtName, 		// VT to diddle
	const GsDtArray &DidVtArgs, 	// Args to VT to diddle
	const LtSecDbObj &DidDbObj,		// Object to diddle
	const GsString &EvalVtName, 	// VT to evaluate
	const GsDtArray &EvalVtArgs,	// Args to VT to evaluate	
	const LtSecDbObj &EvalDbObj		// Object to evaluate VT on
);
	

CC_END_NAMESPACE

#endif 
