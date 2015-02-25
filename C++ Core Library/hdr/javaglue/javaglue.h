/****************************************************************
 **
 **  javaglue/javaglue.h
 **
 **  Copyright 1992-1999 - Goldman, Sachs & Co. - New York
 **
 **  $Header: /home/cvs/src/javaglue/src/javaglue/javaglue.h,v 1.62 2012/02/24 18:25:55 e19351 Exp $
 **
 ****************************************************************/


#if !defined( IN_SECJAVA_H )
#define IN_SECJAVA_H

#include <javaglue/base.h>
#include <secexpr.h>

#if defined( USE_GSBASE )

// FIX - 5/12/00
// temporarily remove this, until we enable GsDtGenericDtValue<> in gsdt, and 
// figure out how to do gsdt-dtvalue conversion
// #include <gsdt/GsDtGenericDtValue.h>

CC_USING_NAMESPACE( Gs );

#endif

// #define DEBUG_JAVA
// #define DEBUG_EVENTS
/*
  #define DEBUGJ(x)                                                                   \
  {                                                                                   \
  void *PrintFunction = SlangContextGet( "Print Function" ),                      \
  *PrintHandle   = SlangContextGet( "Print Handle" );                        \
  if ( PrintFunction && PrintHandle )                                             \
  {                                                                               \
  (*(void(*)(void*,const char*))PrintFunction)( PrintHandle, "DEBUGJ: " );    \
  (*(void(*)(void*,const char*))PrintFunction)( PrintHandle, x );             \
  (*(void(*)(void*,const char*))PrintFunction)( PrintHandle, "\n" );          \
  }                                                                               \
  }
*/

//#define DEBUGJ(x) { fprintf( stderr, "DEBUGJ: %s\n", x ); fflush( stderr ); }


#define DEBUGJ(x)   do { } while (0)


class JavaVMI;
class JavaClass;
class JavaObject;

//
// Stolen from Rob Gordon's Java JNI book.
//

class EXPORT_CLASS_JAVAGLUE JClass {
public:
    typedef enum { 
        Sig_Byte       =0, 
        Sig_Char       =1,
        Sig_Double     =2,
        Sig_Float      =3,
        Sig_Int        =4,
        Sig_Long       =5,
        Sig_Short      =6,
        Sig_Void       =7,
        Sig_Boolean    =8,
        Sig_Class      =9,
        Sig_Array      =10,
        Sig_Method     =11,
        Sig_Array_Str  =12,
        Sig_DTValue    =13,
        Sig_String     =14,
        Sig_Interface  =15,
        Sig_Array_Obj  =16,
        Sig_Array_Int  =17,  // **NativeArray**
        Sig_Unsupported=18
    } SigType;
    static jobjectArray getMethods(      JNIEnv*env, jclass cls);
    static jobjectArray getConstructors( JNIEnv*env, jclass cls);
    static jobjectArray getFields(       JNIEnv*env, jclass cls);
    static GsString     toString(        JNIEnv*env, jclass cls);
    static GsString     name(            JNIEnv*env, jclass cls);
    static char*        typeToSig( JClass::SigType sigType, GsString tName );
	static const char*  typeToSigName(   JClass::SigType );
    static SigType      sigType(    GsString tName );
};

class EXPORT_CLASS_JAVAGLUE Method {
public:
    static jobjectArray getParameterTypes(     JNIEnv *env, jobject method );
    static char        *getName(               JNIEnv *env, jobject method );
    static int          getModifiers(          JNIEnv *env, jobject method );
    static jclass       getReturnType(         JNIEnv *env, jobject method );
	
    static void         toStructure( DT_VALUE *r, JNIEnv *env, jobject method );
    static void         toStructureAsConstructor( DT_VALUE *r, JNIEnv *env, jobject method );
};

class EXPORT_CLASS_JAVAGLUE Field {
public:
    static char        *getName(      JNIEnv *env, jobject field );
    static jclass       getType(      JNIEnv *env, jobject field );
    static int          getModifiers( JNIEnv *env, jobject field );
	
