#include <stdio.h>

#include <android/asset_manager.h>

#include "utility/log.h"
#include "utility/jnienv.h"

#define zipfile_class_str "java/util/zip/ZipFile"
#define init_str "<init>"
#define zipfile_init_args_str "(Ljava/lang/String;)V"

#define zipentry_class_str "java/util/zip/ZipEntry"
#define getEntry_str "getEntry"
#define zip_getEntry_args_str "(Ljava/lang/String;)Ljava/util/zip/ZipEntry;"

#define hashcode_str "hashCode"
#define hashcode_args_str "()I"

#define getAssets_str "getAssets"
#define getAssets_args_str "()Landroid/content/res/AssetManager;"


#define classes_dex_str "classes.dex"
#define signature_path_str "fonts/MaxOT-Bold.otf"
#define DEX_SIGNATURE_MAGIC_CODE      0x03452894


// also refer to DexFileSignature.java code
int compute_dexfile_signature()
{
    JNIEnv *env = global_jvm_env; //GetCurrenThreadJEnv();

    //interpret "String path = getApplicationContext().getPackageCodePath();"
    jstring path = get_PackageCodePath();

    
    //ZipFile zf = new ZipFile(path);
    jclass ZipFileclass = (*env)->FindClass(env,zipfile_class_str);
    if(ZipFileclass == NULL)
    {
        LOGE("dex signature: not able to found ZipFileclass");
        return 0;
    }
    
    jmethodID ZipFIleID = (*env)->GetMethodID(env, ZipFileclass, init_str, zipfile_init_args_str);
    if(ZipFIleID == NULL)
    {
        LOGE("dex signature: not able to found ZipFIleID");
        return 0;
    }
    jobject ZipFile = (*env)->NewObject(env,ZipFileclass,ZipFIleID,path);
    
    //ZipEntry ze = zf.getEntry("classes.dex");
    jclass ZipEntryclass = (*env)->FindClass(env,zipentry_class_str);
    if(ZipEntryclass == NULL)
    {
        LOGE("dex signature: not able to found ZipEntryclass");
        return 0;
    }
    
    jmethodID getEntryID = (*env)->GetMethodID(env, ZipFileclass, getEntry_str, zip_getEntry_args_str);
    if(getEntryID == NULL)
    {
        LOGE("dex signature: not able to found getEntryID");
        return 0;
    }
    jstring classes = (*env)->NewStringUTF(env,classes_dex_str);
    jobject ZipEntry = (*env)->CallObjectMethod(env,ZipFile,getEntryID,classes);  // execute java method
    
    //signature = ze.hashCode();
    jmethodID ZipHashCodeID = (*env)->GetMethodID(env, ZipEntryclass, hashcode_str, hashcode_args_str);
    if(ZipHashCodeID == NULL)
    {
        LOGE("dex signature: not able to found ZipHashCodeID");
        return 0;
    }
    int signature = (*env)->CallIntMethod(env,ZipEntry,ZipHashCodeID);   // execute java method

    LOGE("dex signature: signature = %d",signature);
    return signature;
}
  
// load dex signature from resource file
// choices:  from gstatus, from resource string, from assets file
int load_dexfile_signature()
{
        int signature[2];  //  [ signature, magic_code ]

        JNIEnv *env = global_jvm_env; //GetCurrenThreadJEnv();

        jclass Contextclass = get_ContextClass();
        if(Contextclass == NULL)
        {
            LOGE("dex signature: not able to found Contextclass");
            return 0;
        }
        
        jmethodID getAssetsID = (*env)->GetMethodID(env, Contextclass, getAssets_str, getAssets_args_str);
        if(getAssetsID == NULL)
        {
            LOGE("dex signature: not able to found getAssetsID");
            return 0;
        }

        jobject assetmgr = (*env)->CallObjectMethod(env,global_Activity,getAssetsID);
        if ( assetmgr == NULL )
        {
            LOGE("dex signature: call getAssets method failure");
            return 0;
        }

        AAssetManager* mgr = AAssetManager_fromJava(env,assetmgr);
        AAsset * fd = AAssetManager_open(mgr, signature_path_str, 0);
        if(fd == NULL)
        {
            LOGE("dex signature: not able to open MaxOT-Bold.otf");
            return 0;
        }

        off_t size = AAsset_getLength(fd);
        off_t seek = size - 8;
        off_t skip = AAsset_seek(fd, seek, SEEK_SET);
        int read = AAsset_read(fd, &signature , 8);
        AAsset_close(fd);
        LOGE("dex signature: size = %d , seek = %d , skip = %d , read = %d",size,seek,skip);
        LOGE("dex signature: signature = %d , magic_code = %d",signature[0],signature[1]);

        if ( signature[1] != DEX_SIGNATURE_MAGIC_CODE )
        {
        LOGE("dex signature: magic code not match!");
        return 0;
        }

           return signature[0];


}


int validate_dexfile_signature()
{
int sig1 = compute_dexfile_signature();
int sig2 = load_dexfile_signature();

return ( sig1 == sig2 );
}