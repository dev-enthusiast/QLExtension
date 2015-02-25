/****************************************************************
**
**	gsdt/GsDtExceptionInfo.h	- The GsDtExceptionInfo class
**
**	Copyright 2000 - Goldman, Sachs & Co. - New York
**
**		GenInfo
**		Class		- GsDtExceptionInfo
**      Base		- Exception
**      BaseParent	- 
**      HdrName		- gsdt/GsDtExceptionInfo.h
**      FnClass		- GsDtExceptionInfo.cpp 
**      Streamable	- 0
**      Iterable	- 0
**      PrintTo		- 0
**
**	$Log: GsDtExceptionInfo.h,v $
**	Revision 1.7  2011/05/16 22:19:44  khodod
**	Eliminate redundant type names that cause a warning.
**	AWR: #177463
**
**	Revision 1.6  2001/11/27 22:45:00  procmon
**	Reverted to CPS_SPLIT.
**
**	Revision 1.4  2001/07/17 19:25:50  shahia
**	store LastError in SlangExceptions when SLANG_ERROR is caught
**	
**	Revision 1.3  2001/06/26 09:33:04  shahia
**	added LineEnd and ColEnd num info to GsDtExceptionInfo
**	
**	Revision 1.2  2001/06/01 13:30:15  walkerpa
**	Change callMemberFn signature to include arg count
**	
**	Revision 1.1  2001/01/24 16:03:28  shahia
**	Implementing slang exceptions, change GsDtException to GsDtExceptionInfo
**	
**	
**
****************************************************************/

#if !defined( IN_GSDT_EXCEPTION_INFO_H )
#define IN_GSDT_EXCEPTION_INFO_H
#include	<datatype.h>
#include	<gscore/GsString.h>
#include	<gsdt/GsDt.h>
#include	<gsdt/FormatInfo.h>




CC_BEGIN_NAMESPACE( Gs )

/*
**	Forward declare classes
*/


class EXPORT_CLASS_GSDTLITE GsDtExceptionInfo : public GsDt
{
	GSDT_DECLARE_TYPE( GsDtExceptionInfo )
public:
	static const char* const ErrorCodeLabel;
	static const char* const SlangModuleNameLabel;
	static const char* const SlangLineNumLabel;
	static const char* const SlangLineEndNumLabel;
	static const char* const SlangColNumLabel;
	static const char* const SlangColEndNumLabel;
	static const char* const CPPModuleNameLabel;
	static const char* const CPPLineNumLabel;
	static const char* const ObjectLabel;
	static const char* const LastErrorLabel;

	
	GsDtExceptionInfo();
	GsDtExceptionInfo(
		int			ErrorCode,
		GsString	SlangModuleName,
		long		SlangLineNum,
		long		SlangLineEndNum,
		long		SlangColNum,
		long		SlangColEndNum,
		GsString	CPPModuleName,
		long		CPPLineNum,
		const GsDt*		pObj,
		GsString	LastError = ""
		);
	GsDtExceptionInfo( const GsDtExceptionInfo& );
	virtual ~GsDtExceptionInfo();

	int			getErrorCode() const { return m_ErrorCode; }
	GsString	getSlangModuleName() const { return m_SlangModuleName; }
	long		getSlangLineNum() const { return m_SlangLineNum; }
	long		getSlangLineEndNum() const { return m_SlangLineEndNum; }
	long		getSlangColNum() const { return m_SlangColNum; }
	long		getSlangColEndNum() const { return m_SlangColEndNum; }
	GsString	getCPPModuleName() const { return m_CPPModuleName; }
	long		getCPPLineNum() const { return m_CPPLineNum; }
	GsDt*		getObject() const {  return m_pObject; }
	GsString	getLastError() const { return m_LastError; }


	// GsDt Behaviour, note virtual for info only
	virtual GsDt* CopyShallow() const;
	virtual GsDt* CopyDeep() const;
	virtual size_t GetSize() const;
	virtual size_t GetCapacity() const;
	virtual size_t GetMemUsage() const;
	virtual void GetInfo( FcDataTypeInstanceInfo	&Info ) const;
	virtual void StreamStore( GsStreamOut	&Stream ) const;
	virtual void StreamRead( GsStreamIn	&Stream );
	virtual FcHashCode HashQuick() const;
	virtual FcHashCode HashComplete() const;
	virtual GsStatus BinaryOpAssignSame(	GSDT_BINOP	Op,	const GsDt	&Other );
	virtual int CompareSame( const GsDt	&Other	) const;
	virtual GsBool Truth() const;
	virtual GsBool IsValid() const;
	virtual GsString toString() const;
	virtual GsStatus doFormat(FormatInfo& Info);
	virtual GsStatus callMemberFn( const char *memberName, GsDt	*Args[], int nArgs, GsDt *&RetValue );

		
private:

	int			m_ErrorCode;
	GsString	m_SlangModuleName;
	long		m_SlangLineNum;
	long		m_SlangLineEndNum;
	long		m_SlangColNum;
	long		m_SlangColEndNum;
	GsString	m_CPPModuleName;
	long		m_CPPLineNum;
	GsDt*		m_pObject;
	GsString	m_LastError;

	// don't want copy and assignment
	
	const GsDtExceptionInfo& operator=(const GsDtExceptionInfo&);

};

CC_END_NAMESPACE

#endif

