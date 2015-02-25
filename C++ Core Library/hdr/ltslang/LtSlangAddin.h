/****************************************************************
**
**	LTSLANGADDIN.H	- 
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/ltslang/src/ltslang/LtSlangAddin.h,v 1.16 2001/03/21 18:49:17 vengrd Exp $
**
****************************************************************/

#if !defined( IN_LTSLANG_LTSLANGADDIN_H )
#define IN_LTSLANG_LTSLANGADDIN_H

#include <ltslang/base.h>
#include <ltslang/LtSlang.h>
#include <ltslang/ExLtSlang.h>
#include <datatype.h>
#include <ccstl.h>
#include <vector>
#include <gsdt/GsType.h>
#include <gscore/GsString.h>
#include <secexpr.h>
#include <gsdt/GsDtDictionary.h>
#include <fcadlib/adexcel.h>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : LtSlangAddin 
** Description : A class representing an addin written in slang.
**				 it has information on the arg types, return type,
**				 and the slang function to call to do its work.
****************************************************************/

class EXPORT_CLASS_LTSLANG LtSlangAddin
{
public:

	static const LtSlangAddin &AddinByNumber( unsigned AddinNumber ) { return s_LtSlangAddin[ AddinNumber ]; }

	unsigned NumArgs() const { return m_ArgNames.size(); }

	const GsString &AddinName() const { return m_AddinName; }

	const GsString &ArgName(		  unsigned ArgNumber ) const { return m_ArgNames[		   ArgNumber ]; }
	const GsType   *ArgType(		  unsigned ArgNumber ) const { return m_ArgTypes[		   ArgNumber ]; }
	const bool		ArgTypeIsDtValue( unsigned ArgNumber ) const { return m_ArgTypeIsDtValues[ ArgNumber ]; }
	const bool		ArgIsRequired(	  unsigned ArgNumber ) const { return m_ArgIsRequired[	   ArgNumber ]; }

	const DT_VALUE *SlangFunction() const { return &m_SlangFunc; }

	const SLANG_NODE *SlangNode() const
	{
		DT_SLANG
				*Slang = (DT_SLANG *)DT_VALUE_TO_POINTER( &m_SlangFunc );

		if( !Slang )
			return NULL;
		else
			return Slang->Expression;
	}	

	bool IsRegistered() const { return m_Registered;   }
	bool NeedsLoading() const { return m_NeedsLoading; }

	static GsStatus RegisterAddin( 
		const GsString &AddinName, 
		const GsString &FunctionDescription, 
		const CC_STL_VECTOR( GsString ) &ArgNames, 
		const CC_STL_VECTOR( const GsType * ) &ArgTypes, 
		const CC_STL_VECTOR( bool ) &ArgTypeIsDtValues, 
		const CC_STL_VECTOR( bool ) &ArgIsRequired, 
		const GsDtDictionary &ArgsDescDict,
		const DT_VALUE &SlangFunc, 
		GsString &ResultMsg 
	);

	static GsStatus PreRegisterAddin( 
		const GsString &AddinName, 
		const GsString &SlangScriptName, 
		const GsString &ScopedSlangFunctionName, 
		const GsString &FunctionDescription, 
		const GsDtArray &ArgsArray,
		GsString &ResultMsg 
	);

	static GsStatus LoadAddinByNumber( unsigned AddinNumber );

	static GsStatus UnregisterAllAddins();

	static size_t NumRegisteredAddins() { return s_NextAddinIndex; }

private:

	// Nobody should really be creating these with anything but RegisterAddin.  Leave these private 
	// for manipulating s_LtSlangAddin.

	LtSlangAddin();
	LtSlangAddin( const LtSlangAddin & );

	~LtSlangAddin();

	LtSlangAddin &operator=( const LtSlangAddin &rhs );

	// Subroutine for registering with excel.  Handles both RegisterAddin and PreRegisterAddin

	static GsStatus RegisterAddinWithExcel( 
		const GsString &AddinName, 
		const GsString &FunctionDescription, 
		const CC_STL_VECTOR( GsString ) &ArgNames, 
		const GsDtDictionary &ArgsDescDict,
		GsString &ResultMsg 
	);

	// Data members:

	bool
			m_Registered;

	GsString
			m_AddinName;

	CC_STL_VECTOR( const GsType * )
			m_ArgTypes;

	CC_STL_VECTOR( GsString )
			m_ArgNames;

	CC_STL_VECTOR( bool ) 
			m_ArgTypeIsDtValues; 

	CC_STL_VECTOR( bool ) 
			m_ArgIsRequired; 

	XLOPER
			m_XlRegId;

	DT_VALUE
			m_SlangFunc;

	// Do we still need to load the slang script for it?

	bool
			m_NeedsLoading;

	// Next two are used for addins that are registered using
	// LtSlangPreRegister.

	GsString
			m_LoadScriptName;

	GsString			
			m_LoadFunctionName;	

	// Static data members:

	static LtSlangAddin
			s_LtSlangAddin[ EXLTSLANG_NUM_ADDINS ];

	static unsigned
			s_NextAddinIndex;
	
};


CC_END_NAMESPACE

#endif 
