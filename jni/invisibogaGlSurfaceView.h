/**
 * @file invisibogaGlSurfaceView.h
 *
 * @brief Schnittstelle zu invisibogaGlSurfaceView.cpp.
 *
 * @author Alexander Bertram
 *
 * @date 19.06.2011
 */

#ifndef INVISIBOGA_GL_SURFACE_VIEW_H
#define INVISIBOGA_GL_SURFACE_VIEW_H

// folgender Code ist C-Code
#ifdef __cplusplus
extern "C" {
#endif

void showView(const char *);
void hideView(const char *);
void showToast(const char *, int);
void setCurrentPlayer(Player *);

#ifdef __cplusplus
}
#endif

#endif
