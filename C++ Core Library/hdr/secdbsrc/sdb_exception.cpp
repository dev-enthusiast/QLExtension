#define GSCVSID "$Header: /home/cvs/src/secdb/src/sdb_exception.cpp,v 1.17 2014/06/11 13:11:46 khodod Exp $"
/****************************************************************
**
** $Log: sdb_exception.cpp,v $
** Revision 1.17  2014/06/11 13:11:46  khodod
** Better control flow.
** AWR: #340553
**
** Revision 1.16  2014/06/11 13:09:04  khodod
** Protect against a NULL-pointer dereference for Slang functions that
** call these methods with SLANG_RET set to NULL.
** AWR: #340553
**
** Revision 1.15  2014/06/09 21:36:27  khodod
** Added missing CVS log/header line.
** AWR: #340553
**
**
****************************************************************/

#define BUILDING_SECDB

#include 	<portable.h>
#include 	<gsdt/x_gsdt.h>
#include	<gsdt/GsDtString.h>

#include	<secdb/sdb_exception.h>

CC_USING_NAMESPACE(Gs);



/****************************************************************
**	Routine: SecDbGetException
**	Returns: const GsDtExceptionInfo*
**	Action : If DT_VALUE contain a slang exception, return it other wise return 0;
****************************************************************/

const GsDtExceptionInfo* SecDbGetException(const DT_VALUE& Value)
{
	const GsDtExceptionInfo* pDtException = 0;
	const GsDt* pDt = 0;
	if (Value.DataType == DtGsDt)
	{
		pDt = (const GsDt*)(Value.Data.Pointer);
		pDtException = GsDtExceptionInfo::Cast(pDt);
	}

	return pDtException;
}


/****************************************************************
**	Routine: SecDbIsException
**	Returns: boolean
**	Action : Does DT_VALUE contain a slang exception
****************************************************************/

int SecDbIsException(const DT_VALUE& Value)
{
	return (0 != SecDbGetException(Value));
}

void SecDbInitExceptionDt(DT_VALUE& Value, const GsDtExceptionInfo* pGsDtExceptionInfo)
{
	Value.DataType = DtGsDt;
	Value.Data.Pointer = pGsDtExceptionInfo;	
	
}

/****************************************************************
**	Routine: SecDbInitExceptionRet
**	Returns: void
**	Action : put a GsDtExceptionInfo in Ret Value
****************************************************************/

void SecDbInitExceptionRet(SLANG_RET& Ret, const GsDtExceptionInfo* pGsDtExceptionInfo)
{
	Ret.Type = SLANG_TYPE_STATIC_VALUE;
	SecDbInitExceptionDt(Ret.Data.Value, pGsDtExceptionInfo);
}


/****************************************************************
**	Routine: SecDbFreeExceptionRet
**	Returns: void
**	Action : make a string out of Slang Exception
****************************************************************/

void SecDbFreeExceptionRet(SLANG_RET& Ret)
{
	delete (GsDtExceptionInfo*) SecDbGetException(Ret.Data.Value);
	SLANG_NULL_RET(&Ret);
}


/****************************************************************
**	Routine: SecDbExceptionToString
**	Returns: GsString
**	Action : make a string out of Slang Exception
****************************************************************/

GsString SecDbExceptionToString(const SLANG_RET& Ret)
{
	const GsDtExceptionInfo* pGsDtExceptionInfo = SecDbGetException(Ret.Data.Value);
	return pGsDtExceptionInfo ? pGsDtExceptionInfo->toString() : GsString("");
}


/****************************************************************
**	Routine: SecDbExceptionToCStr
**	Returns: const char*
**	Action : make a (char *) string out of Slang Exception
****************************************************************/

const char * SecDbExceptionToCStr(const SLANG_RET& Ret)
{
	static GsString sMsg = SecDbExceptionToString(Ret);
	return sMsg.c_str();
}


/****************************************************************
**	Routine: SecDbCppExceptionToSlang
**	Returns: boolean
**	Action : should we route C++ exception to slang (based on config)
****************************************************************/

int SecDbCppExceptionToSlang()
{
	static int SECDB_CPP_EXCEP_TO_SLANG_EXCEP = -1;
	if (-1 == SECDB_CPP_EXCEP_TO_SLANG_EXCEP)
		SECDB_CPP_EXCEP_TO_SLANG_EXCEP = !stricmp( SecDbConfigurationGet( "SECDB_CPP_EXCEP_TO_SLANG_EXCEP", NULL, NULL, "false" ), "true" );
	return SECDB_CPP_EXCEP_TO_SLANG_EXCEP;
}



/****************************************************************
**	Routine: SecDbCppExceptionToSlang
**	Returns: None
**	Action : Util Func to route exception C++ exception to slang
****************************************************************/

void SecDbCppExceptionToSlangRet(
	SLANG_RET		*Ret,
	SLANG_NODE		*Root,
	CC_NS(std, exception)& E
)
{
	GsDtString s(E.what());
	SecDbSetRetToException(Ret, Root, &s);

}

/****************************************************************
**	Routine: SecDbCppExceptionToSlang
**	Returns: None
**	Action : Util Func to route exception C++ exception to slang
****************************************************************/

void SecDbCppExceptionToSlangRet(
	SLANG_RET		*Ret,
	SLANG_NODE		*Root,
	CC_NS(Gs, GsException)& E
)
{

	GsDtString s(E.getText());
	SecDbSetRetToException(Ret, Root, &s, ERR_UNKNOWN, E.getFile(), E.getLine());


}

/****************************************************************
**	Routine: SecDbCppExceptionToSlang
**	Returns: None
**	Action : Util Func to route unknown C++ exception to slang
****************************************************************/

void SecDbCppExceptionToSlangRet(
	SLANG_RET		*Ret,
	SLANG_NODE		*Root
)
{
	GsDtString s("Unknown");
	SecDbSetRetToException(Ret, Root, &s);

}

/****************************************************************
**	Routine: SecDbSetRetToException
**	Returns: None
**	Action : Util Func to Return Exception in Ret Value
****************************************************************/

void SecDbSetRetToException(
	SLANG_RET		*Ret,
	SLANG_NODE		*Root,
	GsDt *pGsDt,
	int				ErrCode,
	const char*		CPPFileName,
	int				CPPLine,
	const char*		LastError
)
{
    // Several Slang functions call SlangEvalError SlangEvalWarn
    // with a NULL value passed for the SLANG_RET variable. 
    // So calling those from a try/catch Slang block would cause
    // a NULL pointer dereference below when an error condition
    // happened. 

    if( !Ret )
    {
        ErrMore( "SLANG_RET is null; cannot initialize Exception information" );
        return;
    }

	GsDtExceptionInfo* pGsDtExceptionInfo = new GsDtExceptionInfo(
							ErrCode,
							Root->ErrorInfo.ModuleName,
							Root->ErrorInfo.BeginningLine,
							Root->ErrorInfo.EndingLine,
							Root->ErrorInfo.BeginningColumn,
							Root->ErrorInfo.EndingColumn,
							CPPFileName,
							CPPLine,
							pGsDt,
							LastError
							);

    SecDbInitExceptionRet(*Ret, pGsDtExceptionInfo);
}

