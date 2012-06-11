/**
 * @file timeUtilities.cpp
 *
 * @brief Beinhaltet Zeit-Hilfsfunktionen.
 *
 * @author Alexander Bertram
 *
 * @date 05.06.2011
 */

#include <sys/time.h>

/**
 * Berechnet die aktuelle Zeit im Millisekunden.
 *
 * @return Aktuelle Zeit in Millisekunden.
 */
unsigned long getCurrentTimeInMs() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long s = tv.tv_sec * 1000;
    unsigned long us = tv.tv_usec / 1000;
    return s + us;
}
