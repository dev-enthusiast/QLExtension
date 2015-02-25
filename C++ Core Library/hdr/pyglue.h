/****************************************************************
**
**	pyglue.h - SecDb access to python
**
**	Copyright 2011, Constellation Energy Group, Inc.
**
**	$Log: pyglue.h,v $
**	Revision 1.10  2013/09/11 01:13:41  khodod
**	Fix snprintf warning on Windows.
**	AWR: #310154
**
**	Revision 1.9  2012/05/10 19:33:11  e19351
**	Use release python dlls in all environments (debug python demands debug
**	versions of all dlls loaded by the runtime).
**	AWR: #237078
**
**	Revision 1.8  2011/09/22 20:04:25  khodod
**	- Move python includes to a common header file.
**	- Use a singleton instance for the interpreter.
**	- Do the PYTHONHOME initialization thru a config file.
**	AWR: #237078
**
**	Revision 1.7  2011/08/11 19:08:00  piriej
**	Drop vm destructor
**
**	Revision 1.6  2011/07/21 20:43:18  piriej
**	Add import & run_module methods to Interpreter object
**
**	Revision 1.5  2011/07/19 19:34:38  piriej
**	Naming changes, hopefully easier now to tell the sdb vars from the python vars
**
**	Revision 1.4  2011/07/01 23:26:15  piriej
**	Add pyerr, return python error instead of printing
**
**	Revision 1.3  2011/06/29 17:07:28  khodod
**	Placate the studio compiler.
**	AWR: #237078
**
**	Revision 1.2  2011/06/29 14:58:37  piriej
**	Formatting (sorry, got carried away)
**	Set py_nositeflag to suppress "import 'site' failed" error
**	Set program name
**
**	Revision 1.1  2011/06/25 18:57:13  e63100
**	initial commit
**
****************************************************************/

#ifndef IN_PYGLUE_H
#define IN_PYGLUE_H

#include	<portable.h>
#include	<datatype.h>
#include	<pyglue/base.h>

// HACK: This ugliness to get python's release dll in debug mode. The headers
// #pragma in the debug dll if _DEBUG is defined, which demands the "_d"
// suffix on all libraries loaded by python.
#if defined( _DEBUG ) && defined( _WIN32 )
#undef _DEBUG
#define PYGLUE_RESTORE_DEBUG
#endif

#if defined( _WIN32 )
#define HAVE_SNPRINTF // Do not use the overrides in pyerror.h.
#endif

#include	<Python.h>      // Python runtime.
#include	<datetime.h>    // Python dates.

#ifdef PYGLUE_RESTORE_DEBUG
#define _DEBUG
#undef PYGLUE_RESTORE_DEBUG
#endif

EXPORT_C_PYGLUE		DT_DATA_TYPE	DtPythonObject;
EXPORT_C_PYGLUE		DT_DATA_TYPE	DtPythonInterp;

const char *PyLastError();
class SdbPythonInterpreter;
class SdbPythonObject;

// SdbPythonInterpreter:  An interface to the Python Interpreter.
class EXPORT_CLASS_PYGLUE SdbPythonInterpreter
{
	public:

		int call(const char *method, DT_VALUE &result, DT_VALUE *parms);
		int import(DT_VALUE &result, DT_VALUE *parms);
		int run_module(DT_VALUE &result, DT_VALUE *parms);
    
        static SdbPythonInterpreter* get();

    protected:

		SdbPythonInterpreter() {}
        SdbPythonInterpreter( const SdbPythonInterpreter& ); // Cannot copy these.
        SdbPythonInterpreter& operator =( const SdbPythonInterpreter& ); // Cannot assign them, either.
       
    private:

        static SdbPythonInterpreter *ThePythonInterpreter;
};

// The one and only instance of the Python Interpreter in a Secdb session
//EXPORT_C_PYGLUE SdbPythonInterpreter *ThePythonInterpreter;

// PythonObject:  An interface to PythonObjects.
class EXPORT_CLASS_PYGLUE SdbPythonObject
{
	public:
		int call(const char *method, DT_VALUE &result, DT_VALUE *parms);
};

#endif
