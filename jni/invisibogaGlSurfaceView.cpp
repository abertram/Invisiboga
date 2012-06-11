/**
 * @file invisibogaGlSurfaceView.cpp
 *
 * @brief Gegenstück zu InvisibogaGlSurfaceView.java.
 *
 * @details Beinhaltet sowohl Funktionen, die aus der InvisibogaGlSurfaceView.java aufgerufen
 *      werden, als auch Funktionen, die Methoden aus InvisibogaGlSurfaceView.java aufrufen.
 *
 * @author Alexander Bertram
 *
 * @date 19.06.2011
 */

#include <jni.h>

#include <QCAR/Tool.h>
#include <QCAR/Trackable.h>

#include "game.h"
#include "invisiboga.h"
#include "renderer.h"
#include "Texture.h"
#include "timeUtilities.h"
#include "types.h"
#include "utils.h"

// folgender Code ist C-Code
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Zeiger auf die Java-Virtual-Machine (wird in der Init-Funktion für spätere Aufrufe
     * gespeichert).
     */
    static JavaVM* javaVm;
    /**
     * Das Java-Object, das die Init-Funktion aufruft (wird spätere Aufrufe gespeichert).
     */
    static jobject javaObject;
    /**
     * Die Java-Klasse, die die Init-Funktion aufruft (wird spätere Aufrufe gespeichert).
     */
    static jclass javaClass;
    /*
     * Aktueller Spiel-Zustand (verwaltet in game.cpp).
     */
    extern int gameState;
    /**
     * Zeit der letzten Frame-Berechnung.
     */
    unsigned long lastFrameTime;
    /**
     * Model-View-Matrix.
     */
    QCAR::Matrix44F modelViewMatrix;
    /**
     * Flag das anzeigt, ob ein Trackable sichtbar ist.
     */
    bool trackableVisible = false;
    /*
     * Bildschrim-Höhe (verwaltet in invisiboga.cpp).
     */
    extern unsigned int screenHeight;
    /*
     * Bildschirm-Breite (verwaltet in invisiboga.cpp).
     */
    extern unsigned int screenWidth;
    /*
     * Anzahl der Texturen (verwaltet in invisiboga.cpp).
     */
    extern int textureCount;
    /*
     * Texturen (verwaltet in invisiboga.cpp).
     */
    extern Texture **textures;

    /**
     * Führt Initialiserungen durch.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_nativeInit(
            JNIEnv* env, jobject obj) {
        LOG("Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_nativeInit");
        // aufrufendes Java-Objekt speichern
        javaObject = env->NewGlobalRef(obj);
        jclass objClass = env->GetObjectClass(obj);
        // aufrufende Java-Klasse speichern
        javaClass = (jclass)env->NewGlobalRef(objClass);
        // Java-VM speichern
        env->GetJavaVM(&javaVm);
    }

    /**
     * Rendert einen Frame.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_renderFrame(
            JNIEnv*, jobject) {
        // Zeitintervall seit letztem Frame in Sekunden
        float timeIntervalSinceLastFrame = (getCurrentTimeInMs() - lastFrameTime) / 1000.0f;
        // GL-Buffer leeren
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // rendern starten
        QCAR::State state = QCAR::Renderer::getInstance().begin();
        // Flag setzen, ob ein Trackable sichtbar ist
        trackableVisible = state.getNumActiveTrackables() > 0;
        // Prüfung, ob Trackable sichtbar ist
        if (trackableVisible) {
            // Wenn der aktuelle Spiel-Zustand, der Zustand ist, in dem auf das Trackable gewartet
            // wird, Spiel-Zustand updaten und Hinweis anzeigen lassen, wie das Spielfeld erzeugt
            // werden kann.
            if (gameState == GAME_STATE_WAITING_FOR_MARKER) {
                updateGameState(GAME_STATE_SHOW_HINT_CREATE_SPACES);
            }
            // das erste Trackable auslesen
            const QCAR::Trackable *trackable = state.getActiveTrackable(0);
            // Model-View-Matrix berechnen
            modelViewMatrix = QCAR::Tool::convertPose2GLMatrix(trackable->getPose());
            // Prüfung, ob im aktuellen Spiel-Zustand das Spielfeld erzeugt wird
            if (gameState == GAME_STATE_CREATING_FIELD) {
                // Touch-Event verarbeiten
                handleTouchEvent();
            }
            // Spielzug verarbeiten
            processMove(timeIntervalSinceLastFrame);
            // Realitätserweiterung rendern
            renderAugmentation();
        }
        // rendern beenden
        QCAR::Renderer::getInstance().end();
        // Zeit des Aufrufs speichern
        lastFrameTime = getCurrentTimeInMs();
    }

    /*
     * Die nachfolgenden Funktionen sind für die Manipulation des User Interfaces auf der Java-Ebene
     * zuständig. Der Ablauf ist immer gleich:
     *
     * - Der aktuelle Thread wird an die gespeicherte Java-VM angehängt, um Thread-Konflikte zu
     * vermeiden.
     * - Die benötigte Methode wird in der gespeicherten Java-Klasse gesucht und mit entpsrechenden
     * Parametern aufgerufen.
     */

    /**
     * Zeigt eine View an.
     *
     * @param name View-Name.
     */
    void showView(const char *name) {
        LOGD("InvisibogaGlSurfaceView.showView");
        JNIEnv *javaEnvironment;
        javaVm->AttachCurrentThread(&javaEnvironment, NULL);
        jmethodID method = javaEnvironment->GetMethodID(javaClass, "showView",
                "(Ljava/lang/String;)V");
        if (method != NULL) {
            jstring javaString = javaEnvironment->NewStringUTF(name);
            javaEnvironment->CallVoidMethod(javaObject, method, javaString);
        } else {
            LOGE("showView() not found");
        }
    }

    /**
     * Versteckt eine View.
     *
     * @param name View-Name.
     */
    void hideView(const char *name) {
        LOGD("InvisibogaGlSurfaceView.hideView");
        JNIEnv *javaEnvironment;
        javaVm->AttachCurrentThread(&javaEnvironment, NULL);
        jmethodID method = javaEnvironment->GetMethodID(javaClass, "hideView",
                "(Ljava/lang/String;)V");
        if (method != NULL) {
            jstring javaString = javaEnvironment->NewStringUTF(name);
            javaEnvironment->CallVoidMethod(javaObject, method, javaString);
        } else {
            LOGE("hideView() not found");
        }
    }

    /**
     * Zeigt einen Toast an.
     *
     * @param text Inhalt des Toasts.
     * @param duration Dauer des Toasts, entweder TOAST_DURATION_SHORT oder TOAST_DURATION_LONG.
     */
    void showToast(const char *text, int duration) {
        LOG("showToast");
        JNIEnv* javaEnvironment;
        javaVm->AttachCurrentThread(&javaEnvironment, NULL);
        jstring javaString = javaEnvironment->NewStringUTF(text);
        jmethodID method = javaEnvironment->GetMethodID(javaClass, "showToast",
                "(Ljava/lang/CharSequence;I)V");
        LOGI("GetMethodID returned %d", method);
        if (method != NULL) {
            javaEnvironment->CallVoidMethod(javaObject, method, javaString, duration);
        } else {
            LOGE("showToast() not found");
        }
    }

    /**
     * Setzt die Text-Farbe der currentPlayer-TextView.
     *
     * @param alpha Alpha.
     * @param red Rot.
     * @param green Grün.
     * @param blue Blau.
     */
    void setCurrentPlayerTextViewFontColor(int alpha, int red, int green, int blue) {
        LOGD("setCurrentPlayerTextViewFontColor(alpha: %d, red: %d, green: %d, blue: %d)", alpha,
                red, green, blue);
        JNIEnv* javaEnvironment;
        javaVm->AttachCurrentThread(&javaEnvironment, NULL);
        jmethodID method = javaEnvironment->GetMethodID(javaClass,
                "setCurrentPlayerTextViewFontColor", "(IIII)V");
        if (method != NULL) {
            javaEnvironment->CallVoidMethod(javaObject, method, alpha, red, green, blue);
        } else {
            LOGE("setCurrentPlayerTextViewFontColor not found");
        }
    }

    /**
     * Schreibt den Namen und den Typen in der entsprechnenden Farbe des aktuellen Spieler in die
     * currentPlayer-TextView.
     *
     * @param player Der aktuelle Spieler.
     */
    void setCurrentPlayer(Player *player) {
        LOGD("setCurrentPlayer");
        JNIEnv* javaEnvironment;
        javaVm->AttachCurrentThread(&javaEnvironment, NULL);
        // Spieler-Namen und -Typ in einen String schreiben
        char nameAndType[256];
        sprintf(nameAndType, "%s (%s)", player->name,
                player->type == PLAYER_TYPE_COMPUTER ? "Computer" : "Mensch");
        jstring javaString = javaEnvironment->NewStringUTF(nameAndType);
        jmethodID method = javaEnvironment->GetMethodID(javaClass, "setCurrentPlayerText",
                "(Ljava/lang/CharSequence;)V");
        if (method != NULL) {
            // Methode zum Setzen des currentPlayer-TextView aufrufen
            javaEnvironment->CallVoidMethod(javaObject, method, javaString);
            // Farbe setzen
            // Es wird der ambiente Anteil der Spielfigur verwendet. Dabei werden die Farben von
            // float in int umgerechnet.
            setCurrentPlayerTextViewFontColor((int)player->pawn.light.ambient[3] * 255,
                    (int)player->pawn.light.ambient[0] * 255,
                    (int)player->pawn.light.ambient[1] * 255,
                    (int)player->pawn.light.ambient[2] * 255);
        } else {
            LOGE("setCurrentPlayerText() method not found");
        }
    }

    /**
     * Initialisiert das Rendern.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_initRendering(
            JNIEnv* env, jobject obj) {
        LOG("Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_initRendering");
        glClearColor(0.0f, 0.0f, 0.0f, QCAR::requiresAlpha() ? 0.0f : 1.0f);
        // OpenGL-Texturen erzeugen und Parameter setzen
        for (int i = 0; i < textureCount; ++i) {
            glGenTextures(1, &(textures[i]->mTextureID));
            glBindTexture(GL_TEXTURE_2D, textures[i]->mTextureID);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[i]->mWidth, textures[i]->mHeight, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*)textures[i]->mData);
        }
    }

    /**
     * Führt ein Render-Update durch. Das ist zum Beispiel nötig, nachdem die App durch das OS oder
     * vom Benutzer pausiert wurde und fortgesetzt wird.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_updateRendering(
            JNIEnv* env, jobject obj, jint width, jint height) {
        LOG("Java_de_fhwedel_vr2_invisiboga_InvisibogaGlSurfaceView_updateRendering");
        // Display-Größe aktualisieren
        screenWidth = width;
        screenHeight = height;
        // Video-Hintergrund rekonfigurieren
        configureVideoBackground();
    }

#ifdef __cplusplus
}
#endif
