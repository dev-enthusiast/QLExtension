#define GSCVSID "$Header: /home/cvs/src/rglue/src/sdbInterface.cpp,v 1.4 2013/06/05 22:52:57 khodod Exp $"
/***********************************************************//**
** @file sdbInterface.cpp
**
** Simplistic interface to SecDb.
** Based on Dan Khodorkovsky code.
**
** @author Kirill Lapshin
** 
****************************************************************
**
** $Log: sdbInterface.cpp,v $
** Revision 1.4  2013/06/05 22:52:57  khodod
** Make sure we can call Printf from Slang code.
** AWR: #177555
**
** Revision 1.3  2012/07/27 13:17:42  khodod
** Rename log to something less likely to collide in the global namespace.
** AWR: #274292
**
** Revision 1.2  2012/07/27 13:13:46  khodod
** Fix up the header and add GSCVSID.
** AWR: #274292
**
**
***************************************************************/

#define BUILDING_SDBINTERFACE

#include "sdbInterface.h"

#include <secdb.h>
#include <secexpr.h>
#include <secdrv.h>
#include <sectrans.h>
#include <slangfnc.h>
#include <gscore/GsException.h>

#include <string>
#include <iostream>
#include <map>

// #include <R.h>

CC_USING (std::map);
CC_USING (std::string);


CC_BEGIN_NAMESPACE( Gs )

//global variable. ought to use it to implement PrintFunction correctly.
const SecDbTracer * s_pSecDbTracer = 0;

/*
#ifdef __DEBUG
#define R_log printf
#else
#define R_log Rprintf
#endif
*/
#define R_log printf

class SmartSlangScope
{
	SLANG_SCOPE *Scope;
public:
	SmartSlangScope () 
	{
		Scope = SlangScopeCreate();
		if( Scope == NULL )
		{
			R_log("Error in SmartSlangScopeConstructor: %s\n", ErrGetString() );
        }
    }

	~SmartSlangScope()
	{
		try
		{
			SlangScopeDestroy (Scope);
		}
		catch(...)
		{
		}
	}

	SLANG_SCOPE* get() {return Scope;}
	SLANG_SCOPE* operator->() {return Scope;}

private:
	SmartSlangScope (const SmartSlangScope&);
	SmartSlangScope operator= (const SmartSlangScope&);
};


class SlangInvokerImpl : public SlangInvoker
{
	string			m_scriptName;
	SDB_DB			*m_srcDb;
	SDB_DB			*m_db;
	SmartSlangScope m_slangScope;
	SLANG_NODE		*m_slangRoot;

public:
	SlangInvokerImpl(const char * dbName, const char * scriptName);
	virtual ~SlangInvokerImpl();

	virtual dt_value_var Invoke(
			const char *funcName,
			const DT_ARRAY& args
			);
	
    virtual dt_value_var Invoke(
			const char *funcName,
            DT_VALUE& args
			);
};

class sbdObjectAutoPtr
{
	SDB_OBJECT * m_obj;
public:
	explicit sbdObjectAutoPtr (SDB_OBJECT * obj = 0) : m_obj(obj)
	{
	}

	~sbdObjectAutoPtr()
	{
		if (m_obj) SecDbFree (m_obj);
	}

	SDB_OBJECT * get () const {return m_obj;}

private:
	sbdObjectAutoPtr(const sbdObjectAutoPtr&);//not implemented yet
	sbdObjectAutoPtr& operator= (const sbdObjectAutoPtr&);//not implemented yet
};

class SecDbInterfaceImpl: public SecDbInterface
{
public:
	static void initialize (
			const char * applicationName
			);
	
	static SecDbInterface * instance (
			const char * dbName
			);

	explicit SecDbInterfaceImpl (
			const char * dbName
			);

	virtual ~SecDbInterfaceImpl ();

	virtual dt_value_var getValueType(
				const char *secName, 
				const char *vtName, 
				const DT_ARRAY* args = 0
				);

	virtual void setValueType(
				const char *secName, 
				const char *vtName,
				const DT_VALUE* value, 
				const DT_ARRAY* args = 0
				);

