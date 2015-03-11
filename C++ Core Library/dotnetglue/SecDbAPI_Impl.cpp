#define GSCVSID "$Header: /home/cvs/src/dotnetglue/src/SecDbAPI_Impl.cpp,v 1.77 2014/01/15 19:20:08 c038571 Exp $"
/**************************************************************************
**
** Date:        2013-09-20
** Copyright:   Copyright (C) 2012-2013, Constellation, an Exelon Company
** Author:      Tom Krzeczek
**
** SecDbAPI_Impl.cpp : Defines exported functions for the SecDbAPI C# DLL.
** This code is based on the original version by Randall Maddox.
** 
***************************************************************************
**
** $Log: SecDbAPI_Impl.cpp,v $
** Revision 1.77  2014/01/15 19:20:08  c038571
** Added comments.
**
** Revision 1.76  2014/01/13 21:54:28  c038571
** AWR: #324190
**
** Revision 1.75  2013/12/09 19:57:20  c038571
** Bug fix.
**
** Revision 1.74  2013/11/20 14:36:54  c038571
**  Fixed typo.
**
** Revision 1.73  2013/11/19 22:47:22  c038571
** Increased Minimum REquired Client Version due to API changes
**
** Revision 1.72  2013/11/14 15:54:28  c038571
** Updated in response to the code review.
**
** Revision 1.71  2013/11/14 13:26:41  c038571
** Versioning support.
**
** Revision 1.70  2013/11/05 20:30:22  c038571
** Bug fix.
**
** Revision 1.69  2013/10/31 19:11:17  c038571
** Added version.
**
** Revision 1.68  2013/10/09 21:41:44  c038571
** Removed pre-compiled header.
**
** Revision 1.67  2013/09/24 18:41:44  c038571
** Logic related to SecDb data types has been moved to SecDbDataTypeAPI.cpp
**
** Revision 1.66  2013/09/20 23:21:07  khodod
** Added standard header.
** AWR: #NA
**
**
**************************************************************************/

#include <iostream>
#include <portable.h>
#include <datatype.h>
#include <err.h>
#include <secdb.h>
#include <slangfnc.h>
#include "dotnetglue.h"


/**
* Minimum required client version
*/
static const char* MinimumRequiredEDSVersion = "1.1.0.0";


/**
* @param client_version EDS version
* @param minimum_version_supported 
* @return TRUE if the caller version is >= MinimumRequiredEDSVersion, FALSE otherwise.
*/
EXPORT_C_EXPORT int APIVersionSupported(
	const char* client_version,
	char** min_version_supported
	)
{
	int supported = TRUE;

	*min_version_supported = (char*)co_task_mem_alloc( strlen(MinimumRequiredEDSVersion) + 1);
	strcpy( *min_version_supported, MinimumRequiredEDSVersion );

	const char* p_mv = *min_version_supported, *p_cv = client_version;
	const char* b_mv = *min_version_supported, *b_cv = client_version;

	while( p_mv )
	{
		if( *p_mv  == '.' || *p_mv == '\0' )
		{
			std::string token_mv(b_mv, p_mv);

			//get the next token from the current version string
			std::string token_cv;

			while( p_cv )
			{
				if( *p_cv == '.' || *p_cv == '\0' )
				{
					token_cv = std::string( b_cv, p_cv );
					b_cv = ++p_cv;

					break;
				}else
					++p_cv;
			}

			if( !*p_mv && *p_cv || *p_mv && !*p_cv ) return FALSE;

			//compare tokens
			int min_ver = ::atoi( token_mv.c_str() );
			int cur_ver = ::atoi( token_cv.c_str() );

			if( min_ver > cur_ver ) 
				return FALSE;
			else if( min_ver < cur_ver ) 
				return TRUE;

			if( *p_mv == '\0' ) break;

			b_mv = ++p_mv;
		}else
			++p_mv;
	}

	return supported;
}