    static void         toStructure( DT_VALUE *r, JNIEnv *env, jobject method );
};

class EXPORT_CLASS_JAVAGLUE Modifier {
public:
    static jboolean isStatic( JNIEnv *env, int mods);
};

GsString BuildSignature( JNIEnv       *env,
                         jobjectArray params,
                         jint         nParams,
                         jclass       retType );

//
//  JavaVMI
//     An interface to the Java Virtual Machine.
//

class EXPORT_CLASS_JAVAGLUE JavaVMI {
    JNIEnv *env;
    JNIEnv *secdbEnv;
    JavaVM *jvm;
public:
    JavaVMI();
    JavaVMI( JNIEnv *env );
    ~JavaVMI();
    JavaClass *getClass( const char* name );
    int isValid() { return jvm != NULL ; }
    void restoreEnv() { env = secdbEnv; }
    void setEnv( JNIEnv *e ) { env = e; }
    JNIEnv *getEnv() { return env!=NULL?env:secdbEnv; }
};

EXPORT_C_JAVAGLUE JavaVMI   *TheJVM;

GsString BuildSignature( JNIEnv       *env,
                         jobjectArray params,
                         jint         nParams );

// 
// CheckException checks and clears any exception
// in the JVM.  
// returns NULL or a caller-owned string.
//

char *CheckException( JNIEnv *env );

class EXPORT_CLASS_JAVAGLUE JavaMethodSignature {
    friend class JavaMethodInfo;
    GsString m_name;
#if defined( USE_GSBASE )
    friend struct JavaMethodSignatureToIntFunctor;
#else
    friend HASH_BUCKET HashSigHash( HASH_KEY key );
#endif
    int  m_numArgs;
    DT_DATA_TYPE_ID *m_argTypes;
    int  m_isStatic;
public:
    JavaMethodSignature( const char *name, DT_VALUE *values, int isStatic );
    ~JavaMethodSignature();
    int operator==( const JavaMethodSignature &rhs);
};


class EXPORT_CLASS_JAVAGLUE JavaMethodInfo {
    friend class JavaClass;
	
    jmethodID m_mid;
    JavaMethodSignature *m_sig;
    JClass::SigType m_sigType;
    JClass::SigType *m_argSigTypes;
public:
    JavaMethodInfo( jmethodID mid, JClass::SigType sigType, JClass::SigType *argSigTypes )
	{
		m_mid = mid;
		m_sigType = sigType;
		m_argSigTypes = argSigTypes;
	}
    ~JavaMethodInfo()
	{
		delete []m_argSigTypes;
	}
    JClass::SigType sigType() 
	{
		return m_sigType;
	}
    jmethodID mid()
	{
		return m_mid;
	}
    jvalue *buildArgs( JNIEnv *env, DT_VALUE *args );
    void freeArgs( JNIEnv *env, jvalue *args );
};

#if defined( USE_GSBASE )
struct JavaMethodSignatureToIntFunctor
{
    inline int operator()( const JavaMethodSignature* Sig ) const
    {   
        GsHashFromString sh;
		
        int Result = sh( Sig->m_name );
        Result ^= Sig->m_isStatic;
        Result ^= Sig->m_numArgs<<16;
        for( int i=0; i<Sig->m_numArgs; i++ )
            Result ^= Sig->m_argTypes[i] << i;
        return Result;
    };
};

struct JavaMethodSignatureComparator {
    int operator()( JavaMethodSignature*a, JavaMethodSignature*b )
	{	
		return *a == *b;
	}
};

#else
typedef HASH* SigToMethodHash;
#endif

//
// The interface for calling functions in the JVM...
//

class EXPORT_CLASS_JAVAGLUE JavaCall {
	
protected:
	
    static bool convertJObjectToDTValue( JNIEnv *env, DT_VALUE &result, jobject src );
	
};

//
// JavaClass
//    An interface to a specific Java class.
//
//    call() is the main function:  it converts a set of DT_VALUES to the appropriate Java
//    data types, calls the Java function, and converts the result type back into a DT_VALUE.    
//
//    JavaClass instances stick around while SecDB has a reference.  
//    instances are ref counted and shared.
//    Each JavaClass maintains a hash of methods that have been called on it.
//

