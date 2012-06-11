/**
 * @file field.cpp
 *
 * @brief Beinhaltet das Spielfeld und Funktionen für dessen Analyse und Manipulation.
 *
 * @author Alexander Bertram
 *
 * @date 03.09.2011
 */

#include <math.h>

#include "constants.h"
#include "mathUtils.h"
#include "types.h"
#include "utils.h"

/**
 * Spielfeld.
 */
Field field;

/**
 * Überprüft anhand der Abstände zu den anderen Feldern, ob an der übergebenen Position ein Feld
 * erstellt werden kann.
 *
 * @param position Die zu überprüfende Position.
 * @return True, wenn an der übergebenen Position ein Feld erstell werden kann.
 */
bool canCreateSpace(QCAR::Vec2F position) {
    bool result = true;
    // nur ausführen, wenn mindestens ein Feld vorhanden ist
    if (field.length > 0) {
        // Abstand zum zuletzt erzeugten Feld berechnen
        float distance = vec2FDistance(position, field.spaces[field.length - 1].position);
        // Prüfung, ob der Abstand zum zuletzt erzeugten Feld den doppelten Feldradius überschreitet
        result = distance > 2 * SPACE_RADIUS;
        if (result) {
            // Abstand zu allen anderen Feldern berechnen und prüfen, ob der Abstand ok ist
            for (int i = 0; i < field.length && result; ++i) {
                // das zuletzt erstellte Feld überspringen, das wurde bereits berechent.
                if (i != field.length - 1) {
                    distance = vec2FDistance(position, field.spaces[i].position);
                    result = distance > 2 * SPACE_RADIUS;
                }
            }
        }
    }
    return result;
}

/**
 * Erstellt ein neues Feld an der übergebenen Position.
 *
 * @param position Position, an der ein Feld erstellt werden soll.
 */
void createSpace(QCAR::Vec2F position) {
    LOGD("createSpace");
    // nichts tun, wenn die maximale Anzahl der Felder bereits erreicht wurde
    if (field.length == MAX_SPACE_COUNT) {
        return;
    }
    // Feld initialisieren
    Space* space = &field.spaces[field.length];
    space->id = field.length;
    space->position = position;
    space->angle = 0.f;
    space->occupied = false;
    if (field.length == 0) {
        // wenn noch keine Felder da sind, ist das aktuelle Feld das Startfeld
        space->type = START_SPACE;
    } else if (field.length == 1) {
        // bei zwei Feldern ist das zweite Feld per Definition das Zielfeld
        space->type = TARGET_SPACE;
        // Winkel vom ersten Feld in Richtung des zweiten Feldes berechnen, um das erste Feld in
        // diese Richtung drehen zu können
        float angle = 0.f;
        QCAR::Vec2F d = vec2FSub(space->position, field.spaces[0].position);
        angle = atan2(d.data[1], d.data[0]);
        angle *= 180.f / M_PI;
        while (angle < -180.f) {
            angle += 360.f;
        }
        while (angle > 180.f) {
            angle -= 360.f;
        }
        field.spaces[0].angle = angle;
    } else {
        // das zuletzt erstellte Feld ist das Zielfeld
        space->type = TARGET_SPACE;
        // das vorletzte erstellte Feld ist ein normales Feld
        field.spaces[field.length - 1].type = SPACE;
    }
    field.length++;
}

/**
 * Überprüft, ob sich an einer bestimmten Position bereits ein Feld befindet.
 *
 * @param position Position, die überprüft werden soll.
 * @return Das Feld, wenn eins gefunden wird, sonst NULL.
 */
Space* selectSpace(QCAR::Vec2F position) {
    Space* space = NULL;
    // nur ausführen, wenn mindestens ein Feld vorhanden ist
    if (field.length > 0) {
        // über die Felder iterieren
        for (int i = 0; i < field.length && space == NULL; ++i) {
            // Distanz zur übergebenen Position berechnen und prüfen ob diese den Feldradius
            // unterschreitet
            if (vec2FDistance(position, field.spaces[i].position) <= SPACE_RADIUS) {
                space = &field.spaces[i];
            }
        }
    }
    return space;
}
