#include <android/log.h>

#define  LOG_TAG    "SoltarioIta"

void TraceInServiceINFO(char* myBuff) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", myBuff);
}

void TraceInServiceDEBUG(char* myBuff) {
    __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "%s", myBuff);
}
