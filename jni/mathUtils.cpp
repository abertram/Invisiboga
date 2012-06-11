/**
 * @file mathUtils.cpp
 *
 * @brief Beinhaltet mathematische Hilfsfunktionen. Die Funktionen wurden aus den QCAR-Beispielen
 *      übernommen.
 *
 * @author QCAR
 *
 * @date 17.06.2011
 */

#include <math.h>
#include <stdlib.h>

#include <QCAR/Renderer.h>
#include <QCAR/VideoBackgroundConfig.h>

/**
 * Berechnet die Differenz zweier 2D-Vektoren .
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Differenz-Vektor.
 */
QCAR::Vec2F vec2FSub(QCAR::Vec2F v1, QCAR::Vec2F v2) {
    QCAR::Vec2F r;
    r.data[0] = v1.data[0] - v2.data[0];
    r.data[1] = v1.data[1] - v2.data[1];
    return r;
}

/**
 * Berechnet die Summe zweier 2D-Vektoren.
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Summen-Vektor.
 */
QCAR::Vec2F vec2FAdd(QCAR::Vec2F v1, QCAR::Vec2F v2) {
    QCAR::Vec2F r;
    r.data[0] = v1.data[0] + v2.data[0];
    r.data[1] = v1.data[1] + v2.data[1];
    return r;
}

/**
 * Berechnet die Summe zweier 3D-Vektoren.
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Summen-Vektor.
 */
QCAR::Vec3F vec3FAdd(QCAR::Vec3F v1, QCAR::Vec3F v2) {
    QCAR::Vec3F r;
    r.data[0] = v1.data[0] + v2.data[0];
    r.data[1] = v1.data[1] + v2.data[1];
    r.data[2] = v1.data[2] + v2.data[2];
    return r;
}

/**
 * Berechnet die Differenz zweier 3D-Vektoren.
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Differenz-Vektor.
 */
QCAR::Vec3F vec3FSub(QCAR::Vec3F v1, QCAR::Vec3F v2) {
    QCAR::Vec3F r;
    r.data[0] = v1.data[0] - v2.data[0];
    r.data[1] = v1.data[1] - v2.data[1];
    r.data[2] = v1.data[2] - v2.data[2];
    return r;
}

/**
 * Skaliert einen 2D-Vektor um einen Faktor.
 *
 * @param v Vektor, der skaliert werden soll.
 * @param s Faktor, um den skaliert werden soll.
 * @return Skalierter Vektor.
 */
QCAR::Vec2F vec2FScale(QCAR::Vec2F v, float s) {
    QCAR::Vec2F r;
    r.data[0] = v.data[0] * s;
    r.data[1] = v.data[1] * s;
    return r;
}

/**
 * Skaliert einen 3D-Vektor um einen Faktor.
 *
 * @param v Vektor, der skaliert werden soll.
 * @param s Faktor, um den skaliert werden soll.
 * @return Skalierter Vektor.
 */
QCAR::Vec3F vec3FScale(QCAR::Vec3F v, float s) {
    QCAR::Vec3F r;
    r.data[0] = v.data[0] * s;
    r.data[1] = v.data[1] * s;
    r.data[2] = v.data[2] * s;
    return r;
}

/**
 * Dividiert einen Vektor durch einen Divisor.
 *
 * @param v Vektor, der dividiert werden soll.
 * @param s Divisor, durch durch den dividiert werden soll.
 * @return Dividierter Vektor.
 */
QCAR::Vec4F vec3FDiv(QCAR::Vec4F v, float s) {
    QCAR::Vec4F r;
    r.data[0] = v.data[0] / s;
    r.data[1] = v.data[1] / s;
    r.data[2] = v.data[2] / s;
    r.data[3] = v.data[3] / s;
    return r;
}

/**
 * Berechnet das Punkt-Produkt zweier 2D-Vektoren.
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Punkt-Produkt.
 */
