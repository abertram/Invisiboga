/**
 * @file game.cpp
 *
 * @brief Kontrolliert den Spielablauf.
 *
 * @details Der Spielablauf wird in einem Zustandsautomaten verwaltet.
 *
 * @author Alexander Bertram
 *
 * @date 23.06.2011
 */

#include <stdlib.h>

#include "constants.h"
#include "game.h"
#include "invisibogaGlSurfaceView.h"
#include "pawn.h"
#include "player.h"
#include "Texture.h"
#include "types.h"
#include "utils.h"

/**
 * Der aktuelle Spiel-Zustand.
 */
int gameState = GAME_STATE_NOT_INITED;
/**
 * Spieler.
 */
Player players[PLAYER_COUNT];
/**
 * Index des Spielers, der an der Reihe ist.
 */
int currentPlayerIndex;
/**
 * Spieler, der an der Reihe ist.
 */
Player *currentPlayer;
/*
 * Spielfeld (verwaltet in field.cpp).
 */
extern Field field;

/**
 * Initialisiert das Spiel.
 */
void initGame() {
    LOG("initGame");
    // Start-Zustand setzen
    updateGameState(GAME_STATE_SHOW_HINT_MARKER_NEEDED);
    // Spieler initialisieren
    for (int i = 0; i < PLAYER_COUNT; ++i) {
        // Spielertyp initialisieren
        players[i].type = PLAYER_TYPES[i];
        initPlayer(&players[i]);
        // Spielernamen setzen
        sprintf(players[i].name, "Spieler %d", i + 1);
        // Farbe der Spielfigur setzen
        players[i].pawn.light = PAWN_LIGHTS[i];
    }
}

/**
 * Versetzt das Spiel in den Startzustand.
 */
void restartGame() {
    LOGD("restartGame");
    // Spiel-Zustand auf den Zustand setzen, in dem der Benutzer das Spielfeld erstellt
    gameState = GAME_STATE_CREATING_FIELD;
    // Spieler initialisieren
    for (int i = 0; i < PLAYER_COUNT; ++i) {
        initPlayer(&players[i]);
    }
}

/**
 * Setzt den aktuellen Spieler anhand des Spieler-Indexes.
 *
 * @param playerIndex Spieler-Index.
 */
void setCurrentPlayer(int playerIndex) {
    LOGD("Game.setCurrentPlayer");
    // Spieler-Index merken
    currentPlayerIndex = playerIndex;
    // Spieler merken
    currentPlayer = &players[playerIndex];
    // erforderliche Aktionen auf der Java-Ebene anstoßen
    setCurrentPlayer(currentPlayer);
}

/**
 * Aktualisiert den aktuellen Spiel-Zustand.
 *
 * @param newGameState Neuer Spielzustand.
 */
void updateGameState(int newGameState) {
    LOGD("updateGameState");
    // nichts tun, wenn sich der Zustand nicht verändert hat
    if (newGameState == gameState) {
        return;
    }
    // Spielzustand speichern
    gameState = newGameState;
    // Buffer für Toast-Inhalte
    char text[256];
    // Spiel-Zustand auswerten und die entsprechenden Aktionen ausführen
    switch (gameState) {
        case GAME_STATE_SHOW_HINT_MARKER_NEEDED:
            // um spielen zu können, muss die Kamera auf den Marker gerichtet sein

            sprintf(text, "Halte die Kamera auf den Marker.");
            showToast(text, TOAST_DURATION_LONG);
            gameState = GAME_STATE_WAITING_FOR_MARKER;
            break;
        case GAME_STATE_WAITING_FOR_MARKER:
            // warten, bis die Handy-Kamera auf das Bild gerichtet ist

            break;
        case GAME_STATE_SHOW_HINT_CREATE_SPACES:
            // Hinweise anzeigen, wie das Spielfeld erzeugt werden kann

            sprintf(text, "Halte die Kamera weiterhin auf den Marker.");
            showToast(text, TOAST_DURATION_LONG);
            sprintf(text, "Erzeuge nun ein Spielfeld, indem du auf den Bildschirm tippst oder ...");
            showToast(text, TOAST_DURATION_LONG);
            sprintf(text, "... mit einem Finger langsam ueber den Bildschirm ziehst.");
            showToast(text, TOAST_DURATION_LONG);
            sprintf(text,
                    "Einzelne Felder koennen durch Antippen in Sonderfelder umgewandelt werden.");
            showToast(text, TOAST_DURATION_LONG);
            sprintf(text, "Bleibt ein Spieler auf einem Sonderfeld stehen, ...");
            showToast(text, TOAST_DURATION_LONG);
            sprintf(text,
                    "... darf er um ein paar Felder vorruecken oder muss ein paar Felder zurueck.");
            showToast(text, TOAST_DURATION_LONG);
            gameState = GAME_STATE_CREATING_FIELD;
            break;
        case GAME_STATE_CREATING_FIELD:
            // Spielfeld kann erzeugt werden

            break;
        case GAME_STATE_DRAWING_BEGINNING_PLAYER:
            // ersten Spieler durch Würfeln bestimmen

            LOGI("Drawing beginning player");
            // Die Felder auf denen die Spielfiguren stehen, können erst jetzt initialisiert werden,
            // weil die Positionen der Felder erst jetzt bekannt sind.
            for (int i = 0; i < PLAYER_COUNT; ++i) {
                initPawnSpaces(&players[i].pawn);
            }
            // currentPlayerTextView anzeigen
            showView("currentPlayerTextView");
            // aktuellen Spieler auswürfeln und setzen
            setCurrentPlayer(rand() % PLAYER_COUNT);
            // "Würfeln"-Buuton anzeigen, damit der menschliche Spieler würfeln kann
            if (currentPlayer->type == PLAYER_TYPE_HUMAN) {
                showView("diceButton");
            }
            updateGameState(GAME_STATE_RUNNING);
            break;
        case GAME_STATE_RUNNING:
            // Spiel läuft

            break;
        default:
            LOGE("Unknown game state: %d", gameState);
            break;
    }
}

