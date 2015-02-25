/****************************************************************
**
**	JAVA_PORTABLE.H	- 
**
**	Copyright 1999 - Goldman, Sachs & Co. - New York
**
**	$Header: /home/cvs/src/javaglue/src/javaglue/java_portable.h,v 1.18 2001/11/27 22:48:54 procmon Exp $
**
****************************************************************/

#if !defined( IN_JAVAGLUE_JAVA_PORTABLE_H )
#define IN_JAVAGLUE_JAVA_PORTABLE_H

#include <javaglue/base.h>

#define USING_JVM_12

#if defined( USE_GSBASE )
#include	<gscore/GsString.h>
#include	<gscore/GsHashForward.h>
CC_USING_NAMESPACE( std );

class 	JavaMethodSignature;
class  	JavaMethodInfo;
struct 	JavaMethodSignatureToIntFunctor;
struct 	JavaMethodSignatureComparator;

typedef Gs::GsHashMap< 
		JavaMethodSignature*, JavaMethodInfo*, JavaMethodSignatureToIntFunctor, JavaMethodSignatureComparator > 
		SigToMethodHash;

#include	<gscore/GsHash.h>
CC_USING_NAMESPACE( Gs );
#else

#include    <hash.h>
#include    <string.h>
#include    <stdlib.h>

class GsString {
	char *m_s;
  public:
	GsString() {
		m_s = NULL;
	}
	GsString( const char *s) {
		m_s = strdup(s);
	}
	GsString( const GsString &rhs ) {
		m_s = strdup( rhs.c_str() );
	}
	GsString( const GsString *rhs ) {
		m_s = strdup( rhs->c_str() );
	}
	GsString &operator=( const GsString &rhs ) {
		m_s = strdup( rhs.c_str() );
		return *this;
	}
	~GsString() {
		free( m_s );
	}
	int operator==( const GsString &rhs ) {
		return !strcmp( m_s, rhs.m_s );
	}
	int operator!=( const GsString &rhs ) {
		return !strcmp( m_s, rhs.m_s );
	}
	GsString operator+( const GsString &rhs ) {
		char buf[512];
		if ( m_s != NULL ) {
			strcpy( buf, m_s );
			strcat( buf, rhs.m_s );
		}
		else
			strcpy( buf, rhs.m_s );
		return new GsString( buf );
	}
	GsString operator+( const char *rhs ) {
		char buf[512];
		if ( m_s != NULL ) {
			strcpy( buf, m_s );
			strcat( buf, rhs );
		}
		else
			strcpy( buf, rhs );
		return new GsString( buf );
	}
	GsString &operator+=( const char *rhs ) {
		char buf[512];
		strcpy( buf, m_s );
		strcat( buf, rhs );
		free( m_s );
		m_s = strdup( buf );
		return *this;
	}
	char *c_str() const {
		return m_s;
	}
};

#endif

//
//  Annoying Sun people seem to think that a T** will autocast to a void**...
//

#if defined( USING_JVM_12 )
#define JNI_CREATEJAVAVM( jvm, env, vm_args ) JNI_CreateJavaVM( jvm, (void**) env, vm_args )
#else
#define JNI_CREATEJAVAVM( jvm, env, vm_args ) JNI_CreateJavaVM( jvm, env, vm_args )
#endif

#endif 
