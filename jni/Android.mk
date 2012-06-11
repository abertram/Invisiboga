# Makefile für das Android-NDK

# lokaler Pfad, unter dem die Sourcen zu finden sind
LOCAL_PATH := $(call my-dir)
# lokale Variablen (ausser LOCAL_PATH) löschen
include $(CLEAR_VARS)

# 1. Modul: native QCAR-Bibliothek im Ausgabe-Verzeichnis erstellen
LOCAL_MODULE := QCAR-prebuilt
LOCAL_SRC_FILES = ../dks/qcar-android-1-0-6/build/lib/$(TARGET_ARCH_ABI)/libQCAR.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../dks/qcar-android-1-0-6/build/include
include $(PREBUILT_SHARED_LIBRARY)

# lokale Variablen (ausser LOCAL_PATH) löschen
include $(CLEAR_VARS)

# 2. Modul: native Invisiboga-Bibliothes erstellen
LOCAL_MODULE := Invisiboga
TARGET_PLATFORM := android-8
OPENGLES_LIB  := -lGLESv1_CM
OPENGLES_DEF  := -DUSE_OPENGL_ES_1_1
LOCAL_CFLAGS := -Wno-write-strings $(OPENGLES_DEF)
LOCAL_LDLIBS := -llog $(OPENGLES_LIB)
LOCAL_SHARED_LIBRARIES := QCAR-prebuilt
LOCAL_SRC_FILES := field.cpp game.cpp invisiboga.cpp invisibogaGlSurfaceView.cpp mathUtils.cpp \
	overlayView.cpp pawn.cpp player.cpp renderer.cpp Texture.cpp timeUtilities.cpp utils.cpp
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)