// local helpers
namespace
{

//custom secdb output tracer. The function is provided by .NET client.
custom_secdb_tracer secdb_tracer = NULL;

// Slang context handler names
const char * const printHandleName   = "Print Handle";
const char * const printFunctionName = "Print Function";


/**
* This "PrintFunction" is just a C-linkage wrapper, which is registered in Slang context.
* The real job is done inside secdb_tracer, which is supplied by client application.
*
* @param msg message to be printed.
* @return TRUE upon success, FALSE otherwise.
*/
extern "C" int PrintFunction(void * /* filePtr */ , const char *msg)
{
	if(secdb_tracer)
		return (*secdb_tracer)(msg);
	return TRUE;
}

// dummy structs for typesafe overloading, which void * does not provide!
struct PH_t {};  // print handle type
struct PF_t {};  // print function type

void Cleanup(PH_t * const oldPrintHandle,
                    PF_t * const oldPrintFunction)
{
  if(!secdb_tracer) return;
  SlangContextSet(printHandleName, oldPrintHandle);
  SlangContextSet(printFunctionName, oldPrintFunction);
}

void Cleanup(PH_t       * const oldPrintHandle,
                    PF_t       * const oldPrintFunction,
                    SDB_OBJECT * const objPtr)
{
  Cleanup(oldPrintHandle, oldPrintFunction);
  if(objPtr) SecDbFree(objPtr);
}

void Cleanup(PH_t        * const oldPrintHandle,
                    PF_t        * const oldPrintFunction,
                    SDB_OBJECT  * const objPtr,
                    SLANG_SCOPE * const slangScope)
{
  Cleanup(oldPrintHandle, oldPrintFunction, objPtr);
  if(slangScope) SlangScopeDestroy(slangScope);
}

void Cleanup(PH_t        * const oldPrintHandle,
                    PF_t        * const oldPrintFunction,
                    SDB_OBJECT  * const objPtr,
                    SLANG_SCOPE * const slangScope,
                    SLANG_NODE  * const slangRoot)
{
  Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope);
  if(slangRoot) SlangFree(slangRoot);
}

}  // end unnamed namespace for local helpers

/**
* Sets function for printing in Slang context.
* @param custom_secdb_tracer function for printing in Slang context.
* @return TRUE.
*/
EXPORT_C_EXPORT int SecDbTracerSet(custom_secdb_tracer tracer)
{
	secdb_tracer = tracer;
	return TRUE;
}


