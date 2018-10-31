/*
 * jniwrapper.h
 *
 *  Created on: 2015-6-11
 *      Author: fanghui
 */

#ifndef __JNI_WRAPPER_H__
#define __JNI_WRAPPER_H__


#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif


JNIEXPORT jstring JNICALL   Java_com_nlog2n_mukey_MuThreats_printAppStatus(JNIEnv *env, jobject thiz);

JNIEXPORT jint JNICALL      Java_com_nlog2n_mukey_MuServices_GenOTP(JNIEnv *, jobject obj);

JNIEXPORT void JNICALL      Java_com_nlog2n_mukey_MuServices_onCreateNative(JNIEnv *env, jobject thiz, jobject activityObject);

JNIEXPORT void JNICALL      Java_com_nlog2n_mukey_MuServices_onDestroy(JNIEnv *, jobject obj);

JNIEXPORT jint JNICALL      Java_com_nlog2n_mukey_DexOptCheck_runDexOpt(JNIEnv* env, jobject thiz, jstring apkPath, jstring dexOptPath);


#ifdef __cplusplus
}
#endif


#endif /* __JNI_WRAPPER_H__ */
