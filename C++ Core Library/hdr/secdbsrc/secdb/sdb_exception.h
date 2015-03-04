/****************************************************************
**
**	secdb/sdb_exception.h	- Slang Exception support
**
**	Copyright 1998 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/secdb/src/secdb/sdb_exception.h,v 1.9 2014/06/10 20:21:40 khodod Exp $
**
****************************************************************/

#ifndef IN_SDB_EXCEPTION_H
#define IN_SDB_EXCEPTION_H 

#include <secdb/base.h>
#include <datatype.h>
#include <secexpr.h>
#include <gsdt/GsDtExceptionInfo.h>

#include <exception> // for std::exception
#include <string> // for std::string

class EXPORT_CLASS_SECDB TryBlockGuard
{
public:
	TryBlockGuard(SLANG_SCOPE* pScope);   
        
	~TryBlockGuard();
	static void RestoreLastErrorFunc();

private:
	static			CC_STL_STACK(void*) s_OldErrorFuncs;
	void*           m_OldErrorFunc;
    SLANG_SCOPE*    m_pScope;
};

EXPORT_CPP_SECDB const CC_NS(Gs, GsDtExceptionInfo)* SecDbGetException(const DT_VALUE& Value);
EXPORT_CPP_SECDB int SecDbIsException(const DT_VALUE& Value);
EXPORT_CPP_SECDB void SecDbInitExceptionDt(DT_VALUE& Value, const CC_NS(Gs, GsDtExceptionInfo)* pGsDtExceptionInfo);
EXPORT_CPP_SECDB void SecDbInitExceptionRet(SLANG_RET& Ret, const CC_NS(Gs, GsDtExceptionInfo)*  pGsDtExceptionInfo);
EXPORT_CPP_SECDB void SecDbFreeExceptionRet(SLANG_RET& Ret);
EXPORT_CPP_SECDB CC_NS(std, string) SecDbExceptionToString(const SLANG_RET& Ret);
EXPORT_CPP_SECDB const char* SecDbExceptionToCStr(const SLANG_RET& Ret);
EXPORT_CPP_SECDB void SecDbSetRetToException(SLANG_RET *Ret, SLANG_NODE *Root, CC_NS(Gs,GsDt)* pGsDt, int ErrCode = ERR_UNKNOWN, const char* CPPFileName = "",int	CPPLine = 0, const char* LastError = "" );


EXPORT_CPP_SECDB int SecDbCppExceptionToSlang();
EXPORT_CPP_SECDB void SecDbCppExceptionToSlangRet(SLANG_RET *Ret,SLANG_NODE *Root);
EXPORT_CPP_SECDB void SecDbCppExceptionToSlangRet(SLANG_RET *Ret,SLANG_NODE *Root, CC_NS(std, exception)& E);
EXPORT_CPP_SECDB void SecDbCppExceptionToSlangRet(SLANG_RET *Ret,SLANG_NODE *Root, CC_NS(Gs, GsException)& E);


#endif	