/**
 * Verarbeitet den Zug.
 *
 * @param timeIntervalSinceLastFrame Zeit in ms, die seit der Berechnung des letzten Bildes
 *            vergangen ist (nötig für die Animation der Spielfiguren).
 */
void processMove(float timeIntervalSinceLastFrame) {
    // aktuellen Spiel-Zustand auswerten und die entsprechenden Aktionen ausführen
    switch (gameState) {
        case GAME_STATE_RUNNING: {
            // Spiel läuft

            // Prüfung, ob der aktuelle Spieler senen Zug beeendet hat
            if (currentPlayer->state == PLAYER_STATE_POST_MOVING) {
                // Prüfung, ob der Spieler auf einem Sonderfeld gelandet ist
                if (field.spaces[currentPlayer->targetSpaceIndex].type == SPECIAL_SPACE) {
                    // Spieler ist auf einem Sonderfeld gelandet. Es wird per Zufall bestimmt, ob er
                    // vor oder zurück darf und wie weit.

                    // Vorzeichen bestimmen, 0: Spieler darf vor, 1: Spieler muss zurück
                    int sign = rand() % 2;
                    // Betrag bestimmen
                    int value = rand() % MAX_DIE_PIP_COUNT + 1;
                    // Betrag evtl. negieren
                    value *= sign == 0 ? +1 : -1;
                    // neues Ziel setzen
                    setPlayerTargetSpace(currentPlayer, currentPlayer->targetSpaceIndex + value);
                    // Hinweis anzeigen
                    char text[256];
                    sprintf(text, "%s ist auf einem Sonderfeld gelandet und %s %d %s %s",
                            currentPlayer->name, value >= 0 ? "darf um" : "muss", abs(value),
                            abs(value) == 1 ? "Feld" : "Felder",
                            value >= 0 ? "vorruecken" : "zurueck");
                    showToast(text, TOAST_DURATION_LONG);
                    // Zug vorbereiten
                    prepareMove(currentPlayer);
                } else if (currentPlayer->targetSpaceIndex < field.length - 1) {
                    // Aktueller Spieler hat das Zielfeld noch nicht erreicht. Es wird der nächste
                    // Spieler bestimmt und die dafür nötigen Aktionen ausgeführt.

                    // aktuellen Spieler schon mal für seinen nächsten Zug vorbereiten
                    if (currentPlayer->type == PLAYER_TYPE_HUMAN) {
                        // menschlicher Spieler muss warten, bis der "Würfeln"-Button gedrückt wird
                        currentPlayer->state = PLAYER_STATE_WAITING;
                    } else {
                        // Computer-Spieler kann gleich würfeln und ziehen
                        currentPlayer->state = PLAYER_STATE_PRE_MOVING;
                    }
                    // aktuellen Spieler aktualisieren
                    setCurrentPlayer((currentPlayerIndex + 1) % PLAYER_COUNT);
                    // evtl. den "Würfeln"-Button anzeigen
                    if (currentPlayer->type == PLAYER_TYPE_HUMAN) {
                        showView("diceButton");
                    }
                } else {
                    // Der aktuelle Spieler hat das Ziel erreicht und gewinnt.

                    // Hinweis anzeigen
                    char text[256];
                    sprintf(text, "%s erreicht als erster das Ziel und gewinnt",
                            currentPlayer->name);
                    showToast(text, TOAST_DURATION_LONG);
                    // Spiel-Zustand aktualisieren
                    gameState = GAME_STATE_GAME_OVER;
                }
            }
            // den Zug vom aktuellen Spieler weiterverarbeiten lassen
            processPlayerMove(currentPlayer, timeIntervalSinceLastFrame);
            break;
        }
        case GAME_STATE_GAME_OVER: {
            // Spiel ist zu Ende

            LOGD("Game state is GAME_STATE_GAME_OVER");
            break;
        }
        default:
            break;
    }
}
