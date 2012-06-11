/**
 * @file player.cpp
 *
 * @brief Beinhaltet Funktionen zur Verwaltung des Zustandes eines Spielers.
 *
 * @author Alexander Bertram
 *
 * @date 23.06.2011
 */

#include <stdlib.h>

#include "constants.h"
#include "invisibogaGlSurfaceView.h"
#include "mathUtils.h"
#include "pawn.h"
#include "timeUtilities.h"
#include "types.h"
#include "utils.h"

/*
 * Spielfeld (verwaltet in field.cpp).
 */
extern Field field;

/**
 * Initialisiert einen Spieler
 *
 * @param player Spieler, der initialisiert werden soll.
 */
void initPlayer(Player *player) {
    LOGD("initPlayer");
    // aktuelles Feld und Zielfeld setzen
    player->currentSpaceIndex = 0;
    player->targetSpaceIndex = 0;
    // Start-Zustand abhängig vom Spieler-Typ setzen
    player->state =
            player->type == PLAYER_TYPE_HUMAN ? PLAYER_STATE_WAITING : PLAYER_STATE_PRE_MOVING;
    // Spielfigur initialisieren
    initPawn(&player->pawn);
}

/**
 * Setzt das Zielfeld, auf das der Spieler ziehen soll.
 *
 * @param player Der ziehende Spieler.
 * @param targetSpaceIndex Index des Feldes.
 */
void setPlayerTargetSpace(Player *player, int targetSpaceIndex) {
    LOGD("setPlayerTargetSpace(%d, %d)", player, targetSpaceIndex);
    // Index setzen
    player->targetSpaceIndex = targetSpaceIndex;
    // dafür sorgen, dass der Spieler die Spielfeld-Grenzen nicht überschreitet
    player->targetSpaceIndex = max(0, player->targetSpaceIndex);
    player->targetSpaceIndex = min(player->targetSpaceIndex, field.length - 1);
}

/**
 * Bereitet die Spielfigur für einen Zug vor.
 *
 * @param player Der ziehende Spieler.
 */
void preparePawnMove(Player *player) {
    // aktuelles Feld der Spielfigur setzen
    setPawnCurrentSpace(&player->pawn, player->currentSpaceIndex);
    // Index des Zielfeldes berechnen. Dieser hängt davon ab, ob der Spieler vor- oder rückwärts
    // zieht (Stichwort: Sonderfelder).
    int pawnTargetSpaceIndex =
            player->targetSpaceIndex >= player->currentSpaceIndex ? player->currentSpaceIndex + 1 :
                player->currentSpaceIndex - 1;
    // Zielfeld der Spielfigur setzen
    setPawnTargetSpace(&player->pawn, pawnTargetSpaceIndex);
    // Spielfigur kurz warten lassen und Zug vorbereiten
    player->pawn.state = PAWN_STATE_DELAYING;
    player->pawn.nextState = PAWN_STATE_PRE_MOVING;
    player->pawn.continueTime = getCurrentTimeInMs() + PAWN_DELAY;
}

/**
 * Bereitet den Spieler-Zug vor.
 *
 * @param player Der zeihende Spieler.
 */
void prepareMove(Player *player) {
    LOGD("Prüfe, ob Ziel belegt ist");
    // Prüfung ob das Zielfeld belegt ist
    if (field.spaces[player->targetSpaceIndex].occupied) {
        // Ziel ist belegt
        LOGD("Ziel ist belegt");
        // Hinweis anzeigen
        char text[256];
        sprintf(
                text,
                "Zielfeld ist belegt. Spieler zieht auf das Feld davor (vom Startfeld aus gesehen).");
        showToast(text, TOAST_DURATION_LONG);
        // Ziel verändern und auf das Feld davor setzen
        player->targetSpaceIndex = max(0, player->targetSpaceIndex - 1);
    }
    // Prüfung, ob der Spieler überhaupt ziehen kann
    if (player->targetSpaceIndex != player->currentSpaceIndex) {
        // Spielfigur vorbereiten
        preparePawnMove(player);
        LOGD("Setting player state to PLAYER_STATE_MOVING");
        // Zustand aktualisieren
        player->state = PLAYER_STATE_MOVING;
    } else {
        LOGD("Setting player state to PLAYER_STATE_POST_MOVING");
        // Spieler kurz warten lassen und Zug beenden
        player->state = PLAYER_STATE_DELAYING;
        player->nextState = PLAYER_STATE_POST_MOVING;
        player->continueTime = getCurrentTimeInMs() + PLAYER_DELAY;
    }
}