float vec3FDot(QCAR::Vec3F v1, QCAR::Vec3F v2) {
    return v1.data[0] * v2.data[0] + v1.data[1] * v2.data[1] + v1.data[2] * v2.data[2];
}

/**
 * Normalisiert einen 3D-Vektor.
 *
 * @param v Vektor, der normalisiert werden soll.
 * @return Normalisierter Vektor.
 */
QCAR::Vec3F vec3FNormalize(QCAR::Vec3F v) {
    QCAR::Vec3F r;
    float length = sqrt(v.data[0] * v.data[0] + v.data[1] * v.data[1] + v.data[2] * v.data[2]);
    if (length != 0.0f) {
        length = 1.0f / length;
    }
    r.data[0] = v.data[0] * length;
    r.data[1] = v.data[1] * length;
    r.data[2] = v.data[2] * length;
    return r;
}

/**
 * Transformiert einen 4D-Vektor mit Hilfe einer Matrix.
 *
 * @param v Vektor, der transformiert werden soll.
 * @param m Transformationsmatrix.
 * @return Transformierter Vektor.
 */
QCAR::Vec4F vec4FTransform(QCAR::Vec4F& v, QCAR::Matrix44F& m) {
    QCAR::Vec4F r;
    r.data[0] = m.data[0] * v.data[0] + m.data[1] * v.data[1] + m.data[2] * v.data[2] + m.data[3]
            * v.data[3];
    r.data[1] = m.data[4] * v.data[0] + m.data[5] * v.data[1] + m.data[6] * v.data[2] + m.data[7]
            * v.data[3];
    r.data[2] = m.data[8] * v.data[0] + m.data[9] * v.data[1] + m.data[10] * v.data[2] + m.data[11]
            * v.data[3];
    r.data[3] = m.data[12] * v.data[0] + m.data[13] * v.data[1] + m.data[14] * v.data[2]
            + m.data[15] * v.data[3];
    return r;
}

/**
 * Erzeugt eine 4x4 große Einheitsmatrix.
 *
 * @return Die erzeugte Einheitsmatrix.
 */
QCAR::Matrix44F matrix44FIdentity() {
    QCAR::Matrix44F r;
    for (int i = 0; i < 16; i++) {
        r.data[i] = 0.0f;
    }
    r.data[0] = 1.0f;
    r.data[5] = 1.0f;
    r.data[10] = 1.0f;
    r.data[15] = 1.0f;
    return r;
}

/**
 * Berechnet die Determinante einer 4x4-Matrix.
 *
 * @param m Matrix, deren Determinante berechnet werden soll.
 * @return Determinante der Matrix.
 */
float matrix44FDeterminate(QCAR::Matrix44F& m) {
    return m.data[12] * m.data[9] * m.data[6] * m.data[3] - m.data[8] * m.data[13] * m.data[6]
            * m.data[3] - m.data[12] * m.data[5] * m.data[10] * m.data[3] + m.data[4] * m.data[13]
            * m.data[10] * m.data[3] + m.data[8] * m.data[5] * m.data[14] * m.data[3] - m.data[4]
            * m.data[9] * m.data[14] * m.data[3] - m.data[12] * m.data[9] * m.data[2] * m.data[7]
            + m.data[8] * m.data[13] * m.data[2] * m.data[7] + m.data[12] * m.data[1] * m.data[10]
            * m.data[7] - m.data[0] * m.data[13] * m.data[10] * m.data[7] - m.data[8] * m.data[1]
            * m.data[14] * m.data[7] + m.data[0] * m.data[9] * m.data[14] * m.data[7] + m.data[12]
            * m.data[5] * m.data[2] * m.data[11] - m.data[4] * m.data[13] * m.data[2] * m.data[11]
            - m.data[12] * m.data[1] * m.data[6] * m.data[11] + m.data[0] * m.data[13] * m.data[6]
            * m.data[11] + m.data[4] * m.data[1] * m.data[14] * m.data[11] - m.data[0] * m.data[5]
            * m.data[14] * m.data[11] - m.data[8] * m.data[5] * m.data[2] * m.data[15] + m.data[4]
            * m.data[9] * m.data[2] * m.data[15] + m.data[8] * m.data[1] * m.data[6] * m.data[15]
            - m.data[0] * m.data[9] * m.data[6] * m.data[15] - m.data[4] * m.data[1] * m.data[10]
            * m.data[15] + m.data[0] * m.data[5] * m.data[10] * m.data[15];
}

