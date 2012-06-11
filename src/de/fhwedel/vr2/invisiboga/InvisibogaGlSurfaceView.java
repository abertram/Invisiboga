package de.fhwedel.vr2.invisiboga;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import android.content.Context;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Bundle;
import android.os.Message;

import com.qualcomm.QCAR.QCAR;

/**
 * Stellt einen Zeichenbereich für OpenGL-Inhalte, die im nativen Code erzeugt
 * werden, zur Verfügung. Sie ist zuständig für die Initialisierung des
 * Zeichenbereichs und die Behandlung der im Renderer-Interface ausgelösten
 * Ereignisse. Bei der Behandlung dieser Ereignisse werden die entsprechenden
 * Ereignisse im nativen Code und im QCAR-Framework ausgelöst.<br>
 * <br>
 * Die Initialisierung wurde aus den QCAR-Beispielen übernommen und an einigen
 * Stellen minimal modifiziert.<br>
 * <br>
 * Für das Rendern des OpenGL-Inhaltes wird ein eigener Thread erzeugt, der
 * GL-Thread. Das User Interface kann nur aus dem Haupt-Thread manipuliert
 * werden. Die onDrawFrame-Methode und somit die von ihr aufgerufene native
 * renderFrame-Methode läuft innerhalb des GL-Threads ab. Soll also das User
 * Interface im Kontext der renderFrame-Methode manipuliert werden, wird eine
 * Thread-Kommunikation über Nachrichten notwendig.
 * 
 * @author QCAR, Alexander Bertram
 * 
 */
public class InvisibogaGlSurfaceView extends GLSurfaceView implements Renderer {
    /**
     * Instanz der OverlayView, die für die Anzeige des User Interfaces
     * zuständig ist.
     */
    private OverlayView mOverlayView;

    /**
     * Konstruktor.
     * 
     * @param context
     *            Kontext, in dem die View erzeugt wird.
     * @param overlayView
     *            View, die für die Anzeige des User Interfaces zuständig ist.
     */
    public InvisibogaGlSurfaceView(Context context, OverlayView overlayView) {
        super(context);
        mOverlayView = overlayView;
    }

    /**
     * Initialisierung (aus QCAR-Beispielen übernommen).
     */
    public void init(int flags, boolean translucent, int depth, int stencil) {
        Log.i("Using " + (translucent ? "translucent" : "opaque") + " GLView, depth buffer size: "
            + depth + ", stencil size: " + stencil);
        if (translucent) {
            getHolder().setFormat(PixelFormat.TRANSLUCENT);
        }
        setEGLContextFactory(new ContextFactory());
        setEGLConfigChooser(translucent ? new ConfigChooser(8, 8, 8, 8, depth, stencil)
            : new ConfigChooser(5, 6, 5, 0, depth, stencil));
        setRenderer(this);
        // native Initialisierung
        nativeInit();
    }

    /**
     * ContextFactory für den OpenGL-Context (aus QCAR-Beispielen übernommen).
     */
    private static class ContextFactory implements EGLContextFactory {
        private static int EGL_CONTEXT_CLIENT_VERSION = 0x3098;

        public EGLContext createContext(EGL10 egl, EGLDisplay display, EGLConfig eglConfig) {
            EGLContext context;
            Log.i("Creating OpenGL ES 1.x context");
            checkEglError("Before eglCreateContext", egl);
            int[] attrib_list_gl1x = { EGL_CONTEXT_CLIENT_VERSION, 1, EGL10.EGL_NONE };
            context = egl.eglCreateContext(display, eglConfig, EGL10.EGL_NO_CONTEXT,
                attrib_list_gl1x);
            checkEglError("After eglCreateContext", egl);
            return context;
        }

        public void destroyContext(EGL10 egl, EGLDisplay display, EGLContext context) {
            egl.eglDestroyContext(display, context);
        }
    }

    /**
     * Prüft, ob OpenGL-Fehler aufgetreten sind (aus QCAR-Beispielen
     * übernommen).
     */
    private static void checkEglError(String prompt, EGL10 egl) {
        int error;
        while ((error = egl.eglGetError()) != EGL10.EGL_SUCCESS) {
            Log.e(String.format("%s: EGL error: 0x%x", prompt, error));
        }
    }

    /**
     * Klasse zur Auswahl einer bestimmten OpenGL-Konfiguration aus einer Liste
     * von möglichen OpenGL-Konfigurationen (aus QCAR-Beispielen übernommen).
     */
    private static class ConfigChooser implements EGLConfigChooser {
        protected int mRedSize;
        protected int mGreenSize;
        protected int mBlueSize;
        protected int mAlphaSize;
        protected int mDepthSize;
        protected int mStencilSize;
        private int[] mValue = new int[1];

        public ConfigChooser(int r, int g, int b, int a, int depth, int stencil) {
            mRedSize = r;
            mGreenSize = g;
            mBlueSize = b;
            mAlphaSize = a;
            mDepthSize = depth;
            mStencilSize = stencil;
        }