	virtual dt_value_var evalSlang (const char * slangScript);

	virtual SlangInvoker_ptr getInvoker (const char * scriptName);

	virtual SecDbTransaction_ptr startTransaction (const char * desc);

	virtual const char *getDbName() const;

private:
	SDB_DB		*m_db;
	
	CC_NS(std,string) m_dbName;

	struct InstancePool
	{
		typedef map<string, SecDbInterface *> InstMap;
		InstMap instMap;
	public:
		~InstancePool()
		{
			for (InstMap::iterator i = instMap.begin(); i != instMap.end(); ++i)
				delete i->second;
		}

		SecDbInterface* get(const char * dbName)
		{
			InstMap::iterator f = instMap.find(InstMap::key_type(dbName));

			if (f == instMap.end())
			{
				SecDbInterface * sdb = new SecDbInterfaceImpl(dbName);
				instMap[InstMap::key_type(dbName)] = sdb;
				return sdb;
			}
			else
			{
				return f->second;
			}
		}
	};

	static InstancePool s_instPool;

	static std::string s_applicationName;

private:

	void attach();
	void detach();

	friend class AttachGuard;

	// Copy constructor and assignment are not written yet.
	SecDbInterfaceImpl (const SecDbInterfaceImpl&); 
	const SecDbInterfaceImpl & operator=(const SecDbInterfaceImpl&); 
};

class AttachGuard
{
	SecDbInterfaceImpl * m_i;
public:
	explicit AttachGuard(SecDbInterfaceImpl * i) : m_i(i) 
	{
		m_i->attach();
	}
	~AttachGuard()
	{
		m_i->detach();
	}
};

class SecDbTransaction_impl : public SecDbTransaction
{
	SDB_DB * m_db;
	bool m_bRollback;
public:
	SecDbTransaction_impl (SDB_DB * db, const char * desc) : m_db(db), m_bRollback(true)
	{
		if (!SecDbTransBegin (m_db, desc))
		{
			printf ("Failed to begin transaction due to error: %s\n", ErrGetString());
		}
	}

	virtual ~SecDbTransaction_impl ()
	{
		if (m_bRollback)
		{
			if (!SecDbTransAbort (m_db))
			{
				printf ("Failed to abort transaction.");
				//can't throw exception from dtor
				std::cerr << ErrGetString() << std::endl; 
			}
		}
	}

	virtual void commit ()
	{
		if (!SecDbTransCommit (m_db))
		{
			printf("Failed to commit transaction due to error: %s\n", ErrGetString());
			m_bRollback = true;
		}
		else
		{
			m_bRollback = false;
	
		}
	}
};

SecDbInterfaceImpl::InstancePool SecDbInterfaceImpl::s_instPool;
std::string SecDbInterfaceImpl::s_applicationName;


extern "C" int PrintFunction(void *fh, const char *mesg);

/*
 * SlangInterface::SlangInterface will perform the required
 * initializations before a call to Invoke() can be made.
 */

SecDbInterfaceImpl::SecDbInterfaceImpl(
		const char * dbName = 0
	) : 
	m_db(0)
{	  

	if (dbName == 0 || dbName[0] == '\0')
		dbName = getenv("SECDB_DATABASE");

	{
		std::string msg = "Connecting to ";
		if (dbName)
			msg += dbName;
		else
			msg += "default database";
		msg += "\n";
		PrintFunction (0, msg.c_str());
	}
	
	
	// Setup context and scope.
	SlangContextSet("Print Handle", 0); // See secdb:secexpr.c.
	SlangContextSet("Print Function", (void *) PrintFunction);

	// dbName was specified as an argument or taken from SECDB_DATABASE.
	if (dbName) 
	{
		m_dbName = dbName;
	} 
	else 
	{
		attach();
		SDB_DB_INFO info;
		
		if (SecDbDbInfoFromDb(m_db, &info))
		{
			m_dbName = info.DbName;
		}
		else
		{
			printf("SecDbDbInfoFromDb call failed: %s\n", ErrGetString());
        }
		detach();
	}
}

/*
 * SlangInterface::~SlangInterface.
 */

