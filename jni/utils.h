/**
 * @file utils.h
 *
 * @brief Schnittstelle zu utils.cpp und Makros zum Loggen.
 *
 * @author Alexander Bertram
 *
 * @date 17.06.2011
 */

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <android/log.h>

/**
 * Tag unter dem die Log-Einträge erscheinen.
 */
#define LOG_TAG "Invisiboga"
/**
 * Erstellt einen Log-Eintrag auf dem Debug-Level.
 */
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
/**
 * Erstellt einen Log-Eintrag auf dem Debug-Level.
 */
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
/**
 * Erstellt einen Log-Eintrag auf dem Error-Level.
 */
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
/**
 * Erstellt einen Log-Eintrag auf dem Info-Level.
 */
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void printMatrix(const float* matrix);
void checkGlError(const char* operation);
int min(int a, int b);
int max(int a, int b);

#endif
