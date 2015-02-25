/** @file sdbInterface.h
  *
  * Simplistic interface to SecDb.
  * Based on Dan Khodorkovsky code.
  *
  * @author Kirill Lapshin
  *
  *	$Header: /home/cvs/src/mtglue/src/sdbInterface.h,v 1.8 2002/12/30 23:24:25 vodzaj Exp $
  */

#ifndef _SDBINTERFACE_H_INCLUDED
#define _SDBINTERFACE_H_INCLUDED

#include <portable.h>
#include <datatype.h>

#include <memory>

#ifdef BUILDING_SDBINTERFACE
#define EXPORT_C_SDBINTERFACE			EXPORT_C_EXPORT
#define EXPORT_CPP_SDBINTERFACE			EXPORT_CPP_EXPORT
#define EXPORT_CLASS_SDBINTERFACE		EXPORT_CLASS_EXPORT
#define EXPORT_TEMPLATE_SDBINTERFACE		EXPORT_TEMPLATE_EXPORT
#else
#define EXPORT_C_SDBINTERFACE			EXPORT_C_IMPORT
#define EXPORT_CPP_SDBINTERFACE			EXPORT_CPP_IMPORT
#define EXPORT_CLASS_SDBINTERFACE		EXPORT_CLASS_IMPORT
#define EXPORT_TEMPLATE_SDBINTERFACE		EXPORT_TEMPLATE_IMPORT
#endif


CC_BEGIN_NAMESPACE( Gs )

//Note: All functions throw GsException to notify of error condition

class SlangInvoker;
typedef CC_NS(std,auto_ptr<SlangInvoker>)  SlangInvoker_ptr;

class SecDbTransaction;
typedef CC_NS(std,auto_ptr<SecDbTransaction>)  SecDbTransaction_ptr;

class SecDbTracer;


/** Main Interface class to SecDb.
  * One can think of this class as of connection to SecDb database. 
  * It provides nice C++ wrapper to SecDb C API.
  * Functionality of this interface is quite limited though, it was designed to
  * provide simple usage of SecDb: connect to database, execute Slang expression,
  * Invoke function from the Slang library, etc.
  */
class SecDbInterface
{
public:
	EXPORT_CPP_SDBINTERFACE static void initialize (const char * applicationName); 
	
	/** Get connection to given database, database nami is optional, and if ommitted it is deduced from evironment variable */
	EXPORT_CPP_SDBINTERFACE static SecDbInterface * instance (
			const char * dbName = 0
			); 

	/** Set Tracer -- object which will print out Slang output as well as some other messages, by default all the output goes to nul.
	  * Unfortunetly there is only one tracer object per application, so there is no way to print out
	  * trace in different ways for different instances of SecDbInterface. This limitation dictated by
	  * underlying SecDb C API.
	  */
	EXPORT_CPP_SDBINTERFACE static void setTracer (const SecDbTracer * tracer = 0); 

	virtual ~SecDbInterface () {}

	/** Get Value of given ValueType on given object */
	virtual dt_value_var getValueType(
				const char *secName, 
				const char *vtName, 
				const DT_ARRAY* args = 0
				) = 0;

	/** Set Value of given ValueType on given object */
	virtual void setValueType(
				const char *secName, 
				const char *vtName,
				const DT_VALUE* value, 
				const DT_ARRAY* args = 0
				) = 0;

	/** Evaluate given slang code. Note: slangScript is a code not a script name! */
	virtual dt_value_var evalSlang (const char * slangScript) = 0;

	/** Load Slang library and return SlangInvoker interface capable of invoking functions from the library */
	virtual SlangInvoker_ptr getInvoker (const char * scriptName) = 0;

	/** Start Transaction. Returns transaction object which by default rollbacks transaction in dtor, unless being explicetly asked to commit it. */
	virtual SecDbTransaction_ptr startTransaction (const char * desc = "") = 0;

	/** Get database name to which current interface connected to */
	virtual const char *getDbName() const = 0;
};

/** SlangInvoker provides ability to invoke functions defined in slang script (library). */
class SlangInvoker
{
public:
	/** Invoke given function from the slang script.
	  * @param funcName - name of the function, could be scoped, e.g. MyScope::MyFoo
	  * @param args - arguments to the function
	  * @return return value of the function 
	  */
	virtual dt_value_var Invoke(
			const char *funcName,
			const DT_ARRAY& args
			) = 0;

	virtual ~SlangInvoker() {}
};

/** SecDb Transaction object.
  * Uses "Initialization is acquisition" paradigm
  * During object construction Transaction started, unless being explicetly commited
  * the transaction will be rolled back in destructor
  */
class SecDbTransaction
{
public:
	/** Don't be fooled with empty body, it is an interface, actual implementation will roll transaction back. */
	virtual ~SecDbTransaction () {};
	/** Commit transaction to SecDb */
	virtual void commit () = 0;
};

/** Tracer interface. 
  * SecDbInterface itself, as well as Slang scripts ans function could print something out,
  * since particular applications might want to print it in different ways (to stdout, to the file,
  * ignore, to some proprietary interface such as mexPrintf for Matlab etc)
  * actual printing is a responsibility of the client.
  * Client either reuses one of existing implementations of the SecDbTracer interface, or creates his
  * own implementation. Implementation passed then to SecDbInterface.
  */
class SecDbTracer
{
public:
	virtual ~SecDbTracer() {}

	/** Actual printing. To be defined in implementations */
	virtual void trace (const char * msg) const =0;

	/** Returns predefined implementation of the interface which prints to stdout */
	EXPORT_CPP_SDBINTERFACE static const SecDbTracer * stdouttracer ();
};

CC_END_NAMESPACE

#endif
