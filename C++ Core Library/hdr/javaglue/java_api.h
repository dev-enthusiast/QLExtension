/****************************************************************
**
**	javaglue/java_api.h
**
**	Copyright 1992-1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/javaglue/src/javaglue/java_api.h,v 1.5 2000/05/03 20:48:13 nauntm Exp $
**
****************************************************************/

#if !defined( IN_JAVA_API_H )
#define IN_JAVA_API_H

#include <javaglue/base.h>

EXPORT_C_JAVAGLUE void Java_JVMInit();

EXPORT_C_JAVAGLUE void JavaConsoleInit();
EXPORT_C_JAVAGLUE void JavaAppletConsoleInit( jobject console );

//
// Some datatypes visible to secdb...
//   Java VM     - the Java virtual machine.  This is a shared, indestructible object.
//   Java Object - a Java object.
//   Java Class  - a Java class.
//
// Note that hanging onto Java Objects and Java Classes causes them to be pinned in memory.
//

EXPORT_C_JAVAGLUE DT_DATA_TYPE DtJavaVM;
EXPORT_C_JAVAGLUE DT_DATA_TYPE DtJavaObject;
EXPORT_C_JAVAGLUE DT_DATA_TYPE DtJavaClass;

#endif