/**
 * Verarbeitet einen Spieler-Zug.
 *
 * Je nach Spieler-Typ muss der Spieler auf eine Aktion von außen warten. Der menschliche Spieler
 * muss würfeln, der Computer-Spieler würfelt selbst. Anhand des Würfelergebnisses wird das Zielfeld
 * berechnet. Die Spielfigur des Spielers wird Feld für Feld Richtung Zielfeld bewegt, bis sie das
 * Zielfeld erreicht. Anschließend wird der entsprechende Spieler-Status gesetzt, damit in game.cpp
 * bekannt ist, dass der Spieler seinen Zug durchgeführt hat. Dort wird entschieden, wann ein
 * Spieler-Wechsel stattfindet.
 *
 * @param player Der ziehende Spieler.
 * @param timeIntervalSinceLastFrame Zeit-Intervall seit letztem Frame (nötig für die Animaiton).
 */
void processPlayerMove(Player *player, float timeIntervalSinceLastFrame) {
    // Spieler-Zustand auswerten und entsprechende Aktion(en) ausführen
    switch (player->state) {
        case PLAYER_STATE_WAITING:
            // Spieler wartet (auf eine Aktion von außen)

            break;
        case PLAYER_STATE_PRE_MOVING: {
            // Spieler bereitet seinen Zug vor

            // würfeln
            int diePips = rand() % MAX_DIE_PIP_COUNT + 1;
            // Zielfeld setzen
            setPlayerTargetSpace(player, player->targetSpaceIndex + diePips);
            // Hinweis anzeigen
            char text[256];
            sprintf(text, "%s wuerfelt eine %d", player->name, diePips);
            showToast(text, TOAST_DURATION_SHORT);
            // Zug vorbereiten
            prepareMove(player);
            break;
        }
        case PLAYER_STATE_MOVING: {
            // Spieler zieht

            // Spielfigur-Zustand auswerten und entsprechende Aktion(en) ausführen
            switch (player->pawn.state) {
                case PAWN_STATE_RESTING:
                    // Spielfigur ruht
                case PAWN_STATE_PRE_MOVING:
                    // Spielfigur bereitet ihren Zug vor
                case PAWN_STATE_MOVING:
                    // Spielfigur zieht
                    break;
                case PAWN_STATE_POST_MOVING:
                    // Spielfigur hat ihren Zug beendet

                    // Prüfung, ob die Spielfigur ihr endgültiges Ziel erreicht hat
                    if (player->pawn.targetSpace->id == player->targetSpaceIndex) {
                        // Zug beendet

                        // Feld, auf dem der Spieler vor dem Zug stand, als frei markieren
                        field.spaces[player->currentSpaceIndex].occupied = false;
                        // aktuellen Feld-Index aktualisieren
                        player->currentSpaceIndex = player->targetSpaceIndex;
                        // Feld, auf dem der Spieler jetzt steht, als besetzt markieren
                        field.spaces[player->currentSpaceIndex].occupied = true;
                        // Zustand aktualisieren
                        player->state = PLAYER_STATE_POST_MOVING;
                    } else {
                        // Zug noch nicht beendet

                        // aktuelles Feld und Zielfeld für die Spielfigur berechnen
                        // Diese hängen davon ab, ob der Spieler vor- oder rückwärts zieht
                        // (Stichwort: Sonderfelder).
                        int pawnCurrentSpaceIndex =
                                player->targetSpaceIndex >= player->currentSpaceIndex ?
                                    player->pawn.currentSpace->id + 1 :
                                    player->pawn.currentSpace->id - 1;
                        setPawnCurrentSpace(&player->pawn, pawnCurrentSpaceIndex);
                        int pawnTargetSpaceIndex =
                                player->targetSpaceIndex >= player->currentSpaceIndex ?
                                    player->pawn.targetSpace->id + 1 :
                                    player->pawn.targetSpace->id - 1;
                        setPawnTargetSpace(&player->pawn, pawnTargetSpaceIndex);
                        // Zustand der Spielfigur aktualisieren
                        player->pawn.state = PAWN_STATE_PRE_MOVING;
                    }
                    break;
                default:
                    break;
            }
            break;
        }
        case PLAYER_STATE_POST_MOVING:
            // Spieler hat seinen Zug beendet

            LOGD("Player state is POST_MOVING");
            break;
        case PLAYER_STATE_DELAYING:
            // Spiler-Aktion wird künstlich verzögert

            // Prüfung, ob die Verzögerungszeit abgelaufen ist
            if (getCurrentTimeInMs() >= player->continueTime) {
                // Zustand aktualisieren
                player->state = player->nextState;
            }
            break;
        default:
            break;
    }
    // Spielfigur-Zug verarbeiten
    processPawnMove(&player->pawn, timeIntervalSinceLastFrame);
}