SecDbInterfaceImpl::~SecDbInterfaceImpl()
{
	//TBD: figure out
	//SecDbShutdown is currently broken :( 
//	bool fastAndLoose = true;
//	SecDbShutdown(fastAndLoose);
}


dt_value_var SecDbInterfaceImpl::getValueType(
				const char *secName, 
				const char *vtName,
				const DT_ARRAY* args
				)
{
	AttachGuard attachg(this);

	dt_value_var retVal;

	DT_VALUE *VolValue;

	unsigned Flags = SDB_REFRESH_CACHE | SDB_IGNORE_PATH;
	sbdObjectAutoPtr SecPtr ( SecDbGetByName( secName, m_db, Flags ) );

	if (!SecPtr.get())
	{
		printf ("ERROR: Failed to load security. Returning Null\n");
		DT_VALUE nullValue ;
		nullValue.DataType = DtNull;
		retVal = dt_value_var(&nullValue);
	}
	else
	{

		SDB_VALUE_TYPE VtVol = SecDbValueTypeFromName(
				vtName,   // Name
				NULL ); 			// Data type

		DT_VALUE ** argsptr = 0;
		size_t size = args ? args->Size : 0;
		if (size)
		{
			argsptr = new DT_VALUE*[size];
			for (int i=0; i < size; ++i)
				argsptr[i] = args->Element+i;
		}

		VolValue = SecDbGetValueWithArgs(
				SecPtr.get(),		  // Security pointer
				VtVol,			// Value type
				size,			   // Arg count
				argsptr,		// Argument pointers
				NULL ); 	// Data type restriction

		delete [] argsptr;

		if (!VolValue)
		{
			printf("SecDbGetValueWithArgs failed due to error: %s\n", ErrGetString());
			DT_VALUE nullValue ;
			nullValue.DataType = DtNull;
			retVal = dt_value_var(&nullValue);
		}
		else
		{
			DTM_ASSIGN(retVal.get(), VolValue);
		}
	}

	return retVal;
}


void SecDbInterfaceImpl::setValueType(
				const char *secName, 
				const char *vtName,
				const DT_VALUE* value, 
				const DT_ARRAY* args
				)
{
	AttachGuard attachg(this);
	
	unsigned Flags = SDB_REFRESH_CACHE | SDB_IGNORE_PATH;
	sbdObjectAutoPtr SecPtr ( SecDbGetByName( secName, m_db, Flags ) );

	if (!SecPtr.get())
	{
		printf ("Failed to load security");
		//FIX - GSX_THROW (GsXInvalidOperation, ErrGetString());
	}

	SDB_VALUE_TYPE VtVol = SecDbValueTypeFromName(
			vtName,   // Name
			NULL ); 			// Data type

	DT_VALUE ** argsptr = 0;
	size_t size = args ? args->Size : 0;
	if (size)
	{
		argsptr = new DT_VALUE*[size];
		for (int i=0; i < size; ++i)
			argsptr[i] = args->Element+i;
	}

	int ret = SecDbSetValueWithArgs(
			SecPtr.get(),		  // Security pointer
			VtVol,			// Value type
			size,			   // Arg count
			argsptr,		// Argument pointers
			const_cast<DT_VALUE*>(value),	//Value to set to
			SDB_CACHE_SET/*SDB_SET_RETAINED*/ );	 // SDB_SET_FLAGS

	delete [] argsptr;

	if (!ret)
	{
		printf ("SecDbSetValueWithArgs failed");
		//FIX - GSX_THROW (GsXInvalidOperation, ErrGetString());
	}

	if (!SecDbUpdate(SecPtr.get()))
	{
		printf ("SecDbUpdate failed");
		//FIX - GSX_THROW (GsXInvalidOperation, ErrGetString());
	}
}

