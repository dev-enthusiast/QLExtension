/****************************************************************
**
**	LTDTARGS.H	- LtDtArgs class
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/LtDtArgs.h,v 1.5 2001/11/27 23:25:01 procmon Exp $
**
****************************************************************/

#if !defined( IN_SECDB_LTDTARGS_H )
#define IN_SECDB_LTDTARGS_H

#include <secdb/base.h>
#include <gscore/types.h>
#include <gscore/gsdt_fwd.h>
#include <ccstl.h>
#include <datatype.h>
#include <vector>

//#include <gsdt/GsDtRDate.h>

#define LT_DT_ARGS_DECLARE int Argc = 0, DT_VALUE** Argv = NULL
#define LT_DT_ARGS_DEFINE int Argc, DT_VALUE** Argv
#define LT_DT_ARGS Argc, Argv
#define LT_DT_MAKE_ARGS(a) a.size(), a.array()
#define LT_DT_SET_ARG(a) (a.array())[0]

CC_BEGIN_NAMESPACE( Gs )

class EXPORT_CLASS_SECDB LtDtArgs
{
  typedef CC_STL_VECTOR(DT_VALUE) dt_value_vec_type;

	DT_VALUE**
			m_Array;
	/*
	** We store the return of array() here, so we own it.
	*/

	dt_value_vec_type 
	  m_DtValues;

	
	void valueAssign( const LtDtArgs& Other );
	void clear();

public:
	
	LtDtArgs();
	LtDtArgs( const LtDtArgs& Rhs );
	~LtDtArgs();

	LtDtArgs& operator=( const LtDtArgs& Rhs );
		
	int size() const { return m_DtValues.size(); }

	DT_VALUE** array();

    /*	
	**  We do not return const here since the SecDb API does not support it.
	*/


	// FIX would like these to be protected: , but cannot seem to make a template fucntion a friend of this class

	void add( double Value );
	void add( DT_DATA_TYPE DataType, void *Pointer ); // pointer is stolen, do not delete
	void add( const GsString& Value );
	void add( const GsDate& Value );
	void add( GsDt* Value ); // We steal this pointer so do not delete it
	void add( DT_VALUE* Value );
};


EXPORT_CPP_SECDB void
		LtDtArgAdd( LtDtArgs& Args, double 					Value ),
		LtDtArgAdd( LtDtArgs& Args, int 					Value ),
		LtDtArgAdd( LtDtArgs& Args, const GsString& 		Value ),
		LtDtArgAdd( LtDtArgs& Args, const GsDate& 			Value ),
		LtDtArgAdd( LtDtArgs& Args, const GsDtDictionary&	Value, bool ConvertToStructure = true ),
		LtDtArgAdd( LtDtArgs& Args, GsDt*	Value );

CC_END_NAMESPACE

#endif 








