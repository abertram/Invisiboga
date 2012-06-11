/**
 * @file invisiboga.cpp
 *
 * @brief Beinhaltet Funktionen, die aus der Invisiboga.java aufgerufen werden.
 *
 * @author Alexander Bertram
 *
 * @date 17.06.2011
 */

#include <jni.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include <android/log.h>

#include <GLES/gl.h>
#include <GLES/glext.h>

#include <QCAR/CameraDevice.h>
#include <QCAR/QCAR.h>
#include <QCAR/Renderer.h>
#include <QCAR/Tool.h>
#include <QCAR/Tracker.h>
#include <QCAR/Trackable.h>
#include <QCAR/VideoBackgroundConfig.h>

#include "constants.h"
#include "field.h"
#include "game.h"
#include "invisibogaGlSurfaceView.h"
#include "mathUtils.h"
#include "pawn.h"
#include "utils.h"
#include "Texture.h"
#include "timeUtilities.h"
#include "types.h"

// folgender Code ist C-Code
#ifdef __cplusplus
extern "C" {
#endif

    /**
     * Inverse Projektionsmatrix.
     */
    QCAR::Matrix44F inverseProjectionMatrix;
    /**
     * Zeitpunkt der letzten Frame-Berechnung (verwaltet in invisibogaGlSurfaceView.cpp).
     */
    extern unsigned long lastFrameTime;
    /**
     * Zeitpunkt des letzten Tippens.
     */
    unsigned long lastTapTime;
    /**
     * Model-View-Matrix (verwaltet in invisibogaGlSurfaceView.cpp).
     */
    extern QCAR::Matrix44F modelViewMatrix;
    /**
     * Projektionsmatrix.
     */
    QCAR::Matrix44F projectionMatrix;
    /**
     * Bildschirm-Breite.
     */
    unsigned int screenWidth = 0;
    /**
     * Bildschrim-Höhe.
     */
    unsigned int screenHeight = 0;
    /**
     * Anzahl der Texturen.
     */
    int textureCount = 0;
    /**
     * Texturen.
     */
    Texture **textures = NULL;
    /**
     * Touch-Ereignis.
     */
    TouchEvent touchEvent;
    /*
     * Flag zur Prüfung, ob ein Trackable sichtbar ist (verwaltet in invisibogaGlSurfaceView.cpp).
     */
    extern bool trackableVisible;
    /*
     * Spieler (verwaltet in game.cpp).
     */
    extern Player players[];
    /*
     * Spielfeld (verwaltet in field.cpp).
     */
    extern Field field;
    /*
     * Aktueller Spiel-Zustand, verwaltet in game.cpp.
     */
    extern int gameState;

    /**
     * Führt Initialisierungen durch, nachdem das QCAR-Framework erfogreich initialisiert wurde.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_onQCARInitializedNative(
            JNIEnv*, jobject) {
        LOG("Java_de_fhwedel_vr2_invisiboga_Invisiboga_onQCARInitializedNative");
        // Spielfeld-Länge initialisieren
        field.length = 0;
        // Zeitpunkt des letzten Frames initialisieren
        lastFrameTime = getCurrentTimeInMs();
        // Spiel initialsieren
        initGame();
        // Zufallsgenerator initialisieren
        srand(time(NULL));
    }

    /**
     * Verarbeitet das Touch-Event.
     */
    void handleTouchEvent() {
        // Prüfung, ob ein Tipp auf den Touchscreen erkannt und dieser noch nicht verarbeitet wurde
        if (touchEvent.isTap && touchEvent.startTime > lastTapTime) {
            // Es wurde auf den Touchscreen getippt. Es wird versucht, an der Stelle ein Feld zu
            // erzeugen.

            QCAR::Vec3F intersection;
            // Projektion der Koordinaten, auf die getippt wurde, auf die Ebene des getrackten
            // Objektes
            projectScreenPointToPlane(screenWidth, screenHeight,
                    QCAR::Vec2F(touchEvent.tapPosition.data[0], touchEvent.tapPosition.data[1]),
                    inverseProjectionMatrix, modelViewMatrix, QCAR::Vec3F(0, 0, 0),
                    QCAR::Vec3F(0, 0, 1), intersection);
            // Prüfung, ob sich an den projezierten Koordinaten bereits ein Feld befindet
            Space* space = selectSpace(QCAR::Vec2F(intersection.data[0], intersection.data[1]));
            if (space != NULL) {
                // Feld gefunden

                // normales Feld in Sonderfeld umwandeln und umgekehrt
                if (space->type == SPACE) {
                    space->type = SPECIAL_SPACE;
                } else if (space->type == SPECIAL_SPACE) {
                    space->type = SPACE;
                }
            } else if (canCreateSpace(QCAR::Vec2F(intersection.data[0], intersection.data[1]))) {
                // kein Feld gefunden, es kann ein neues erstellt werden

                // Feld erzeugen
                createSpace(QCAR::Vec2F(intersection.data[0], intersection.data[1]));
                // Weiter-Button beim Erreichen einer bestimmten Anzahl von Feldern anzeigen
                if (field.length == SPACE_COUNT_TO_SHOW_NEXT_BUTTON) {
                    showView("nextButton");
                }
                // Neustart-Button beim Erreichen einer bestimmten Anzahl von Feldern anzeigen
                if (field.length == SPACE_COUNT_TO_SHOW_RESTART_BUTTON) {
                    showView("restartButton");
                }
            }
            // Zeit des Verarbeitung manipulieren, damit dieser Tipp nicht mehr verarbeitet wird
            lastTapTime = touchEvent.startTime;
        } else if (touchEvent.isActive) {
            // Der User hat den Finger immer noch auf dem Touchscreen. Wenn der Finger über den
            // Bildschrim gezogen wird, dann wird versucht auf diesem Zieh-Pfad nacheinander Felder
            // zu erzeugen.

            // Touch-Ereignis ignorieren, wenn daraus noch ein Tipp werden könnte
            if (touchEvent.lifeTime > MAX_TAP_TIME
                    || touchEvent.startPositionLastPositionDistance > MAX_TAP_SQUARED_DISTANCE) {
                QCAR::Vec3F intersection;
                // Projektion der Koordinaten, auf denen sich der Finger befindet, auf die Ebene des
                // getrackten Objektes
                projectScreenPointToPlane(
                        screenWidth,
                        screenHeight,
                        QCAR::Vec2F(touchEvent.currentPosition.data[0],
                                touchEvent.currentPosition.data[1]), inverseProjectionMatrix,
                        modelViewMatrix, QCAR::Vec3F(0, 0, 0), QCAR::Vec3F(0, 0, 1), intersection);
                // Prüfung, ob an den projezierten Koordinaten ein Feld erzeugt werden kann
                if (canCreateSpace(QCAR::Vec2F(intersection.data[0], intersection.data[1]))) {
                    // Feld erzeugen
                    createSpace(QCAR::Vec2F(intersection.data[0], intersection.data[1]));
                    // Weiter-Button beim Erreichen einer bestimmten Anzahl von Feldern anzeigen
                    if (field.length == SPACE_COUNT_TO_SHOW_NEXT_BUTTON) {
                        showView("nextButton");
                    }
                    // Neustart-Button beim Erreichen einer bestimmten Anzahl von Feldern anzeigen
                    if (field.length == SPACE_COUNT_TO_SHOW_RESTART_BUTTON) {
                        showView("restartButton");
                    }
                }
            }
        }
    }

    /**
     * Wird aus dem Java-Code aufgerufen, wenn ein Touch-Ereignis aufgetreten ist.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     * @param actionType Aktion des Touch-Ereignisses.
     * @param x x-Koordinate des Touch-Ereignisses.
     * @param y y-Koordinate des Touch-Ereignisses.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_nativeTouchEvent(JNIEnv*,
            jobject, jint actionType, jfloat x, jfloat y) {
        // Touch-Ereignisse nur behandeln, wenn ein Trackable sichtbar ist
        if (trackableVisible) {
            // Prüfung, ob das Ereignis den Start einer Druck-Geste ist
            if (actionType == ACTION_DOWN) {
                // Start-Parameter speichern
                touchEvent.lastPosition.data[0] = x;
                touchEvent.lastPosition.data[1] = y;
                touchEvent.startPosition.data[0] = x;
                touchEvent.startPosition.data[1] = y;
                touchEvent.startTime = getCurrentTimeInMs();
                touchEvent.isTap = false;
            } else {
                // letzte Position speichern
                touchEvent.lastPosition = touchEvent.currentPosition;
            }
            // Alter und Distanz berechnen, um das Tippen auf dem Bildschirm erkennen zu können
            unsigned long time = getCurrentTimeInMs();
            touchEvent.lifeTime = time - touchEvent.startTime;
            touchEvent.startPositionLastPositionDistance = vec2FDistance(touchEvent.lastPosition,
                    touchEvent.startPosition);
            // Prüfung, ob das Touch-Ereignis das Ende einer Druck-Geste beschreibt
            if (actionType == ACTION_UP) {
                touchEvent.isActive = false;
                // Wenn die Dauer der Geste und die zurückgelegte Distanz bestimmte Schwellwerte
                // nicht überschreiten, war es ein Tipp.
                if (touchEvent.lifeTime < MAX_TAP_TIME
                        && touchEvent.startPositionLastPositionDistance < MAX_TAP_SQUARED_DISTANCE) {
                    touchEvent.isTap = true;
                    touchEvent.tapPosition = touchEvent.startPosition;
                }
            } else {
                touchEvent.isActive = true;
            }
            // aktuelle Paramter speichern
            touchEvent.actionType = (ActionType)actionType;
            touchEvent.currentPosition.data[0] = x;
            touchEvent.currentPosition.data[1] = y;
        }
    }

    /**
     * Deinitialisiert die Anwendung.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_deinitNative(JNIEnv *,
            jobject) {
        LOGD("Java_de_fhwedel_vr2_invisiboga_Invisiboga_deinitNative");
        // Texturen löschen und freigeben
        if (textures != NULL) {
            for (int i = 0; i < textureCount; i++) {
                delete textures[i];
                textures[i] = NULL;
            }
            delete[] textures;
            textures = NULL;
            textureCount = 0;
        }
    }

    /**
     * Initialisiert die Anwendung.
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     * @param width Bildschirm-Breite.
     * @param height Bildschrim-Höhe.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_initApplicationNative(
            JNIEnv* env, jobject obj, jint width, jint height) {
        LOG("Java_de_fhwedel_vr2_invisiboga_Invisiboga_initApplicationNative");
        // Display-Größe speichern
        screenWidth = width;
        screenHeight = height;
        // Anzahl der Texturen ermitteln
        jclass activityClass = env->GetObjectClass(obj);
        jmethodID getTextureCountMethodID = env->GetMethodID(activityClass, "getTextureCount",
                "()I");
        if (getTextureCountMethodID == NULL) {
            LOGE("Function getTextureCount() not found.");
            return;
        }
        textureCount = env->CallIntMethod(obj, getTextureCountMethodID);
        if (!textureCount) {
            LOGE("getTextureCount() returned zero.");
            return;
        }
        // Texturen erzeugen
        textures = new Texture*[textureCount];
        jmethodID getTextureMethodId = env->GetMethodID(activityClass, "getTexture",
                "(I)Lde/fhwedel/vr2/invisiboga/Texture;");
        if (getTextureMethodId == 0) {
            LOGE("Function getTexture() not found.");
            return;
        }
        for (int i = 0; i < textureCount; ++i) {
            jobject textureObject = env->CallObjectMethod(obj, getTextureMethodId, i);
            if (textureObject == NULL) {
                LOG("getTexture() returned zero pointer");
                return;
            }
            textures[i] = Texture::create(env, textureObject);
        }
    }

    /**
     * Konfiguriert den Video-Hintergrund (aus QCAR-Beispielen übernommen).
     */
    void configureVideoBackground() {
        LOG("configureVideoBackground");
        // Default-Videomodus auslesen
        QCAR::CameraDevice& cameraDevice = QCAR::CameraDevice::getInstance();
        QCAR::VideoMode videoMode = cameraDevice.getVideoMode(QCAR::CameraDevice::MODE_DEFAULT);
        // Video-Hintergrund konfigurieren
        QCAR::VideoBackgroundConfig config;
        config.mEnabled = true;
        config.mSynchronous = true;
        config.mPosition.data[0] = 0.0f;
        config.mPosition.data[1] = 0.0f;
        config.mSize.data[0] = screenWidth;
        config.mSize.data[1] = videoMode.mHeight * (screenWidth / (float)videoMode.mWidth);
        // Video-Hintergrund setzen
        QCAR::Renderer::getInstance().setVideoBackgroundConfig(config);
    }

    /**
     * Startet die Kamera (aus QCAR-Beispielen übernommen).
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_startCamera(JNIEnv*, jobject) {
        LOG("Java_de_fhwedel_vr2_invisiboga_Invisiboga_startCamera");
        // Kamera initialisieren
        if (!QCAR::CameraDevice::getInstance().init()) {
            return;
        }
        // Video-Hintergrund konfigurieren
        configureVideoBackground();
        // Kamera-Modus auf Default setzen
        if (!QCAR::CameraDevice::getInstance().selectVideoMode(QCAR::CameraDevice::MODE_DEFAULT)) {
            return;
        }
        // Kamera starten
        if (!QCAR::CameraDevice::getInstance().start()) {
            return;
        }
        // Tracker starten
        QCAR::Tracker::getInstance().start();
        // Projektionsmatrix und inverse Projektionsmatrix setzen
        const QCAR::Tracker& tracker = QCAR::Tracker::getInstance();
        const QCAR::CameraCalibration& cameraCalibration = tracker.getCameraCalibration();
        projectionMatrix = QCAR::Tool::getProjectionGL(cameraCalibration, 20.0f, 5000.0f);
        inverseProjectionMatrix = matrix44FInverse(projectionMatrix);
    }

    /**
     * Stoppt die Kamera (aus QCAR-Beispielen übernommen).
     *
     * @param env Die JNI-Umgebung.
     * @param obj Das aufrufende Objekt.
     */
    JNIEXPORT void JNICALL Java_de_fhwedel_vr2_invisiboga_Invisiboga_stopCamera(JNIEnv*, jobject) {
        LOG("Java_de_fhwedel_vr2_invisiboga_Invisiboga_stopCamera");
        QCAR::CameraDevice::getInstance().stop();
        QCAR::CameraDevice::getInstance().deinit();
    }

#ifdef __cplusplus
}
#endif