dt_value_var SecDbInterfaceImpl::evalSlang(const char *slangScript)
{
	AttachGuard attachg(this);

	dt_value_var retVal;

	SmartSlangScope Scope;

	SLANG_NODE *Expression = SlangParse( slangScript, slangScript, SLANG_MODULE_STRING );
	if( Expression == NULL )
	{
		char buf[ 1024 ];
		sprintf( buf, "Slang Parse Error: @ %s(%s) - (%d,%d) - (%d,%d)\n",
					  SlangModuleNameFromType( SlangErrorInfo.ModuleType ),
					  NULLSTR( SlangErrorInfo.ModuleName ),
					  SlangErrorInfo.BeginningLine,
					  SlangErrorInfo.BeginningColumn,
					  SlangErrorInfo.EndingLine,
					  SlangErrorInfo.EndingColumn );
		printf( buf );
		printf("Check Your Slang Code! Returning NULL\n");
		dt_value_var emptyValue;
		return(emptyValue);
	}

	SLANG_RET Ret;
	SLANG_RET_CODE Status = SlangEvaluate( Expression, SLANG_EVAL_RVALUE, &Ret, Scope.get() );

	if( SLANG_OK == Status || SLANG_EXIT == Status )
	{

		DTM_ASSIGN (retVal.get(), &Ret.Data.Value);
		SLANG_RET_FREE( &Ret );
		SlangFree( Expression );
	}
	else
	{
		char buf[ 1024 ];
		sprintf( buf, "Slang Eval Error: @ %s(%s) - (%d,%d) - (%d,%d)\n",
					  SlangModuleNameFromType( SlangErrorInfo.ModuleType ),
					  NULLSTR( SlangErrorInfo.ModuleName ),
					  SlangErrorInfo.BeginningLine,
					  SlangErrorInfo.BeginningColumn,
					  SlangErrorInfo.EndingLine,
					  SlangErrorInfo.EndingColumn );
		printf( buf );
		SlangFree( Expression );
		printf("Check Your Slang Code! Returning NULL\n");
		dt_value_var emptyValue;
		return(emptyValue);
	}

	return retVal;
}

SecDbTransaction_ptr SecDbInterfaceImpl::startTransaction (const char * desc)
{
	AttachGuard attachg(this);
	return SecDbTransaction_ptr (new SecDbTransaction_impl(m_db, desc) );
}

SlangInvoker_ptr SecDbInterfaceImpl::getInvoker (const char * scriptName)
{
	return SlangInvoker_ptr (new SlangInvokerImpl(getDbName(), scriptName));
}

void SecDbInterfaceImpl::attach()
{
	if (m_db) 
	{
		printf ("ERROR: attach was called without calling detach first\n");
    }
	const char * dbName = getDbName();

	if (dbName[0] == '\0')
	{
		dbName = SecDbConfigurationGet("SECDB_DATABASE",0,0,0);
		//printf("We're using the default database: %s\n", dbName);
	}
	else
	{
		//printf("We're attempting to connect to database: %s\n", dbName);
	}
	m_db = SecDbAttach (dbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_FRONT);
	if (!m_db)
	{
		printf ("SecDbAttach failed due to error: %s\n", ErrGetString());
	}
}

void SecDbInterfaceImpl::detach()
{
	if (!m_db)
    { 
		printf ("ERROR: SecDb Detach was called without calling attach first\n");
    }
	if (!SecDbDetach (m_db))
	{
		printf ("SecDbDetach Failed: %s\n", ErrGetString());
	}
	m_db = 0;
}

const char *SecDbInterfaceImpl::getDbName() const 
{
	return(m_dbName.c_str());
}

