/**
 * @file types.h
 *
 * @brief Beinhaltet eigene Datentyp-Definitionen.
 *
 * @author Alexander Bertram
 *
 * @date 17.06.2011
 */

#ifndef TYPES_H
#define TYPES_H

#include <GLES/gl.h>

#include <QCAR/Renderer.h>

/**
 * Aktion, die der Benutzer auf dem Touchscreen ausführt.
 */
enum ActionType {
    /**
     * Start einer Druck-Geste.
     */
    ACTION_DOWN,
    /**
     * Änderungen an einer Druck-Geste, Bewegung mit dem Finger auf dem Bildschirm.
     */
    ACTION_MOVE,
    /**
     * Ende einer Druck-Geste.
     */
    ACTION_UP,
    /**
     * Abbruch einer Druck-Geste.
     */
    ACTION_CANCEL
};

/**
 * @brief Touch-Ereignis.
 */
typedef struct TouchEvent {
    /**
     * Aktion, die der Benutzer auf dem Touchscreen ausführt.
     */
    ActionType actionType;
    /**
     * Position, an der die Druck-Geste begonnen hat.
     */
    QCAR::Vec2F startPosition;
    /**
     * Aktuelle Position der Druck-Geste.
     */
    QCAR::Vec2F currentPosition;
    /**
     * Letzte Position der Druck-Geste. Wird am Anfang der Ereignis-Behandlung mit
     * @link currentPosition @endlink überschrieben, wenn die Druck-Geste bereits begonnen hat.
     */
    QCAR::Vec2F lastPosition;
    /**
     * Position des Tippens auf den Touchscreen, wenn es einer war.
     */
    QCAR::Vec2F tapPosition;
    /**
     * Flag, das anzeigt, ob die letzte Druck-Geste ein Tipp auf den Touchscreen war.
     */
    bool isTap;
    /**
     * Startzeit der Geste (nötig für die Entscheidung, ob die letzte Druck-Geste ein Tipp auf den
     * Touchscreen war).
     */
    unsigned long startTime;
    /**
     * Endzeit der Geste (nötig für die Entscheidung, ob die letzte Druck-Geste ein Tipp auf den
     * Touchscreen war).
     */
    unsigned long lifeTime;
    /**
     * Distanz zwischen Start- und End-Positon (nötig für die Entscheidung, ob die letzte
     * Druck-Geste ein Tipp auf den Touchscreen war).
     */
    float startPositionLastPositionDistance;
    /**
     * Flag, das anzeigt, ob die Geste aktiv ist.
     */
    bool isActive;
} TouchEvent;

/**
 * Feld-Art. Die Reihenfolge muss der Reihenfolge der Texturen aus dem Java-Code entsprechen.
 */
enum SpaceType {
    /**
     * Start-Feld.
     */
    START_SPACE,
    /**
     * Normales Feld.
     */
    SPACE,
    /**
     * Sonderfeld.
     */
    SPECIAL_SPACE,
    /**
     * Ziel-Feld.
     */
    TARGET_SPACE
};

/**
 * @brief Feld.
 */
typedef struct Space {
    /**
     * Feld-Id.
     */
    int id;
    /**
     * Feld-Typ.
     */
    SpaceType type;
    /**
     * Position, an der das Feld gerendert werden soll.
     */
    QCAR::Vec2F position;
    /**
     * Winkel, um den das Feld bzw. die Feld-Textur beim Rendern gedrehen werden soll.
     */
    float angle;
    /**
     * Flag, das anzeigt, ob ein Feld durch eine Spielfigur besetzt ist.
     */
    bool occupied;
} Space;

/**
 * Maximale Anzahl der Felder.
 *
 * Diese Konstante musste hier definiert werden, um Probleme aufgrund von zyklischen Abhängigkeiten
 * zwischen constants.h und types.h zu umgehen.
 */
#define MAX_SPACE_COUNT 100

/**
 * @brief Spielfeld.
 */
typedef struct Field {
    /**
     * Anzahl der Felder.
     */
    int length;
    /**
     * Felder.
     */
    Space spaces[MAX_SPACE_COUNT];
} Field;

/**
 * Spielfigur-Zustände.
 */
enum PawnState {
    /**
     * Spielfigur ruht.
     */
    PAWN_STATE_RESTING,
    /**
     * Nächster Spielfigur-Zustand wird künstlich verzögert.
     */
    PAWN_STATE_DELAYING,
    /**
     * Spielfigur bereitet ihren Zug vor.
     */
    PAWN_STATE_PRE_MOVING,
    /**
     * Spielfigur zieht.
     */
    PAWN_STATE_MOVING,
    /**
     * Spielfigur hat ihren Zug beendet.
     */
    PAWN_STATE_POST_MOVING
};