/**
 * Invertiert eine 4x4-Matrix.
 *
 * @param m Matrix, die invertiert werden soll.
 * @return Invertierte Matrix.
 */
QCAR::Matrix44F matrix44FInverse(QCAR::Matrix44F& m) {
    QCAR::Matrix44F r;
    float det = 1.0f / matrix44FDeterminate(m);
    r.data[0] = m.data[6] * m.data[11] * m.data[13] - m.data[7] * m.data[10] * m.data[13]
            + m.data[7] * m.data[9] * m.data[14] - m.data[5] * m.data[11] * m.data[14] - m.data[6]
            * m.data[9] * m.data[15] + m.data[5] * m.data[10] * m.data[15];
    r.data[4] = m.data[3] * m.data[10] * m.data[13] - m.data[2] * m.data[11] * m.data[13]
            - m.data[3] * m.data[9] * m.data[14] + m.data[1] * m.data[11] * m.data[14] + m.data[2]
            * m.data[9] * m.data[15] - m.data[1] * m.data[10] * m.data[15];
    r.data[8] = m.data[2] * m.data[7] * m.data[13] - m.data[3] * m.data[6] * m.data[13] + m.data[3]
            * m.data[5] * m.data[14] - m.data[1] * m.data[7] * m.data[14] - m.data[2] * m.data[5]
            * m.data[15] + m.data[1] * m.data[6] * m.data[15];
    r.data[12] = m.data[3] * m.data[6] * m.data[9] - m.data[2] * m.data[7] * m.data[9] - m.data[3]
            * m.data[5] * m.data[10] + m.data[1] * m.data[7] * m.data[10] + m.data[2] * m.data[5]
            * m.data[11] - m.data[1] * m.data[6] * m.data[11];
    r.data[1] = m.data[7] * m.data[10] * m.data[12] - m.data[6] * m.data[11] * m.data[12]
            - m.data[7] * m.data[8] * m.data[14] + m.data[4] * m.data[11] * m.data[14] + m.data[6]
            * m.data[8] * m.data[15] - m.data[4] * m.data[10] * m.data[15];
    r.data[5] = m.data[2] * m.data[11] * m.data[12] - m.data[3] * m.data[10] * m.data[12]
            + m.data[3] * m.data[8] * m.data[14] - m.data[0] * m.data[11] * m.data[14] - m.data[2]
            * m.data[8] * m.data[15] + m.data[0] * m.data[10] * m.data[15];
    r.data[9] = m.data[3] * m.data[6] * m.data[12] - m.data[2] * m.data[7] * m.data[12] - m.data[3]
            * m.data[4] * m.data[14] + m.data[0] * m.data[7] * m.data[14] + m.data[2] * m.data[4]
            * m.data[15] - m.data[0] * m.data[6] * m.data[15];
    r.data[13] = m.data[2] * m.data[7] * m.data[8] - m.data[3] * m.data[6] * m.data[8] + m.data[3]
            * m.data[4] * m.data[10] - m.data[0] * m.data[7] * m.data[10] - m.data[2] * m.data[4]
            * m.data[11] + m.data[0] * m.data[6] * m.data[11];
    r.data[2] = m.data[5] * m.data[11] * m.data[12] - m.data[7] * m.data[9] * m.data[12]
            + m.data[7] * m.data[8] * m.data[13] - m.data[4] * m.data[11] * m.data[13] - m.data[5]
            * m.data[8] * m.data[15] + m.data[4] * m.data[9] * m.data[15];
    r.data[6] = m.data[3] * m.data[9] * m.data[12] - m.data[1] * m.data[11] * m.data[12]
            - m.data[3] * m.data[8] * m.data[13] + m.data[0] * m.data[11] * m.data[13] + m.data[1]
            * m.data[8] * m.data[15] - m.data[0] * m.data[9] * m.data[15];
    r.data[10] = m.data[1] * m.data[7] * m.data[12] - m.data[3] * m.data[5] * m.data[12]
            + m.data[3] * m.data[4] * m.data[13] - m.data[0] * m.data[7] * m.data[13] - m.data[1]
            * m.data[4] * m.data[15] + m.data[0] * m.data[5] * m.data[15];
    r.data[14] = m.data[3] * m.data[5] * m.data[8] - m.data[1] * m.data[7] * m.data[8] - m.data[3]
            * m.data[4] * m.data[9] + m.data[0] * m.data[7] * m.data[9] + m.data[1] * m.data[4]
            * m.data[11] - m.data[0] * m.data[5] * m.data[11];
    r.data[3] = m.data[6] * m.data[9] * m.data[12] - m.data[5] * m.data[10] * m.data[12]
            - m.data[6] * m.data[8] * m.data[13] + m.data[4] * m.data[10] * m.data[13] + m.data[5]
            * m.data[8] * m.data[14] - m.data[4] * m.data[9] * m.data[14];
    r.data[7] = m.data[1] * m.data[10] * m.data[12] - m.data[2] * m.data[9] * m.data[12]
            + m.data[2] * m.data[8] * m.data[13] - m.data[0] * m.data[10] * m.data[13] - m.data[1]
            * m.data[8] * m.data[14] + m.data[0] * m.data[9] * m.data[14];
    r.data[11] = m.data[2] * m.data[5] * m.data[12] - m.data[1] * m.data[6] * m.data[12]
            - m.data[2] * m.data[4] * m.data[13] + m.data[0] * m.data[6] * m.data[13] + m.data[1]
            * m.data[4] * m.data[14] - m.data[0] * m.data[5] * m.data[14];
    r.data[15] = m.data[1] * m.data[6] * m.data[8] - m.data[2] * m.data[5] * m.data[8] + m.data[2]
            * m.data[4] * m.data[9] - m.data[0] * m.data[6] * m.data[9] - m.data[1] * m.data[4]
            * m.data[10] + m.data[0] * m.data[5] * m.data[10];
    for (int i = 0; i < 16; i++)
        r.data[i] *= det;
    return r;
}