SlangInvokerImpl::SlangInvokerImpl(const char * dbName, const char * scriptName) :
	m_scriptName (scriptName), 
	m_srcDb(0),
	m_db(0),
	m_slangRoot(0)
{
	if (m_db) 
	{
		printf ("ERROR: SlangInvokerImpl Constructor - SecDb Attach was called without calling detach first\n");
    }
	else
	{
		if (dbName[0] == '\0')
		{
			dbName = SecDbConfigurationGet("SECDB_DATABASE",0,0,0);
			//printf("We're using the default database: %s\n", dbName);
		}
		else
		{
			//printf("We're attempting to connect to database: %s\n", dbName);
		}

		m_db = SecDbAttach (dbName, SDB_DEFAULT_DEADPOOL_SIZE, SDB_FRONT);
		
		if (!m_db)
		{
			printf ("SecDbAttach Failed Due To Error: %s\n", ErrGetString());
		}
	}

	m_srcDb = SecDbSourceDbGet();
	if (!m_srcDb) {
		printf ("Error in SecDbSourceDbGet(): %s\n", ErrGetString());
	}
	
	// Setup context and scope.
//	SlangContextSet("Print Handle", 0); // See secdb:secexpr.c.
//	SlangContextSet("Print Function", (void *) PrintFunction);

	// Parse and evaluate the script.
	
	unsigned Flags = SDB_REFRESH_CACHE & SDB_IGNORE_PATH;
		
	sbdObjectAutoPtr secPtr ( SecDbGetByName(scriptName, m_srcDb, Flags) );
	if (!secPtr.get()){
		printf ("Error in SlangInvokerImpl Constructor in secPtr.get(): %s\n", ErrGetString());
    }
	else
	{
		SDB_VALUE_TYPE VTExpr = SecDbValueTypeFromName("Expression", NULL);
		DT_VALUE *slangExpr = SecDbGetValue(secPtr.get(), VTExpr);
		if (slangExpr == NULL){
			printf ("Error in SlangInvokerImpl Constructor, slangExpr == NULL with error: %s\n", ErrGetString());
		}
		m_slangRoot = SlangParse((char *) slangExpr->Data.Pointer, scriptName,
								 SLANG_MODULE_OBJECT);
		if (m_slangRoot == NULL){
			printf ("Error in SlangInvokerImpl Constructor, slangRoot == NULL with error: %s\n", ErrGetString());
		}
		SLANG_RET			  evalExpr;
		SLANG_RET_CODE rc = SlangEvaluate(m_slangRoot, SLANG_EVAL_RVALUE, &evalExpr, m_slangScope.get());
		SLANG_RET_FREE(&evalExpr);
		if(rc != SLANG_OK){
			printf ("Error in SlangInvokerImpl Constructor, SlangEvaluate Return Code != SLANG_OK with Error: %s\n", ErrGetString());
		}
	}
}

SlangInvokerImpl::~SlangInvokerImpl()
{
	//BIG FIX THIS!!!! Not sure why this is crashing
	if (!SecDbDetach (m_db))
	{
		printf ("ERROR: SecDbDetach Failed in SlangInvokerImpl Destructor with error: %s\n", ErrGetString());
	}
	m_db = 0;
	SlangFree(m_slangRoot);
}

dt_value_var SlangInvokerImpl::Invoke(
		const char *scopedFuncName,
		const DT_ARRAY& args
		)
{


	DT_VALUE argArray;
	DTM_ALLOC(&argArray, DtArray);
	for (int i = 0; i < args.Size; i++)
		DtAppend(&argArray, &args.Element[i]);

	return Invoke(scopedFuncName, argArray);
}

