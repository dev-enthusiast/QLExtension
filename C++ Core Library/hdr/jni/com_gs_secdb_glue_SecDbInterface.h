/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_gs_secdb_glue_SecDbInterface */

#ifndef _Included_com_gs_secdb_glue_SecDbInterface
#define _Included_com_gs_secdb_glue_SecDbInterface
#ifdef __cplusplus
extern "C" {
#endif
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_FIRST
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_FIRST 1L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_LAST
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_LAST 2L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_EQUAL
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_EQUAL 3L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_LESS
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_LESS 4L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_LE
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_LE 5L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_GREATER
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_GREATER 6L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_GE
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_GE 7L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_NEXT
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_NEXT 8L
#undef com_gs_secdb_glue_SecDbInterface_SDB_GET_PREV
#define com_gs_secdb_glue_SecDbInterface_SDB_GET_PREV 9L
/*
 * Class:     com_gs_secdb_glue_SecDbInterface
 * Method:    lookupManySecTypes
 * Signature: (Ljava/lang/String;I[II)[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL Java_com_gs_secdb_glue_SecDbInterface_lookupManySecTypes
  (JNIEnv *, jclass, jstring, jint, jintArray, jint);

/*
 * Class:     com_gs_secdb_glue_SecDbInterface
 * Method:    getSecType
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_gs_secdb_glue_SecDbInterface_getSecType
  (JNIEnv *, jclass, jstring);

#ifdef __cplusplus
}
#endif
#endif