/**
 * Berechnet die Distanz zwischen zwei 2D-Vektoren.
 *
 * @param v1 Vektor 1.
 * @param v2 Vektor 2.
 * @return Distanz zwischen den Vektoren.
 */
float vec2FDistance(QCAR::Vec2F v1, QCAR::Vec2F v2) {
    float dx = v1.data[0] - v2.data[0];
    float dy = v1.data[1] - v2.data[1];
    return sqrt(dx * dx + dy * dy);
}

/**
 * Berechnet, ob eine Linie eine Ebene schneidet.
 *
 * @param lineStart Koordinaten des Linien-Anfangs.
 * @param lineEnd Koordinaten des Linien-Endes.
 * @param pointOnPlane Ein Punkt der Ebene.
 * @param planeNormal Ebenen-Normale.
 * @param intersection Koordinaten des Schnitt-Punktes, wenn es einen gibt.
 * @return True, wenn die Linie die Ebene schneidet.
 */
bool linePlaneIntersection(QCAR::Vec3F lineStart, QCAR::Vec3F lineEnd, QCAR::Vec3F pointOnPlane,
        QCAR::Vec3F planeNormal, QCAR::Vec3F &intersection) {
    QCAR::Vec3F lineDir = vec3FSub(lineEnd, lineStart);
    lineDir = vec3FNormalize(lineDir);
    QCAR::Vec3F planeDir = vec3FSub(pointOnPlane, lineStart);
    float n = vec3FDot(planeNormal, planeDir);
    float d = vec3FDot(planeNormal, lineDir);
    if (fabs(d) < 0.00001) {
        // Line is parallel to plane
        return false;
    }
    float dist = n / d;
    QCAR::Vec3F offset = vec3FScale(lineDir, dist);
    intersection = vec3FAdd(lineStart, offset);
    return true;
}

