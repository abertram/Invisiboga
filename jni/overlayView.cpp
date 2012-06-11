/**
 * @file overlayView.cpp
 *
 * @brief Beinhaltet native Funktionen, die aus der OverlayView.java aufgerufen werden.
 *
 * @author Alexander Bertram
 *
 * @date 23.06.2011
 */

#include <jni.h>

#include "game.h"
#include "utils.h"

// folgender Code ist C-Code
#ifdef __cplusplus
extern "C" {
#endif

    /*
     * Aktueller Spieler (verwaltet in game.cpp).
     */
    extern Player* currentPlayer;
    /*
    * Spielfeld (verwaltet in field.cpp).
     */
    extern Field field;

    /**
     * Versetzt das Spiel in den Spiel-Zustand, in dem der beginnende Spieler bestimmt wird,
     * nachdem das Spielfeld erzeugt wurde.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_OverlayView_nextButtonClickNative(
            JNIEnv*, jobject) {
        LOGD("Java_de_fhwedel_vr2_invisiboga_OverlayView_nextButtonClickNative");
        updateGameState(GAME_STATE_DRAWING_BEGINNING_PLAYER);
    }

    /**
     * Simuliert einen Würfel-Wurf des menschlichen Spielers.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_OverlayView_diceButtonClickNative(
            JNIEnv*, jobject) {
        LOGD("Java_de_fhwedel_vr2_invisiboga_OverlayView_diceButtonClickNative");
        // Spieler in den Zustand versetzen, in dem der Zug vorbereitet wird
        currentPlayer->state = PLAYER_STATE_PRE_MOVING;
    }

    /**
     * Führt einen Spiel-Neustart durch.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_OverlayView_restartNative(JNIEnv*,
            jobject) {
        LOGD("Java_de_fhwedel_vr2_invisiboga_OverlayView_restartNative");
        // Anzahl der Felder resetten
        field.length = 0;
        // Spiel neustarten
        restartGame();
    }

#ifdef __cplusplus
}
#endif
