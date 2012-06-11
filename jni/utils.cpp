/**
 * @file utils.cpp
 *
 * @brief Hilfsfunktionen.
 *
 * @author Alexander Bertram
 *
 * @date 17.06.2011
 */

#include "utils.h"

#include <math.h>
#include <stdlib.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

/**
 * Gibt eine 4x4 Matrix im Log aus.
 *
 * @param matrix Zeiger auf das erste Element der auszugebenden Matrix.
 */
void printMatrix(const float* matrix) {
    for (int i = 0; i < 4; i++, matrix += 4) {
        LOG("%7.3f %7.3f %7.3f %7.3f", matrix[0], matrix[1], matrix[2], matrix[3]);
    }
}

/**
 * Prüft, ob ein GL-Fehler nach einer Operation aufgetreten ist und gibt diesen im Log aus.
 *
 * @param operation Benutzerdefinierter Name der Operation.
 */
void checkGlError(const char* operation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("after %s() glError (0x%x)", operation, error);
    }
}

/**
 * Gibt die kleinere Zahl zurück.
 *
 * @param a 1. Zahl
 * @param b 2. Zahl
 * @return Die kleinere der beiden Zahlen.
 */
int min(int a, int b) {
    return a <= b ? a : b;
}

/**
 * Gibt die größere Zahl zurück.
 *
 * @param a 1. Zahl
 * @param b 2. Zahl
 * @return Die größere der beiden Zahlen.
 */
int max(int a, int b) {
    return a >= b ? a : b;
}