class EXPORT_CLASS_JAVAGLUE JavaClass : public JavaCall {
    friend class JavaObject;
    jclass  m_cls;
    char    *m_name;
    int     m_refCnt;
	
    SigToMethodHash MethodHash;
	
    JavaMethodInfo *newConstructorObject( DT_VALUE *Params );
    JavaMethodInfo *newMethodObject( const char *methodName, DT_VALUE *Params, int isStatic);
    JavaMethodInfo *getMethodObject( const char *methodName, DT_VALUE *Params, int isStatic);
	
    int scoreSignature( 
        jobject method, 
        jobjectArray mParams, 
        int mParamsCount, 
        DT_VALUE *Params, 
        int isStatic,
        JClass::SigType **argSigTypes
	);
	
    int scoreSignature( 
        jobjectArray mParams, 
        int mParamsCount, 
        DT_VALUE *Params, 
        JClass::SigType **argSigTypes
	);
	
    GsStatus callStaticMethod( JClass::SigType, jmethodID mid, jvalue *argVals, DT_VALUE &ret );
    GsStatus getStaticField( JClass::SigType sigType, jfieldID fid, DT_VALUE &result );
	
    JavaObject *newProxyObject( DT_VALUE *params );
public:
    JavaClass( JNIEnv *env, const char *name );
    JavaClass( JNIEnv *env, const char *name, jclass cls );
    ~JavaClass();
    int valid() { return NULL != m_cls; }
    const char *name() const { return m_name ; }
    GsStatus call( const char *method, DT_VALUE &result, DT_VALUE *Params );
    GsStatus callProxy( const char *method, DT_VALUE &result, DT_VALUE *Params );
    GsStatus getField( const char *name, DT_VALUE &result );
    GsStatus toStructure( DT_VALUE *r );
    JavaObject *newObject( DT_VALUE *params );
    void incRefCount() { m_refCnt++; }
    void decRefCount();
    static JavaClass *Find( const char *clsName );
    const char *name() { return m_name ; }
    jclass cls() { return m_cls; }
};


//
// JavaObject
//    An interface to a specific Java object.
//
//    call() is the main function:  it converts a set of DT_VALUES to the appropriate Java
//    data types, calls the Java function, and converts the result type back into a DT_VALUE.    
//

class EXPORT_CLASS_JAVAGLUE JavaObject : public JavaCall {
    jobject m_obj;
    JavaClass *m_cls;
public:
    JavaObject( JNIEnv *env, jobject obj, JavaClass *cls )
	{
		m_obj = env->NewGlobalRef( obj );
		m_cls = cls;
		if ( m_cls != NULL )
			m_cls->incRefCount();
	}

    JavaObject( JNIEnv *env, jobject obj )
	{
		if ( obj == NULL )
		{
			m_cls = NULL;
			m_obj = NULL;
			return;
		}

		m_obj = env->NewGlobalRef( obj );

		if ( m_obj == NULL )
			fprintf( stderr, "Panic: bad global ref!\n" );

		jclass cls = env->GetObjectClass( m_obj );
		GsString name = JClass::name( env, cls );
		const char *className = name.c_str();
		m_cls = JavaClass::Find( className );
		if ( m_cls == NULL )
			m_cls = new JavaClass( env, className, cls  );
		else
			m_cls->incRefCount();
		env->DeleteLocalRef( cls );
	}
    JavaObject( const JavaObject &rhs )
	{
		m_obj = TheJVM->getEnv()->NewGlobalRef( rhs.m_obj );
		m_cls = rhs.m_cls;
		if ( m_cls != NULL )
			m_cls->incRefCount();
	}
    ~JavaObject()
	{
#if defined( DEBUG_JAVA )
		char buf[128];
		sprintf( buf, "~JavaObject (%s) %x free in env %x", 
				 m_cls->name(),
				 m_obj, 
				 TheJVM->getEnv());
		DEBUGJ( buf );
#endif
		TheJVM->getEnv()->DeleteGlobalRef( m_obj );
		DEBUGJ( "freed ok" );
		if ( m_cls != NULL )
			m_cls->decRefCount();
	}
	
// We only support GsDt, if USE_GSBASE.
// GsDt does not work, if we redefine GsString
#if defined( USE_GSBASE )
    // GsDt support
    GsString toString() const { return typeName(); }
    inline static const char* typeName() { return "JavaObject"; }
	
// FIX - 5/12/00
// temporarily remove this, until we enable GsDtGenericDtValue<> in gsdt, and 
// figure out how to do gsdt-dtvalue conversion
//typedef GsDtGenericDtValue<JavaObject> GsDtType;
	
