/****************************************************************
**
**	GSSYSTEMMONITOR.H	- The virtual base class API for
**  system monitoring functions. It has a static factory
**  method, and ways to query the system. Note that it is
**  NOT a singleton; that is, if you call createMonitor
**  you take ownership of the deletable monitor object.
**
**	Copyright 2001 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/gsutil/src/gsutil/GsSystemMonitor.h,v 1.4 2001/09/07 14:33:58 singhki Exp $
**
****************************************************************/

#ifndef IN_GSUTIL_GSSYSTEMMONITOR_H
#define IN_GSUTIL_GSSYSTEMMONITOR_H

#include <gsutil/base.h>

#include <gsdt/GsDtGeneric.h>

#include <string>

#include <ccstl.h>
#include <vector>

#include <iostream>

CC_BEGIN_NAMESPACE( Gs )

/****************************************************************
** Class	   : GsSystemMonitor 
** Description : GsSystemMonitor is the virtual base class
**               which is implemented on different
****************************************************************/

class EXPORT_CLASS_GSUTILSYSMON GsSystemMonitor
{
protected:
	GsSystemMonitor( );

public:

	virtual ~GsSystemMonitor( );

	static GsSystemMonitor* createMonitor( );

	/*
	  Return the number of CPUs in the system.
	 */
	virtual int getCpuCount( ) = 0;

	/*
	  Return the CPU load normalized between 0 and getCpuCount * 1.
	  So on a 30 CPU system, a getCpuLoad of 30.0 means a fully loaded
	  system.
	 */
	virtual double getCpuLoad( ) = 0;

	/*
	  Return the total memory available (excluding swap) in bytes
	 */
	virtual long getMemorySize( ) = 0;

	/*
	  Return the total memory available (including swap) in kilobytes
	 */
	virtual long getTotalMemorySize( ) = 0;

	/*
	  Return the total memory used
	 */
	virtual long getTotalMemoryUsed( ) = 0;

	/*
	  For monitoring individual processes, we use this process structure
	 */
	struct process {
		process( ); 

		CC_NS( std, string )   processName;
		long                   processId;
		long                   processParentId;
		long                   processMemUsage;
        double                 processCpuUsage;
	};

	typedef CC_STL_VECTOR( process ) process_vector;

	virtual long getPid() = 0;

	/*
	  We can get the process table out of the system
	 */
	virtual process_vector getProcessTable( ) = 0;

	virtual process_vector getProcessChildren( long pid ) = 0;

	virtual void kill( long pid, long signum ) = 0;

	// Finally, include GsDt support
	typedef GsDtGeneric< GsSystemMonitor > GsDtType;
	virtual GsString toString() const = 0;
	inline static char const* typeName() { return "GsSystemMonitor"; }
};



CC_END_NAMESPACE

#endif 