/**
* Executes scopeName::funcName in Slang script scriptName.
*
* @param scriptName Slang script hosting the function to be called.
* @param scopeName function scope
* @param funcName function name
* @param argCount number of arguments passed in
* @param args function arguments
* @param useSecDbCache specifies whether to use the SecDbCache (version stored in cache may not be up-to date).
* @return DT_VALUE* representing the result 
* 
* This code is partially copied from sdbInterface.cpp in the tibadapter project.
* @see SlangInvokerImpl::SlangInvokerImpl()
* @see SlangInvokerImpl::Invoke()
*/
EXPORT_C_EXPORT DT_VALUE * SlangExecFunc(
	const char* const scriptName,
	const char* const scopeName,
	const char* const funcName,
	const int argCount,
	DT_VALUE** const args,
	bool useSecDbCache =true)
{
  ErrClear();  // clear any previous error

  if(!(scriptName && *scriptName &&
       scopeName  && *scopeName  &&
       funcName   && *funcName))
  {
    return 0;
  }

  SDB_DB * const sourceDb = SecDbSourceDbGet();

  if(!sourceDb)
  {
    return 0;
  }

  // save current Slang print stuff, using dummy types from above for
  // typesafe overloading, then reset to ours if debug build
  PH_t * const oldPrintHandle   = (PH_t *) SlangContextGet(printHandleName);
  PF_t * const oldPrintFunction = (PF_t *) SlangContextGet(printFunctionName);

  if(secdb_tracer){
	SlangContextSet(printHandleName, 0);
	SlangContextSet(printFunctionName, PrintFunction);
  }

  SDB_OBJECT * const objPtr = SecDbGetByName(scriptName, sourceDb, useSecDbCache ? 0 : SDB_REFRESH_CACHE );

  if(!objPtr)
  {
    Cleanup(oldPrintHandle, oldPrintFunction);
    return 0;
  }

  SLANG_SCOPE  * const slangScope = SlangScopeCreate();

  if(!slangScope)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr);
    return 0;
  }

  SDB_VALUE_TYPE        vtExpr    = SecDbValueTypeFromName("Expression", 0);
  DT_VALUE      * const slangExpr = SecDbGetValue(objPtr, vtExpr);

  if(!(slangExpr && slangExpr->Data.Pointer))
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope);
    return 0;
  }

  SLANG_NODE * const slangRoot = SlangParse((char *)slangExpr->Data.Pointer,
                                            scriptName, SLANG_MODULE_OBJECT);

  if(!slangRoot)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope);
    return 0;
  }

  SLANG_RET      evalExpr = {0};
  SLANG_RET_CODE rc       = SlangEvaluate(slangRoot, SLANG_EVAL_NO_VALUE,
                                          &evalExpr, slangScope);

  SLANG_RET_FREE(&evalExpr);

  if(rc != SLANG_OK)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
    return 0;
  }

  SLANG_VARIABLE_SCOPE  * const varScope = SlangVariableScopeGet(scopeName,
                                                                 false);

  if(!varScope)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
    return 0;
  }

  SLANG_VARIABLE * const var = SlangVariableGetScoped(varScope->VariableHash,
                                                      funcName);

  if(!(var && var->Value && (var->Value->DataType == DtSlang)))
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
    return 0;
  }

  DT_VALUE * const slangFunc = var->Value;
  SLANG_APP_FUNC   appFunc   = {0};
  int              stat      = SlangAppFunctionPush(&appFunc,
                                                    "SecDbAPI_Impl -> Slang",
                                                    slangRoot, slangFunc, 0,
                                                    slangScope);

  if(!stat)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
    return 0;
  }

  SLANG_RET slangRet = {0};
  DT_VALUE  argArray = {0};

  DTM_ALLOC(&argArray, DtArray);

  // copy arguments into argArray
  for(int i = 0; i < argCount; ++i)
  {
    DtAppend(&argArray, args[i]);
  }

  stat = SlangAppFunctionCall(&appFunc, &argArray, &slangRet);
  if(stat != SLANG_OK){
	  Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
	  DTM_FREE(&argArray);
	  SLANG_RET_FREE(&slangRet);
	  return 0;
  }

  SlangAppFunctionPop(&appFunc);
  Cleanup(oldPrintHandle, oldPrintFunction, objPtr, slangScope, slangRoot);
  DTM_FREE(&argArray);

  DT_VALUE  * retPtr = 0;

  if((stat == SLANG_OK) || (stat == SLANG_EXIT))
  {
    retPtr = AllocAndAssign(slangRet.Data.Value);
  }

  SLANG_RET_FREE(&slangRet);

  return retPtr;
}


/**
* Executes arbitrary expression as a Slang script.
*
* @param scriptText expression to be executed.
* @return DT_VALUE* representing the result. 
* 
* This code is partially copied from sdbInterface.cpp in the tibadapter project.
* @see SlangInvokerImpl::evalSlang()
*/
EXPORT_C_EXPORT DT_VALUE * SlangEvalExpr(const char * const scriptText)
{
  ErrClear();  // clear any previous error

  if(!(scriptText && *scriptText))
  {
    return 0;
  }

  // save current Slang print stuff, using dummy types from above for
  // typesafe overloading, then reset to ours
  PH_t * const oldPrintHandle   = (PH_t *) SlangContextGet(printHandleName);
  PF_t * const oldPrintFunction = (PF_t *) SlangContextGet(printFunctionName);

  if(secdb_tracer){
	SlangContextSet(printHandleName, 0);
	SlangContextSet(printFunctionName, PrintFunction);
  }

  SLANG_SCOPE  * const slangScope = SlangScopeCreate();

  if(!slangScope)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, 0);
    return 0;
  }

  SLANG_NODE * const slangRoot = SlangParse(scriptText, scriptText,
                                            SLANG_MODULE_STRING);

  if(!slangRoot)
  {
    Cleanup(oldPrintHandle, oldPrintFunction, 0, slangScope);
    return 0;
  }

  SLANG_RET slangRet = {0};

  const SLANG_RET_CODE stat = SlangEvaluate(
	  slangRoot,
	  SLANG_EVAL_RVALUE,
	  &slangRet,
	  slangScope);

  DT_VALUE *retPtr = 0;
  if((stat == SLANG_OK) || (stat == SLANG_EXIT))
  {
    retPtr = AllocAndAssign(slangRet.Data.Value);
  }

  SLANG_RET_FREE(&slangRet);

  Cleanup(oldPrintHandle, oldPrintFunction, 0, slangScope, slangRoot);

  return retPtr;
}


// end of this file