    // GsDtGenericDtValue support
    DT_DATA_TYPE getDtValueType() const;
#endif
	
    GsStatus call( const char *method, DT_VALUE &result, DT_VALUE *Params );
    GsStatus callMethod( JClass::SigType sigType, jmethodID mid, jvalue *argVals, DT_VALUE &result );
	GsStatus callProxy( const char *methodName, DT_VALUE   &result, DT_VALUE   *Params );
	
    GsStatus getField( const char *name, DT_VALUE &result );
	GsStatus getFieldValue( jobject field, DT_VALUE *result );
	int getFieldCount();
	int getNextField( DT_VALUE *r, DT_VALUE *b );
	int getLastField( DT_VALUE *r, DT_VALUE *b );
	
    const char *className() {return  m_cls->m_name; }
    uintptr_t objID() { return (uintptr_t) m_obj; }
    jobject objectRef() { return m_obj; }

	char *toString();
};


EXPORT_C_JAVAGLUE int DtFuncJavaVM(  int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b);
EXPORT_C_JAVAGLUE int DtFuncJavaCls( int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b);
EXPORT_C_JAVAGLUE int DtFuncJavaObj( int Msg, DT_VALUE *r, DT_VALUE *a, DT_VALUE *b);

//
// DtValueAccessorInfo
//     holds static data about the DataType class wrapper.
//     note:  don't pass this amongst threads or hold on to it
//     between Slang invocations!
//

class EXPORT_CLASS_JAVAGLUE DtValueAccessorInfo {
    friend class DtValueAccessor;
	JNIEnv *myEnv;
public:
    DtValueAccessorInfo( JNIEnv *env_ );
    ~DtValueAccessorInfo();
    DT_VALUE *getDTValue( jobject obj );
    jobject  newDTValue();
};

//
// function to initialize datatypes, etc:
//

void JavaInit();
void JavaInit2( JNIEnv *env );


char *GetThreadName( JNIEnv *env );

class EXPORT_CLASS_JAVAGLUE JavaSecDbLock {
    JNIEnv *save;
public:
    JavaSecDbLock( JNIEnv *env, int id ) 
    { 
#if defined( DEBUG_JAVA )
		char buf[128];
		sprintf( buf, "Lock being acquired" );
		//DEBUGJ( buf );
		sprintf( buf, "Lock %x env=%x %d %s", this, env, id, GetThreadName( env ) );
		//DEBUGJ( buf );
#endif
		w_GetSecDbAccess( id );
#if defined( DEBUG_JAVA )
		sprintf( buf, "Lock acquired (env=%x myenv=%x)", this, env, TheJVM->getEnv());
		//DEBUGJ( buf );
#endif
		save = TheJVM->getEnv();
		TheJVM->setEnv( env );
#if defined( DEBUG_JAVA )
		sprintf( buf, "Lock %x env now=%x access acquired (thread %x)", this, env, w_GetCurrentThreadID() );
		DEBUGJ( buf );
#endif
    }
	
    ~JavaSecDbLock() 
    { 
#if defined( DEBUG_JAVA )
		char buf[128];
		sprintf( buf, "~Lock %x env=%x (thread %d), setting to NULL", this, TheJVM->getEnv(), w_GetCurrentThreadID() );
		DEBUGJ( buf );
		sprintf( buf, "\tsave was %x, env now %x", save, TheJVM->getEnv());
		DEBUGJ( buf );
#endif
		TheJVM->restoreEnv();
		TheJVM->setEnv( NULL );
		w_ReleaseSecDbAccess();
    }
};

