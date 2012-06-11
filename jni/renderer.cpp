/**
 * @file renderer.cpp
 *
 * @brief Beinhaltet Funktionen zum Rendern der erweiterten Realität.
 *
 * @author Alexander Bertram
 *
 * @date 17.09.2011
 */

#include <GLES/gl.h>
#include <GLES/glext.h>

#include "constants.h"
#include "mathUtils.h"
#include "pawn.h"
#include "Texture.h"
#include "types.h"
#include "utils.h"

/*
 * Aktueller Spiel-Zustand (verwaltet in game.cpp).
 */
extern int gameState;
/*
 * Model-View-Matrix (verwaltet in invisibogaGlSurfaceView.cpp).
 */
extern QCAR::Matrix44F modelViewMatrix;
/*
 * Spieler (verwaltet in game.cpp).
 */
extern Player players[];
/*
 * Projektionsmatrix (verwaltet in invisiboga.cpp).
 */
extern QCAR::Matrix44F projectionMatrix;
/*
 * Spielfeld (verwaltet in game.cpp).
 */
extern Field field;
/*
 * Texturen (verwaltet in invisiboga.cpp).
 */
extern Texture **textures;

/**
 * Rendert ein Feld.
 *
 * @param space Das zu rendernde Feld.
 */
void renderSpace(Space space) {
    // Textur-ID anhand des Feld-Typen bestimmen und an das Ziel binden
    glBindTexture(GL_TEXTURE_2D, textures[space.type]->mTextureID);
    // Transformationsmatrix mit der Identitätsmatrix initialisieren und Translation, Skalierung und
    // Rotation reinrechnen
    QCAR::Matrix44F transform = matrix44FIdentity();
    translatePoseMatrix(space.position.data[0], space.position.data[1], 0.0f, &transform.data[0]);
    scalePoseMatrix(SPACE_RADIUS, SPACE_RADIUS, SPACE_RADIUS, &transform.data[0]);
    rotatePoseMatrix(space.angle, 0, 0, 1, &transform.data[0]);
    // aktuelle Matrix sichern
    glPushMatrix();
    // Transformationsmatrix anwenden
    glMultMatrixf(&transform.data[0]);
    // Feld rendern
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // gesicherte Matrix wiederherstellen
    glPopMatrix();
}

/**
 * Rendert das Spielfeld.
 *
 * Jedes Feld wird mit Hilfe eines transparenten Quadrates mit einer überlagerten Textur gerendert.
 */
void renderField() {
    // Textur-Koordinaten werden verwendet
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    // Blending für Transparenz notwendig
    glEnable(GL_BLEND);
    // 2D-Texturen werden verwendet
    glEnable(GL_TEXTURE_2D);
    // Tiefentest aus, damit die Ecken der Qudrate Teile der benachbarten Felder nicht verdecken
    glDisable(GL_DEPTH_TEST);
    // Licht nicht nötig, es werden Texturen verwendet
    glDisable(GL_LIGHTING);
    // Transparenz aktivieren
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Vertices setzen
    glVertexPointer(3, GL_FLOAT, 0, &UNIT_SQUARE_VERTICES[0]);
    // Normale setzen
    glNormalPointer(GL_FLOAT, 0, &UNIT_SQUARE_NORMALS[0]);
    // Textur-Koordinaten setzen
    glTexCoordPointer(2, GL_FLOAT, 0, &UNIT_SQUARE_TEXTURE_COORDINATES[0]);
    // Spielfeld rendern
    for (int i = 0; i < field.length; i++) {
        renderSpace(field.spaces[i]);
    }
}

/**
 * Rendert eine Spielfigur.
 *
 * @param pawn Die zu rendernde Spielfigur.
 */
void renderPawn(Pawn *pawn) {
    // Lichtkomponenten setzen
    glLightfv(GL_LIGHT0, GL_AMBIENT, pawn->light.ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, pawn->light.diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, pawn->light.specular);
    // Transformationsmatrix mit der Identitätsmatrix initialisieren und Translation und Skalierung
    // reinrechnen
    QCAR::Matrix44F transform = matrix44FIdentity();
    translatePoseMatrix(pawn->position.data[0], pawn->position.data[1], 0.f, &transform.data[0]);
    scalePoseMatrix(PAWN_SCALE_FACTOR, PAWN_SCALE_FACTOR, PAWN_SCALE_FACTOR, &transform.data[0]);
    // gesicherte Matrix wiederherstellen
    glPushMatrix();
    // Transformationsmatrix anwenden
    glMultMatrixf(&transform.data[0]);
    // Spielfigur rendern
    glDrawArrays(GL_TRIANGLES, 0, PAWN_VERTICES_COUNT);
    // gesicherte Matrix wiederherstellen
    glPopMatrix();
}

/**
 * Rendert die Spielfiguren.
 *
 * Die Spielfiguren werden gerendert, indem sie durch eine Lichtquelle beleuchtet werden, die an die
 * Position des Beobachters gebunden ist.
 */
void renderPawns() {
    // keine Textur-Koordinaten nötig
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    // Tiefen-Test für die richtige Beleuchtung ein
    glEnable(GL_DEPTH_TEST);
    // Licht allgemein und erste Lichtquelle einschalten
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    // Normale sollen skaliert werden
    glEnable(GL_RESCALE_NORMAL);
    // Blending ausschalten
    glDisable(GL_BLEND);
    // 2D-Textur ausschalten
    glDisable(GL_TEXTURE_2D);
    // Vertices setzen
    glVertexPointer(3, GL_FLOAT, 0, &pawnVertices[0]);
    // Normale setzen
    glNormalPointer(GL_FLOAT, 0, &pawnNormals[0]);
    // Spielfiguren rendern
    for (int i = 0; i < PLAYER_COUNT; ++i) {
        renderPawn(&players[i].pawn);
    }
}

/**
 * Rendert die Realitätserweiterung.
 */
void renderAugmentation() {
    // Vertices und Normale werden immer benötigt
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    // Projektionsmatrix laden
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(projectionMatrix.data);
    // Positon der Lichtquelle setzen
    // Lichtquelle wird an die Position des Beobachters gebunden und bewegt sich mit dieser
    const GLfloat light0Position[] = { 0.0, 0.0, 0.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, &light0Position[0]);
    // Model-View-Matrix laden
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(modelViewMatrix.data);
    // Spielfeld rendern
    renderField();
    // Spielfiguren rendern, wenn das Spiel läuft
    if (gameState >= GAME_STATE_RUNNING) {
        renderPawns();
    }
    // Prüfung, ob ein GL-Fehler aufgetreten ist
    checkGlError("renderAugmentation");
}
