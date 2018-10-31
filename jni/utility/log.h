#ifndef __NLOG2N_LOG_H__
#define __NLOG2N_LOG_H__

#include <android/log.h>

#define  ENABLE_DEBUG_PRINT        1


#define  LOG_TAG    "[mukey]"
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


#define WRITE_LOG(s, ...) do { \
FILE *f = fopen("/data/local/tmp/mukey_log.txt", "a+"); \
  fprintf(f, s, __VA_ARGS__); \
  fflush(f); \
  fclose(f); \
} while (0)

#endif
