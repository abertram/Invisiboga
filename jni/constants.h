/**
 * @file constants.h
 *
 * @brief Konstanten, die für die Anwendung benötigt werden.
 *
 * @author Alexander Bertram
 *
 * @date 17.06.2011
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "types.h"

/**
 * Feld-Radius.
 */
#define SPACE_RADIUS 10.0f
/**
 * Zeit in Millisekunden, bis zu der eine Bildschirm-Berührung als Tippen gilt.
 */
#define MAX_TAP_TIME 200
/**
 * Entfernung in Pixeln, die zwischen Anfang und Ende der Bildschirm-Berührung zurückgelegt wird
 * und bis zu der eine Bildschrim-Berührung als Tippen gilt.
 */
#define MAX_TAP_SQUARED_DISTANCE 400
/**
 * Koordinaten der Ecken eines Einheitsquadrates.
 */
static const float UNIT_SQUARE_VERTICES[] = {
    -1.0, 1.0, 0.0,
    1.0, 1.0, 0.0,
    -1.0, -1.0, 0.0,
    1.0, -1.0, 0.0 };
/**
 * Normalen der Ecken des Einheitsquadrates. Diese zeigen alle in die positive z-Richtung.
 */
static const float UNIT_SQUARE_NORMALS[] = {
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0,
    0.0, 0.0, 1.0 };
/**
 * Textur-Koordinaten eines Einheitsquadrates.
 */
static const float UNIT_SQUARE_TEXTURE_COORDINATES[] = {
    0.0, 1.0,
    1.0, 1.0,
    0.0, 0.0,
    1.0, 0.0 };
/**
 * Anzahl der Spieler.
 */
#define PLAYER_COUNT 2
/**
 * Anzahl der Translationen pro Sekunde für die Animation.
 */
#define TRANSLATIONS_PER_SECOND 1.f
/**
 * Anzahl der Felder, bei der der "Weiter"-Button angezeigt wird.
 */
#define SPACE_COUNT_TO_SHOW_NEXT_BUTTON 5
/**
 * Anzahl der Felder, bei der der "Neustart"-Button angezeigt wird.
 */
#define SPACE_COUNT_TO_SHOW_RESTART_BUTTON 1
/**
 * Maximale Anzahl der Augenwürfel.
 */
#define MAX_DIE_PIP_COUNT 6
/**
 * Zeit in ms, die genutzt werden kann, um eine Spielfigur zwischen einzelnen Aktionen warten zu
 * lassen und somit eine Verzögerung zu simulieren.
 */
#define PAWN_DELAY 1000
/**
 * Lichtkomponenten der Spielfiguren. Jeweils der ambiente, der diffuse und der spekulare Anteil.
 */
static const Light PAWN_LIGHTS[PLAYER_COUNT] = {
    {
        // rot
        { 1.f, 0.f, 0.f, 1.f },
        { 1.f, 0.f, 0.f, 1.f },
        { 1.f, 1.f, 1.f, 1.f } },
    {
        // grün
        { 0.f, 1.f, 0.f, 1.f },
        { 0.f, 1.f, 0.f, 1.f },
        { 1.f, 1.f, 1.f, 1.f } } };

/**
 * Zeit in ms, die genutzt werden kann, um einen Spieler zwischen einzelnen Aktionen warten zu
 * lassen und somit eine Verzögerung zu simulieren.
 */
#define PLAYER_DELAY 2000
/**
 * Parameter für die kurze Anzeige eines Toasts.
 */
#define TOAST_DURATION_SHORT 0
/**
 * Parameter für die lange Anzeige eines Toasts.
 */
#define TOAST_DURATION_LONG 1
/**
 * Skalierungsfaktor für die Spielfigur.
 */
#define PAWN_SCALE_FACTOR 20.f

/**
 * Definition des Spieler-Typen pro Spieler.
 */
static const PlayerType PLAYER_TYPES[PLAYER_COUNT] = {
    // der erste Spieler ist ein Mensch
    PLAYER_TYPE_HUMAN,
    // der zweite Spieler ist ein Computer-Spieler
    PLAYER_TYPE_COMPUTER
};

#endif