dt_value_var SlangInvokerImpl::Invoke(
		const char *scopedFuncName,
	    DT_VALUE& args
		)
{
	SLANG_VARIABLE_SCOPE *varScope = NULL;
	SLANG_VARIABLE		 *var = NULL;
	SLANG_RET			  ret;
	SLANG_NULL_RET(&ret);
	DT_VALUE			 *slangFunc = NULL;

	//Initialize the return value to null in case we've 
	//accessed a bad function (or other nasty problem)
	DT_VALUE nullValue ;
	DTM_INIT(&nullValue);
	dt_value_var retVal = dt_value_var(nullValue);
		
	SLANG_APP_FUNC		  appFunc;
	int 				  stat;

	std::string scope, funcName = scopedFuncName;

	//split (potentially) scoped function name on scope name and function name
	std::string::size_type pos;
	if ( (pos = funcName.find("::")) != std::string::npos)
	{
		scope = funcName.substr (0, pos);
		funcName = funcName.substr (pos+2);
	}
	


	if (!scope.empty())
	{
		varScope = SlangVariableScopeGet(scope.c_str(), FALSE);
		if (varScope == NULL) {
			R_log("Can't find scope '%s'", scope.c_str());
			R_log (" with error: %s\n", ErrGetString());
		}
		else
		{
			var = SlangVariableGetScoped(varScope->VariableHash, funcName.c_str());
		}
	}
	else
	{
		var = SlangVariableGet(funcName.c_str(), m_slangScope->FunctionStack);
	}

	if (var == NULL) {
		R_log("Can't find function '%s'", funcName.c_str());
		R_log (" with error: %s\n",ErrGetString());
	}
	else
	{
		slangFunc = var->Value;
		if (slangFunc->DataType != DtSlang) {
			R_log("%s::%s expected a Slang Function, not %s",
					scope.c_str(), funcName.c_str(), slangFunc->DataType->Name);
			R_log ("Error: %s\n", ErrGetString());
		}
		
		memset(&appFunc, 0, sizeof(appFunc));
		stat = SlangAppFunctionPush(&appFunc, "SlangInterface Door",
									m_slangRoot, slangFunc, NULL, m_slangScope.get());
		if (!stat) {
			R_log ("SlangAppFunctionPush failed with error: %s\n", ErrGetString());
		}


		stat = SlangAppFunctionCall(&appFunc, &args, &ret);
		
		SlangAppFunctionPop(&appFunc);

		bool slangOK = stat == SLANG_OK;
		bool slangEXIT = stat == SLANG_EXIT;

		if ( !slangOK && !slangEXIT ) 
		{
			R_log("SlangAppFunctionCall() failed: %s\n", ErrGetString());
		}
	}

	DT_VALUE *returnPtr = retVal.get();
	DT_VALUE *dataValue = &ret.Data.Value;
	DTM_ASSIGN(returnPtr, dataValue);
	SLANG_RET_FREE(&ret);

	return retVal;
}

void SecDbInterface::initialize (const char * applicationName)
{
	SecDbInterfaceImpl::initialize (applicationName);
}

SecDbInterface * SecDbInterface::instance (const char * dbName)
{
	return SecDbInterfaceImpl::instance (dbName);
}

void SecDbInterface::setTracer (const SecDbTracer * tracer)
{
	s_pSecDbTracer = tracer;
}

void SecDbInterfaceImpl::initialize (const char * applicationName)
{
	if (applicationName == 0 || applicationName[0] == '\0')
	{
		printf ("Error: applicationName must not be empty.\n");
    }  
	if ( !s_applicationName.empty() ){
		printf("SecDbInterface initialize should be called only once.\n");
	}

	s_applicationName = applicationName;
	
	DtInitialize(NULL, NULL);

	char *username = getenv("USER_NAME");
	if (!username) 
		username = getenv("USERNAME");

	SDB_DB * db = SecDbStartup(NULL, SDB_DEFAULT_DEADPOOL_SIZE,
						(SDB_MSG_FUNC) NULL, (SDB_MSG_FUNC) NULL,
						applicationName, username);
	if (db == NULL)
	{
		printf ("Error in initialize, db==NULL with error: %s\n", ErrGetString());
    }
	if (!SecDbDetach (db))
	{
		printf ("SecDbDetach failed with error: %s\n", ErrGetString());
	}
	db = 0 ;
}

SecDbInterface * SecDbInterfaceImpl::instance (const char * dbName)
{
if ( s_applicationName.empty() ){
		printf ("Error: SecDbInterface initialize must be called prior to SecDbInterface instance.\n");
	}
	return s_instPool.get(dbName);
}

class StdoutSecDbTracer : public SecDbTracer
{
public:
	virtual void trace (const char * msg) const
	{
		CC_NS(std,cout) << msg;// << CC_NS(std,endl);
	}
	virtual void vtrace (const char * msg, va_list argp) const
	{
		CC_NS(std,cout) << msg;// << CC_NS(std,endl);
	}
};

const SecDbTracer * SecDbTracer::stdouttracer ()
{
	static StdoutSecDbTracer tracer;
	return &tracer;
}

/*
 * Must be called using C calling conventions.
 */

extern "C" int PrintFunction(void *dummy, const char *mesg) 
{
	if (s_pSecDbTracer)
		s_pSecDbTracer->trace (mesg);
    return 0; // Success...
}



CC_END_NAMESPACE