class EXPORT_CLASS_JAVAGLUE JavaSecDbUnlock {
    JNIEnv *save;
    int threadID;
public:
    JavaSecDbUnlock( JNIEnv *env=NULL ) 
    { 
#if defined( DEBUG_JAVA )
		char buf[128];
		sprintf( buf, "Unlock %x %x (thread %d)", this, TheJVM->getEnv(), w_GetCurrentThreadID());
		DEBUGJ( buf );
		//sprintf( buf, "\tUnlock %s", this, GetThreadName( env ) );
		//DEBUGJ( buf );
#endif
        if ( env != NULL && TheJVM->getEnv() != env )
            (*(char*)1) = 1;
        save = TheJVM->getEnv();
		TheJVM->restoreEnv();
		TheJVM->setEnv( NULL );
        threadID = w_GetCurrentThreadID();
        w_ReleaseSecDbAccess();
    }
    ~JavaSecDbUnlock() 
    { 
#if defined( DEBUG_JAVA )
        char buf[128];
        sprintf( buf, "~Unlock %x %x (thread %d)", this, save, threadID );
        //DEBUGJ( buf );
        sprintf( buf, "~Unlock %x, curr env %x, will set to %x (thread %d)", this, TheJVM->getEnv(), save, threadID );
        //DEBUGJ( buf );
#endif
        w_GetSecDbAccess( threadID );
        TheJVM->setEnv( save );
#if defined( DEBUG_JAVA )
        sprintf( buf, "~Unlock %x save=%x acquired access", this, save);
        //DEBUGJ( buf );
        sprintf( buf, "~Unlock %x save=%x acquired access (%s)", this, save, GetThreadName( save )  );
        DEBUGJ( buf );
#endif
    }
};

int JavaLockInit();
void DTValueDeletionListInit();

//
// JavaVMIDt 
//    The version of JavaVMI that the user sees
//

class JavaVMIDt {
    GsString packagePrefix; // e.g. com.gs
public:
    JavaVMIDt( GsString prefix );
    ~JavaVMIDt();
    JavaClass *getClass( const char* name );
    const char *getPrefix() 
	{
		return( packagePrefix.c_str() );
	}
};

jclass GetProxyClass();

extern char *SecDbClassPath;
extern char *JavaProxyClass;
extern int   UseProxyClasses;

extern jclass JClassDTValue;
extern jclass JClassSlangInterface;
extern jclass JClassString;
extern jclass JClassObject;
extern jclass JClassClass;
extern jclass JClassIllegalArgumentException;
extern jclass JClassException;
extern jclass JClassModifier;
extern jclass JClassMethod;
extern jclass JClassField;
extern jclass JClassFieldReader;

extern jmethodID JMethodGetStackTrace;
extern jmethodID JMethodThreadName;
extern jmethodID JMethodIsStatic;
extern jmethodID JMethodInit;
extern jmethodID JMethodInit0;
extern jmethodID JMethodGetModifiers;
extern jmethodID JMethodGetMethods;
extern jmethodID JMethodGetReturnType;
extern jmethodID JMethodGetName;
extern jmethodID JMethodGetConstructors;
extern jmethodID JMethodGetField;
extern jmethodID JMethodGetFields;
extern jmethodID JMethodToString;        
extern jmethodID JMethodEquals;
extern jmethodID JMethodHashCode;
extern jmethodID JMethodGetParameterTypes;
extern jmethodID JMethodFieldCount;
extern jmethodID JMethodNextField;
extern jmethodID JMethodLastField;
extern jmethodID JMethodFEquals;
extern jmethodID JMethodFieldName;
extern jmethodID JMethodClassToString;

extern jmethodID JMethodNewObject;
extern jmethodID JMethodCallS;
extern jmethodID JMethodCall;

extern jfieldID  JFieldIDDtValuePtr;

#endif

