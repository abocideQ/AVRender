#ifndef AVRENDER_COMMON_H
#define AVRENDER_COMMON_H

/**
 * log printf
 */
#ifdef __WIN32__

#include <stdio.h>

#define LOG_TAG "WIN32_LOG"

#define LOGE(...) ((void)printf(LOG_TAG, __VA_ARGS__))
#endif

#ifdef __ANDROID__

#include <android/log.h>

#define LOG_TAG "ANDROID_LOG"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

/**
 * include
 */
#include <string>

/**
 * define
 */
#define GET_CHAR(x) #x

#endif //AVRENDER_COMMON_H