/**
 * Projeziert ein Punkt auf eine Ebene.
 *
 * @param screenWidth Bildschirm-Breite.
 * @param screenHeight Bildschrim-Höhe.
 * @param point Punkt, der auf die Ebene projeziert werden soll.
 * @param inverseProjectionMatrix Inverse Projektionsmatrix.
 * @param modelViewMatrix Model-View-Matrix.
 * @param planeCenter Ebenen-Zentrum.
 * @param planeNormal Ebenen-Normale.
 * @param intersection Schnittpunkt der (virtuellen) Linie, die vom Auge des Betrachters durch den
 *      zu projezierenden Punkt geht, und der übergebenen Ebene.
 */
void projectScreenPointToPlane(float screenWidth, float screenHeight, QCAR::Vec2F point,
        QCAR::Matrix44F inverseProjectionMatrix, QCAR::Matrix44F modelViewMatrix,
        QCAR::Vec3F planeCenter, QCAR::Vec3F planeNormal, QCAR::Vec3F &intersection) {
    // Window Coordinates to Normalized Device Coordinates
    QCAR::VideoBackgroundConfig config =
            QCAR::Renderer::getInstance().getVideoBackgroundConfig();
    float halfScreenWidth = screenWidth / 2.0f;
    float halfScreenHeight = screenHeight / 2.0f;
    float halfViewportWidth = config.mSize.data[0] / 2.0f;
    float halfViewportHeight = config.mSize.data[1] / 2.0f;
    float x = (point.data[0] - halfScreenWidth) / halfViewportWidth;
    float y = (point.data[1] - halfScreenHeight) / halfViewportHeight * -1;
    QCAR::Vec4F ndcNear(x, y, -1, 1);
    QCAR::Vec4F ndcFar(x, y, 1, 1);
    // Normalized Device Coordinates to Eye Coordinates
    QCAR::Vec4F pointOnNearPlane = vec4FTransform(ndcNear, inverseProjectionMatrix);
    QCAR::Vec4F pointOnFarPlane = vec4FTransform(ndcFar, inverseProjectionMatrix);
    pointOnNearPlane = vec3FDiv(pointOnNearPlane, pointOnNearPlane.data[3]);
    pointOnFarPlane = vec3FDiv(pointOnFarPlane, pointOnFarPlane.data[3]);
    // Eye Coordinates to Object Coordinates
    QCAR::Matrix44F inverseModelViewMatrix = matrix44FInverse(modelViewMatrix);
    QCAR::Vec4F nearWorld = vec4FTransform(pointOnNearPlane, inverseModelViewMatrix);
    QCAR::Vec4F farWorld = vec4FTransform(pointOnFarPlane, inverseModelViewMatrix);
    QCAR::Vec3F lineStart = QCAR::Vec3F(nearWorld.data[0], nearWorld.data[1], nearWorld.data[2]);
    QCAR::Vec3F lineEnd = QCAR::Vec3F(farWorld.data[0], farWorld.data[1], farWorld.data[2]);
    linePlaneIntersection(lineStart, lineEnd, planeCenter, planeNormal, intersection);
}

/**
 * Multipliziert die übergebene Matrix mit einer Verschiebungsmatrix.
 *
 * @param x Verschiebung in die x-Richtung.
 * @param y Verschiebung in die y-Richtung.
 * @param z Verschiebung in die z-Richtung.
 * @param matrix Die mit der Verschiebungsmatrix zu multiplizierende Matrix.
 */