        private EGLConfig getMatchingConfig(EGL10 egl, EGLDisplay display, int[] configAttribs) {
            int[] num_config = new int[1];
            egl.eglChooseConfig(display, configAttribs, null, 0, num_config);
            int numConfigs = num_config[0];
            if (numConfigs <= 0) {
                throw new IllegalArgumentException("No matching EGL configs");
            }
            EGLConfig[] configs = new EGLConfig[numConfigs];
            egl.eglChooseConfig(display, configAttribs, configs, numConfigs, num_config);
            return chooseConfig(egl, display, configs);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display) {
            final int EGL_OPENGL_ES1X_BIT = 0x0001;
            final int[] s_configAttribs_gl1x = { EGL10.EGL_RED_SIZE, 5, EGL10.EGL_GREEN_SIZE, 6,
                EGL10.EGL_BLUE_SIZE, 5, EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES1X_BIT,
                EGL10.EGL_NONE };
            return getMatchingConfig(egl, display, s_configAttribs_gl1x);
        }

        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display, EGLConfig[] configs) {
            for (EGLConfig config : configs) {
                int d = findConfigAttrib(egl, display, config, EGL10.EGL_DEPTH_SIZE, 0);
                int s = findConfigAttrib(egl, display, config, EGL10.EGL_STENCIL_SIZE, 0);
                if (d < mDepthSize || s < mStencilSize) {
                    continue;
                }
                int r = findConfigAttrib(egl, display, config, EGL10.EGL_RED_SIZE, 0);
                int g = findConfigAttrib(egl, display, config, EGL10.EGL_GREEN_SIZE, 0);
                int b = findConfigAttrib(egl, display, config, EGL10.EGL_BLUE_SIZE, 0);
                int a = findConfigAttrib(egl, display, config, EGL10.EGL_ALPHA_SIZE, 0);
                if (r == mRedSize && g == mGreenSize && b == mBlueSize && a == mAlphaSize) {
                    return config;
                }
            }
            return null;
        }

        private int findConfigAttrib(EGL10 egl, EGLDisplay display, EGLConfig config,
            int attribute, int defaultValue) {
            if (egl.eglGetConfigAttrib(display, config, attribute, mValue)) {
                return mValue[0];
            }
            return defaultValue;
        }
    }

    /**
     * Wird aufgerufen, wenn die Oberfläche (wieder)erzeugt wurde.<br>
     * <br>
     * Löst entsprechende Ereignise im nativen Code und im QCAR-Framework aus.
     */
    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        Log.d("Renderer.onSurfaceCreated");
        initRendering();
        QCAR.onSurfaceCreated();
    }

    /**
     * Wird aufgerufen, wenn sich die Größe der Oberfläche verändert hat.<br>
     * <br>
     * Löst entsprechende Ereignise im nativen Code und im QCAR-Framework aus.
     */
    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        Log.d("Renderer.onSurfaceChanged");
        updateRendering(width, height);
        QCAR.onSurfaceChanged(width, height);
    }

    /**
     * Wird aufgerufen, um ein Frame zu zeichnen.<br>
     * <br>
     * Der Frame wird im nativen Code erzeugt.
     */
    @Override
    public void onDrawFrame(GL10 gl) {
        renderFrame();
    }

    private native void nativeInit();

    public native void initRendering();

    public native void updateRendering(int width, int height);

    public native void renderFrame();

    /**
     * Die folgenden Methoden sind nötig, um innerhalb der nativen
     * renderFrame-Methode das User Interface zu manipulieren. Der Ablauf ist
     * immer der Gleiche: Die Methode wird aus dem nativen Code aufgerufen. Als
     * erstes wird eine Nachricht-Instanz aus dem globalen Nachrichten-Pool
     * geholt. Dann wird der Handler, der diese Nachricht verarbeiten soll/kann,
     * die Art der Nachricht und der Nachrichten-Inhalt gesetzt. Als letztes
     * wird die Nachricht abgeschickt.
     */

    /**
     * Zeigt eine View an.
     * 
     * @param name
     *            Name der anzuzeigenden View.
     */
    private void showView(String name) {
        Message.obtain(mOverlayView.mHandler, OverlayViewHandler.SHOW_VIEW, name).sendToTarget();
    }

    /**
     * Versteckt eine View.
     * 
     * @param name
     *            Name der zu versteckenden View.
     */
    private void hideView(String name) {
        Message.obtain(mOverlayView.mHandler, OverlayViewHandler.HIDE_VIEW, name).sendToTarget();
    }

    /**
     * Zeigt eine Nachricht auf dem Bildschirm an.
     * 
     * @param text
     *            Inhalt der Nachricht.
     * @param duration
     *            Dauer der Nachricht.
     */
    private void showToast(CharSequence text, int duration) {
        Message.obtain(mOverlayView.mHandler, OverlayViewHandler.SHOW_TOAST, duration, 0, text)
            .sendToTarget();
    }

    /**
     * Setzt Spieler-Daten (z.B. Name und Spieler-Typ) des aktuellen Spielers,
     * um diese auf dem Bildschirm anzeigen zu können.
     * 
     * @param text
     *            Der anzuzeigende Text.
     */
    private void setCurrentPlayerText(CharSequence text) {
        Log.d("InvisibogaGlSurfaceView.setCurrentPlayerText");
        Message.obtain(mOverlayView.mHandler, OverlayViewHandler.SET_CURRENT_PLAYER_TEXT_VIEW_TEXT,
            text).sendToTarget();
    }

    /**
     * Setzt die Text-Farbe der View, in der die Spieler-Daten angezeigt werden.
     * 
     * @param alpha
     *            Alpha.
     * @param red
     *            Rot.
     * @param green
     *            Grün.
     * @param blue
     *            Blau.
     */
    protected void setCurrentPlayerTextViewFontColor(int alpha, int red, int green, int blue) {
        Log.d("InvisibogaGlSurfaceView.setCurrentPlayerTextViewFontColor(alpha: %d, red: %d, " +
                "green: %d, blue: %d)", alpha,
            red, green, blue);
        Message message = Message.obtain(mOverlayView.mHandler,
            OverlayViewHandler.SET_CURRENT_PLAYER_TEXT_VIEW_FONT_COLOR);
        Bundle data = new Bundle();
        data.putInt("alpha", alpha);
        data.putInt("red", red);
        data.putInt("green", green);
        data.putInt("blue", blue);
        message.setData(data);
        message.sendToTarget();
    }
}