/**
 * @brief Lichtkomponenten.
 */
typedef struct Light {
    /**
     * Ambienter Anteil.
     */
    GLfloat ambient[4];
    /**
     * Diffuser Anteil.
     */
    GLfloat diffuse[4];
    /**
     * Spekularer Anteil.
     */
    GLfloat specular[4];
} Light;

/**
 * @brief Spielfigur.
 */
typedef struct Pawn {
    /**
     * Aktueller Spielfigur-Zustand.
     */
    PawnState state;
    /**
     * Nächster Spielfigur-Zustand (nach der Verzögerung).
     */
    PawnState nextState;
    /**
     * Aktuelle Position der Spielfigur.
     */
    QCAR::Vec2F position;
    /**
     * Geschwindigkeit der Spielfigur (für die Animation beim Ziehen).
     */
    QCAR::Vec2F velocity;
    /**
     * Feld auf dem die Spielfigur steht.
     */
    Space *currentSpace;
    /**
     * Feld, auf das die Spielfigur ziehen soll.
     */
    Space *targetSpace;
    /**
     * Verzögerungszeit, nach der die Figur ihre Aktion fortführen soll.
     */
    unsigned int continueTime;
    /**
     * Licht, von dem die Spielfigur beleuchtet wird.
     */
    Light light;
} Pawn;

/**
 * Spiel-Status.
 */
enum GameState {
    /**
     * Spiel ist nicht initialisiert.
     */
    GAME_STATE_NOT_INITED,
    /**
     * Hinweis anzeigen, dass zum Spielen die Handykamera auf das Bild gerichtet werden muss.
     */
    GAME_STATE_SHOW_HINT_MARKER_NEEDED,
    /**
     * Warten, dass die Kamera auf das Bild gerichtet wird.
     */
    GAME_STATE_WAITING_FOR_MARKER,
    /**
     * Anleitung zum Erzeugen des Spielfeldes anzeigen.
     */
    GAME_STATE_SHOW_HINT_CREATE_SPACES,
    /**
     * Spielfeld wird erzeugt.
     */
    GAME_STATE_CREATING_FIELD,
    /**
     * Erster Spieler wird bestimmt.
     */
    GAME_STATE_DRAWING_BEGINNING_PLAYER,
    /**
     * Spiel läuft.
     */
    GAME_STATE_RUNNING,
    /**
     * Spiel ist zu Ende.
     */
    GAME_STATE_GAME_OVER
};

/**
 * Spieler-Art.
 */
enum PlayerType {
    /**
     * Computer.
     */
    PLAYER_TYPE_COMPUTER,
    /**
     * Mensch.
     */
    PLAYER_TYPE_HUMAN
};

/**
 * Spieler-Zustand.
 */
enum PlayerState {
    /**
     * Spieler ruht.
     */
    PLAYER_STATE_RESTING,
    /**
     * Spieler wartet auf eine (externe) Aktion.
     */
    PLAYER_STATE_WAITING,
    /**
     * Spieler bereitet seinen Zug vor.
     */
    PLAYER_STATE_PRE_MOVING,
    /**
     * Spieler zieht.
     */
    PLAYER_STATE_MOVING,
    /**
     * Spieler ist mit seinem Zug fertig.
     */
    PLAYER_STATE_POST_MOVING,
    /**
     * Spieler wartet, bis er seine nächste Aktion ausführen darf.
     */
    PLAYER_STATE_DELAYING
};

/**
 * @brief Spieler.
 */
typedef struct Player {
    /**
     * Spieler-Name.
     */
    char name[15];
    /**
     * Spieler-Art.
     */
    PlayerType type;
    /**
     * Aktueller Zustand des Spielers.
     */
    PlayerState state;
    /**
     * Nächster Zustand des Spielers.
     */
    PlayerState nextState;
    /**
     * Index des Feldes, auf dem der Spieler aktuell steht.
     */
    int currentSpaceIndex;
    /**
     * Index des Feldes, auf das der Spieler ziehen soll.
     */
    int targetSpaceIndex;
    /**
     * Spielfigur des Spielers.
     */
    Pawn pawn;
    /**
     * Verzögerungszeit, nach der der Spieler seine nächste Aktion ausführt.
     */
    unsigned int continueTime;
} Player;

#endif
