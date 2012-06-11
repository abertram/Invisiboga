/**
 * @file pawn.cpp
 *
 * @brief Funktionen zur Verwaltung des Zustandes einer Spielfigur.
 *
 * @author Alexander Bertram
 *
 * @date 22.08.2011
 */
#include "constants.h"
#include "mathUtils.h"
#include "pawn.h"
#include "timeUtilities.h"
#include "Types.h"
#include "utils.h"

/*
 * Spielfeld.
 */
extern Field field;

/**
 * Initialisiert eine Spielfigur.
 *
 * @param pawn Spielfigur, die initialisiert werden soll.
 */
void initPawn(Pawn *pawn) {
    LOGD("initPawn");
    pawn->state = PAWN_STATE_RESTING;
}

/**
 * Initialisiert das Feld, auf dem die Spielfigur zur Zeit steht, und das Zielfeld, auf das
 * die Spielfigur ziehen soll.
 *
 * @param pawn Spielfigur, deren Felder initialisiert werden sollen.
 */
void initPawnSpaces(Pawn *pawn) {
    LOGD("initPawnSpaces");
    setPawnCurrentSpace(pawn, 0);
    setPawnTargetSpace(pawn, 0);
}

/**
 * Setzt das Feld, auf dem die Spielfigur zur Zeit steht.
 *
 * @param pawn Spielfigur, deren aktuelles Feld gesetzt werden soll.
 * @param spaceIndex Index des aktuellen Feldes.
 */
void setPawnCurrentSpace(Pawn *pawn, int spaceIndex) {
    // Feld setzen
    pawn->currentSpace = &field.spaces[spaceIndex];
    // Position setzen
    pawn->position = pawn->currentSpace->position;
}

/**
 * Setzt das Feld, auf das die Spielfigur ziehen soll.
 *
 * @param pawn Spielfigur, deren Zielfeld aktualisiert werden soll.
 * @param spaceIndex Index des Zielfeldes.
 */
void setPawnTargetSpace(Pawn *pawn, int spaceIndex) {
    // Feld setzen
    pawn->targetSpace = &field.spaces[spaceIndex];
}

/**
 * Verarbeitet einen Spielfigur-Zug.
 *
 * Diese Funktion wird in player.cpp aufgerufen. Die Spielfigur zieht immer nur von einem Feld zum
 * nächsten und durchläuft dabei jedes Mal den Zustand-Zyklus PRE_TRANSLATING, TRANSLATING,
 * POST_TRANSLATING und DELAYING. Wann das endgültige Ziel erreicht ist, wird von dem Spieler
 * gesteuert, dem die Spielfigur zugeordnet. ist.
 *
 * @param pawn Die aktuell ziehende Spielfigur.
 * @param timeIntervalSinceLastFrame Vergangene Zeit seit letztem Frame (nötig für die Animation).
 */
void processPawnMove(Pawn *pawn, float timeIntervalSinceLastFrame) {
    // Spielfigur-Zustand auswerten und entsprechende Aktion(en) ausführen
    switch (pawn->state) {
        case PAWN_STATE_RESTING: {
            // Spielfigur ruht

            break;
        }
        case PAWN_STATE_DELAYING: {
            // nächste Spielfigur-Aktion wird künstlich verzögert

            // Prüfung, ob die Verzögerungszeit abgelaufen ist
            if (getCurrentTimeInMs() >= pawn->continueTime) {
                // Spielfigur-Zustand aktualisieren
                pawn->state = pawn->nextState;
            }
            break;
        }
        case PAWN_STATE_PRE_MOVING: {
            // Zug-Vorbereitung

            // Geschwindigkeit-Vektor berechnen
            QCAR::Vec2F velocity = vec2FSub(pawn->targetSpace->position,
                    pawn->currentSpace->position);
            // Geschwindigkeits-Vektor um TRANSLATIONS_PER_SECOND skalieren (CPU-Geschwindigkeit
            // unabhängige Animation) und in der Spielfigur speichern
            pawn->velocity = vec2FScale(velocity, TRANSLATIONS_PER_SECOND);
            // Aktualisierung des Spielfigur-Zustandes
            pawn->state = PAWN_STATE_MOVING;
            break;
        }
        case PAWN_STATE_MOVING: {
            // Spielfigur zieht

            // Berechnung der neuen Spielfigur-Position
            // Skalierung mit dem Faktor timeIntervalSinceLastFrame sorgt für eine
            // CPU-Geschwindigkeit unabhängige Animation
            pawn->position = vec2FAdd(pawn->position,
                    vec2FScale(pawn->velocity, timeIntervalSinceLastFrame));
            // Distanz von der Position des aktuellen Feldes zur Position des Zielfeldes der
            // Spielfigur berechnen
            float sourceSpaceTargetSpaceDistance = vec2FDistance(pawn->currentSpace->position,
                    pawn->targetSpace->position);
            // Distanz von der aktuellen Positon der Spielfigur zur Position des Zielfeldes der
            // Spielfigur berechnen
            float sourceSpaceCurrentPositionDistance = vec2FDistance(pawn->currentSpace->position,
                    pawn->position);
            // Prüfung, ob die Spielfigur ihr Zielfeld erreicht hat
            if (sourceSpaceCurrentPositionDistance >= sourceSpaceTargetSpaceDistance) {
                // Position der Spielfigur auf die Position des Zielfeldes setzen, um Sprünge zu
                // vermeiden
                pawn->position = pawn->targetSpace->position;
                // Spielfigur-Zustand aktualisieren
                pawn->state = PAWN_STATE_POST_MOVING;
            }
            break;
        }
        case PAWN_STATE_POST_MOVING: {
            // Spielfigur hat ihren Zug beendet

            break;
        }
        default:
            LOGE("Unknown pawn state %d", pawn->state);
            break;
    }
}
