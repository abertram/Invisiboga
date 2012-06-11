/**
 * @file invisiboga.h
 *
 * @brief Schnittstelle zu invisiboga.cpp.
 *
 * @author Alexander Bertram
 *
 * @date 17.09.2011
 */

#ifndef INVISIBOGA_H
#define INVISIBOGA_H

// folgender Code ist C-Code
#ifdef __cplusplus
extern "C" {
#endif

void configureVideoBackground();
void handleTouchEvent();

#ifdef __cplusplus
}
#endif

#endif