void translatePoseMatrix(float x, float y, float z, float* matrix) {
    // Sanity check
    if (!matrix)
        return;
    // matrix * translate_matrix
    matrix[12] += (matrix[0] * x + matrix[4] * y + matrix[8] * z);
    matrix[13] += (matrix[1] * x + matrix[5] * y + matrix[9] * z);
    matrix[14] += (matrix[2] * x + matrix[6] * y + matrix[10] * z);
    matrix[15] += (matrix[3] * x + matrix[7] * y + matrix[11] * z);
}

/**
 * Multipliziert die übergeben Matrix mit einer Skalierungsmatrix.
 *
 * @param x Skalierungsfaktor in die x-Richtung.
 * @param y Skalierungsfaktor in die y-Richtung.
 * @param z Skalierungsfaktor in die z-Richtung.
 * @param matrix Die mit der Skalierungsmatrix zu multiplizierende Matrix.
 */
void scalePoseMatrix(float x, float y, float z, float* matrix) {
    // Sanity check
    if (!matrix)
        return;
    // matrix * scale_matrix
    matrix[0] *= x;
    matrix[1] *= x;
    matrix[2] *= x;
    matrix[3] *= x;
    matrix[4] *= y;
    matrix[5] *= y;
    matrix[6] *= y;
    matrix[7] *= y;
    matrix[8] *= z;
    matrix[9] *= z;
    matrix[10] *= z;
    matrix[11] *= z;
}

/**
 * Multipliziert zwei Matrizen.
 *
 * @param matrixA 1. Matrix.
 * @param matrixB 2. Matrix.
 * @param matrixC Matrix, in der das Ergebnis gespeichert wird.
 */
void multiplyMatrix(float *matrixA, float *matrixB, float *matrixC) {
    int i, j, k;
    float aTmp[16];
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            aTmp[j * 4 + i] = 0.0;
            for (k = 0; k < 4; k++)
                aTmp[j * 4 + i] += matrixA[k * 4 + i] * matrixB[j * 4 + k];
        }
    }
    for (i = 0; i < 16; i++)
        matrixC[i] = aTmp[i];
}

/**
 * Berechnet eine Rotationsmatrix um einen Vektor.
 *
 * @param angle Rotationswinkel.
 * @param x x-Komponente des Vektors.
 * @param y y-Komponente des Vektors.
 * @param z z-Komponente des Vektors.
 * @param matrix Matrix, in der die Rotationsmatrix gespeichert werden soll.
 */
void setRotationMatrix(float angle, float x, float y, float z, float* matrix) {
    double radians, c, s, c1, u[3], length;
    int i, j;
    radians = (angle * M_PI) / 180.0;
    c = cos(radians);
    s = sin(radians);
    c1 = 1.0 - cos(radians);
    length = sqrt(x * x + y * y + z * z);
    u[0] = x / length;
    u[1] = y / length;
    u[2] = z / length;
    for (i = 0; i < 16; i++)
        matrix[i] = 0.0;
    matrix[15] = 1.0;
    for (i = 0; i < 3; i++) {
        matrix[i * 4 + (i + 1) % 3] = u[(i + 2) % 3] * s;
        matrix[i * 4 + (i + 2) % 3] = -u[(i + 1) % 3] * s;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++)
            matrix[i * 4 + j] += c1 * u[i] * u[j] + (i == j ? c : 0.0);
    }
}

/**
 * Multipliziert die übergebene Matrix mit einer Rotationsmatrix.
 *
 * @param angle Rotationswinkel.
 * @param x
 * @param y
 * @param z
 * @param matrix Die mit der Rotationssmatrix zu multiplizierende Matrix.
 */
void rotatePoseMatrix(float angle, float x, float y, float z, float* matrix) {
    // Sanity check
    if (!matrix)
        return;

    float rotate_matrix[16];
    setRotationMatrix(angle, x, y, z, rotate_matrix);

    // matrix * scale_matrix
    multiplyMatrix(matrix, rotate_matrix, matrix);
}
