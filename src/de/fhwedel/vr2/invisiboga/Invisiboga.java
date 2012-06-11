package de.fhwedel.vr2.invisiboga;

import java.util.Vector;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.pm.ActivityInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.WindowManager;

import com.qualcomm.QCAR.QCAR;

/**
 * Einstiegspunkt der Applikation.<br>
 * <br>
 * Von hier aus wird der Initialisierungs- und der Deinitalisierungsprozess
 * sowohl auf der Java als auch auf der nativen Ebene angestoßen. Zusätzlich
 * wird hier definiert, was passiert, wenn die App von aussen pausiert und
 * wieder fortgesetzt wird.<br>
 * <br>
 * Der App-Ablauf wird in einem Zustandsautomaten gesteuert. Nach der
 * Initialisierung bzw. beim Fortsetzenbefindet sich der Zustandsautomat im
 * Zustand APP_STATUS_CAMERA_RUNNING. Beim Pausieren wird dieser in den Zustand
 * APP_STATUS_CAMERA_STOPPED überführt.
 * 
 * @author Alexander Bertram
 * 
 */
public class Invisiboga extends Activity {
    /**
     * Start-Zustand.
     */
    private static final int APP_STATUS_UNINITED = -1;
    /**
     * Zustand zur Initialisierung von Komponenten, die nichts mit QCAR oder
     * Augmented Reality zu tun haben (z.B. Views erzeugen).
     */
    private static final int APP_STATUS_INIT_APP = 0;
    /**
     * Zustand zur QCAR-Framework-Initialiserung.
     */
    private static final int APP_STATUS_INIT_QCAR = 1;
    /**
     * Zustand zur Initialisierung der Augmented Realitiy-Komponenten.
     */
    private static final int APP_STATUS_INIT_AUGMENTED_REALITY = 2;
    /**
     * Zustand zur Tracker-Initialisierung.
     */
    private static final int APP_STATUS_INIT_TRACKER = 3;
    /**
     * Zustand, der anzeigt, dass die App-Initialisierung zu Ende ist.
     */
    private static final int APP_STATUS_INITED = 4;
    /**
     * Zustand zum Einschalten der Kamera.
     */
    private static final int APP_STATUS_START_CAMERA = 5;
    /**
     * Zustand, der anzeigt, dass die Kamera eingeschaltet ist.
     */
    private static final int APP_STATUS_CAMERA_RUNNING = 5;
    /**
     * Zustand zum Ausschalten der Kamera.
     */
    private static final int APP_STATUS_STOP_CAMERA = 6;
    /**
     * Zustand, der anzeigt, dass die Kamera ausgeschaltet ist.
     */
    private static final int APP_STATUS_CAMERA_STOPPED = 6;
    /**
     * Name der nativen QCAR-Framework-Bibliothek, die beim Start geladen wird.
     */
    private static final String NATIVE_LIB_QCAR = "QCAR";
    /**
     * Name der nativen Bibliothek, die für die Verarbeitung auf der nativen
     * Ebene zuständig ist.
     */
    private static final String NATIVE_LIB_INVISIBOGA = "Invisiboga";

    /**
     * Aktueller App-Status.
     */
    private int mAppStatus = APP_STATUS_UNINITED;
    /**
     * Texturen.
     */
    private Vector<Texture> mTextures;
    /**
     * Instanz des asynchronen Tasks zur Initialisierung des QCAR-Frameworks.
     */
    private InitQcarTask mInitQcarTask;
    /**
     * Instanz des asynchronen Tasks zum Laden der Tracker-Daten.
     */
    private LoadTrackerTask mLoadTrackerTask;
    /**
     * Bildschirm-Breite.
     */
    private int mScreenWidth;
    /**
     * Bildschirm-Höhe.
     */
    private int mScreenHeight;
    /**
     * Instanz der InvisibogaGlSurfaceView.
     */
    private InvisibogaGlSurfaceView mGlSurfaceView;
    /**
     * Instanz des Lade-Fortschritt-Dialogs, der beim App-Start angezeigt wird.
     */
    private ProgressDialog loadProgressDialog;
    /**
     * Instanz der OverlayView, auf der die UI-Elemente platziert werden.
     */
    private OverlayView mOverlayView;

    // Beim Start sollen die nativen Bibliotheken geladen werden.
    static {
        loadNativeLibrary(NATIVE_LIB_QCAR);
        loadNativeLibrary(NATIVE_LIB_INVISIBOGA);
    }

    /**
     * Asynchroner Tasks zur Initialisierung des QCAR-Frameworks.
     */
    private class InitQcarTask extends AsyncTask<Void, Integer, Boolean> {
        private int mProgressValue = -1;

        /**
         * Initialisiert das QCAR-Framework im Hintergrund.
         */
        @Override
        protected Boolean doInBackground(Void... params) {
            // Das QCAR-Framework soll mit der Invisiboga-Activity und OpenGL
            // 1.1 initialisiert werden.
            QCAR.setInitParameters(Invisiboga.this, QCAR.GL_11);
            do {
                mProgressValue = QCAR.init();
            } while (!isCancelled() && mProgressValue >= 0 && mProgressValue < 100);
            return (mProgressValue > 0);
        }

        /**
         * Wird nach doInBackground ausgeführt.
         */
        @Override
        protected void onPostExecute(Boolean result) {
            if (result) {
                // QCAR-Initialisierung erfolgreich
                Log.d("InitQCARTask.onPostExecute: QCAR initialization successful");
                updateAppStatus(APP_STATUS_INIT_AUGMENTED_REALITY);
            } else {
                // QCAR-Initialisierung nicht erfolgreich
                // Dialog-Box für die Anzeige eines Fehlers erzeugen
                AlertDialog alertDialog = new AlertDialog.Builder(Invisiboga.this).create();
                alertDialog.setButton("Close", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        // Exit
                        System.exit(1);
                    }
                });
                // Fehler prüfen und eine entsprechende Log-Nachricht erzeugen
                String logMessage;
                if (mProgressValue == QCAR.INIT_DEVICE_NOT_SUPPORTED) {
                    logMessage = "QCAR konnte nicht initialisiert werden, das Gerät wird nicht unterstützt.";
                } else if (mProgressValue == QCAR.INIT_CANNOT_DOWNLOAD_DEVICE_SETTINGS) {
                    logMessage = "Keine Internet-Verbindung. Diese wird für die Initialisierung der Kamera-Einstellungen benötigt.";
                } else {
                    logMessage = "QCAR konnte nicht initialisiert werden.";
                }
                // Fehler loggen
                Log.e("InitQCARTask.onPostExecute: " + logMessage + " Exiting.");
                // Dialog-Box mit dem Fehler anzeigen
                alertDialog.setMessage(logMessage);
                alertDialog.show();
            }
        }
    }

    /**
     * Asynchroner Task zum Laden der Tracker-Daten.
     */
    private class LoadTrackerTask extends AsyncTask<Void, Integer, Boolean> {
        /**
         * Lädt die Tracker-Daten im Hintergrund.
         */
        @Override
        protected Boolean doInBackground(Void... params) {
            int progressValue = -1;
            do {
                progressValue = QCAR.load();
            } while (!isCancelled() && progressValue >= 0 && progressValue < 100);
            return (progressValue > 0);
        }

        /**
         * Wird nach doInBackground ausgeführt.
         */
        @Override
        protected void onPostExecute(Boolean result) {
            Log.d("LoadTrackerTask.onPostExecute: execution " + (result ? "successful" : "failed"));
            updateAppStatus(APP_STATUS_INITED);
        }
    }

    /**
     * Lädt eine native Bibliothek, die sich unter "libs/armeabi" befindet.
     */
    public static boolean loadNativeLibrary(String libraryName) {
        try {
            System.loadLibrary(libraryName);
            Log.i("Native library lib" + libraryName + ".so loaded");
            return true;
        } catch (UnsatisfiedLinkError ulee) {
            Log.e("The library lib" + libraryName + ".so could not be loaded");
        } catch (SecurityException se) {
            Log.e("The library lib" + libraryName + ".so was not allowed to be loaded");
        }
        return false;
    }

    /**
     * Wird ausgeführt, wenn die Activity erzeugt wird.
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.d("onCreate");
        super.onCreate(savedInstanceState);
        updateAppStatus(APP_STATUS_INIT_APP);
    }

    /**
     * Lädt benötigte Texturen aus dem Android Package.<br>
     * Die Reihenfolge ist wichtig, weil die Texturen im nativen Code anhand des
     * Indexes den Felder-Arten zugeordnet werden.
     */
    private void loadTextures() {
        Log.i("Loading textures");
        // Textur für das Start-Feld
        mTextures.add(Texture.loadTextureFromApk("startSpace.png", getAssets()));
        // Textur für ein normales Feld
        mTextures.add(Texture.loadTextureFromApk("space.png", getAssets()));
        // Textur für ein Spezial-Feld
        mTextures.add(Texture.loadTextureFromApk("specialSpace.png", getAssets()));
        // Textur für das Ziel-Feld
        mTextures.add(Texture.loadTextureFromApk("targetSpace.png", getAssets()));
    }

    /**
     * Aktualisiert den App-Status.
     * 
     * @param appStatus
     *            Der neue App-Status.
     */
    private void updateAppStatus(int appStatus) {
        // Nichts tun, wenn sich der App-Status nicht verändert hat
        if (appStatus == mAppStatus) {
            return;
        }

        // App-Status aktualisieren
        mAppStatus = appStatus;

        // App-Status auswerten, die entsprechende(n) Aktion(en) ausführen und
        // wenn nötig den App-Status aktualisieren
        switch (appStatus) {
            case APP_STATUS_INIT_APP:
                initApp();
                updateAppStatus(APP_STATUS_INIT_QCAR);
                break;
            case APP_STATUS_INIT_QCAR:
                initQcar();
                break;
            case APP_STATUS_INIT_AUGMENTED_REALITY:
                initAugmentedReality();
                updateAppStatus(APP_STATUS_INIT_TRACKER);
                break;
            case APP_STATUS_INIT_TRACKER:
                initTracker();
                break;
            case APP_STATUS_INITED:
                // Anforderung an den Garbage Collector zum Aufräumen
                System.gc();
                // native Initialisierung, die nach der Initialisierung des
                // QCAR-Frameworks ausgeführt werden soll, anstoßen
                onQCARInitializedNative();
                // GLSurfaceView hinzufügen
                addContentView(mGlSurfaceView, new LayoutParams(LayoutParams.FILL_PARENT,
                    LayoutParams.FILL_PARENT));
                // OverlayView hinzufügen
                addContentView(mOverlayView, new LayoutParams(LayoutParams.FILL_PARENT,
                    LayoutParams.FILL_PARENT));
                updateAppStatus(APP_STATUS_START_CAMERA);
                // Lade-Fortschritt-Dialog ausblenden
                loadProgressDialog.dismiss();
                break;
            case APP_STATUS_START_CAMERA:
                startCamera();
                break;
            case APP_STATUS_STOP_CAMERA:
                stopCamera();
                break;
            default:
                throw new RuntimeException("Invalid application state");
        }
    }

    /**
     * Initialisiert Komponenten, die nichts mit QCAR oder Augmented Reality zu
     * tun haben.
     */
    private void initApp() {
        // Lade-Fortschritt-Dialog erzeugen und anzeigen
        loadProgressDialog = ProgressDialog
            .show(Invisiboga.this, "", "Lade, bitte warten...", true);
        // Texturen laden
        mTextures = new Vector<Texture>();
        loadTextures();
        // Querformat setzen
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        // Flags setzen, damit der Bildschirm immer an und hell bleibt
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON,
            WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        // Display-Größe speichern
        DisplayMetrics metrics = new DisplayMetrics();
        getWindowManager().getDefaultDisplay().getMetrics(metrics);
        mScreenWidth = metrics.widthPixels;
        mScreenHeight = metrics.heightPixels;
    }

    /**
     * Startet die QCAR-Initialiserung.
     */
    private void initQcar() {
        try {
            mInitQcarTask = new InitQcarTask();
            mInitQcarTask.execute();
        } catch (Exception e) {
            Log.e("Initializing QCAR SDK failed");
        }
    }

    /**
     * Initialisiert Augmented Reality-spezifische Komponenten.
     */
    private void initAugmentedReality() {
        // native App-Initialisierung anstoßen
        initApplicationNative(mScreenWidth, mScreenHeight);
        // Instanzen der Views erzeugen
        mOverlayView = new OverlayView(this);
        mGlSurfaceView = new InvisibogaGlSurfaceView(this, mOverlayView);
        // Initialisierung der GLSurfaceView anstoßen
        mGlSurfaceView.init(QCAR.GL_11, QCAR.requiresAlpha(), 16, 0);
    }

    /**
     * Startet das Laden der Tracker-Daten.
     */
    private void initTracker() {
        try {
            mLoadTrackerTask = new LoadTrackerTask();
            mLoadTrackerTask.execute();
        } catch (Exception e) {
            Log.e("Loading tracking data set failed");
        }
    }

    /**
     * Ereignis-Handler für das Touch-Event.<br>
     * Wertet die Ereignis-Parameter aus und leitet diese an den
     * Ereignis-Handler auf der nativen Ebene weiter.
     */
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int actionType = -1;
        // prüfen, welche Aktion der User ausgeführt hat
        switch (event.getAction() & MotionEvent.ACTION_MASK) {
        // User berührt den Bildschirm
            case MotionEvent.ACTION_DOWN:
                actionType = 0;
                break;
            // User bewegt den Finger auf dem Bildschirm
            case MotionEvent.ACTION_MOVE:
                actionType = 1;
                break;
            // User nimmt den Finger vom Bildschrim
            case MotionEvent.ACTION_UP:
                actionType = 2;
                break;
            // User bricht die Aktion ab
            case MotionEvent.ACTION_CANCEL:
                actionType = 3;
                break;
        }
        // Koordinaten der Berührung auslesen
        float x = event.getX(0);
        float y = event.getY(0);
        // Ereignis-Behandlung auf der nativen Ebene starten
        nativeTouchEvent(actionType, x, y);
        return true;
    }

    /**
     * Gibt die Anzahl der Texturen zurück.<br>
     * Wird vom nativen Code aufgerufen.
     * 
     * @return Anzahl der Texturen.
     */
    public int getTextureCount() {
        return mTextures.size();
    }

    /**
     * Gibt die Textur an einer bestimmten Stelle zurück.<br>
     * Wird vom native Code aufgerufen.
     * 
     * @param i
     *            Index der Textur.
     * 
     * @return Textur an der übergebenen Stelle.
     */
    public Texture getTexture(int i) {
        Texture texture = null;
        try {
            texture = mTextures.elementAt(i);
        } catch (IndexOutOfBoundsException e) {
            Log.w(e.toString());
        }
        return texture;
    }

    /**
     * Wird aufgerufen, wenn die App aus irgend einem Grund pausiert wird.
     */
    @Override
    protected void onPause() {
        Log.d("onPause");
        super.onPause();
        // GLSurfaceView pausieren
        if (mGlSurfaceView != null) {
            mGlSurfaceView.setVisibility(View.INVISIBLE);
            mGlSurfaceView.onPause();
        }
        // QCAR pausieren
        QCAR.onPause();
        // Kamera ausschalten
        if (mAppStatus == APP_STATUS_CAMERA_RUNNING) {
            updateAppStatus(APP_STATUS_STOP_CAMERA);
        }
    }

    /**
     * Wird aufgerufen, wenn die App wieder fortgesetzt wird.
     */
    @Override
    protected void onResume() {
        Log.d("onResume");
        super.onResume();
        // QCAR fortsetzen
        QCAR.onResume();
        // Kamera einschalten
        if (mAppStatus == APP_STATUS_CAMERA_STOPPED) {
            updateAppStatus(APP_STATUS_START_CAMERA);
        }
        // GLSurfaceView aktivieren
        if (mGlSurfaceView != null) {
            mGlSurfaceView.setVisibility(View.VISIBLE);
            mGlSurfaceView.onResume();
        }
    }

    /**
     * Wird aufgerufen, wenn die App beendet wird.<br>
     * Führt Deinitialisierung auf der Java-Ebene durch und stößt die
     * Deinitialisierung auf der nativen Ebene an.
     */
    @Override
    protected void onDestroy() {
        Log.d("Invisiboga.onDestroy");
        super.onDestroy();
        // InitQcarTask-Instanz freigeben
        if (mInitQcarTask != null && mInitQcarTask.getStatus() != InitQcarTask.Status.FINISHED) {
            mInitQcarTask.cancel(true);
            mInitQcarTask = null;
        }
        // LoadTrackerTask-Instanz freigeben
        if (mLoadTrackerTask != null
            && mLoadTrackerTask.getStatus() != LoadTrackerTask.Status.FINISHED) {
            mLoadTrackerTask.cancel(true);
            mLoadTrackerTask = null;
        }
        // native Deinitialisierung
        deinitNative();
        // Texturen löschen
        mTextures.clear();
        mTextures = null;
        // QCAR deinitalisieren
        QCAR.deinit();
        // Aufforderung an den Garbage Collector zum Aufräumen
        System.gc();
    }

    /**
     * Native Methode zur App-Initialisierung.
     */
    private native void initApplicationNative(int width, int height);

    /**
     * Native Methode, die nach der erfolgreichen Initialisierung des
     * QCAR-Frameworks ausgeführt wird.
     */
    private native void onQCARInitializedNative();

    /**
     * Native Methode zum Einschalten der Kamera.
     */
    private native void startCamera();

    /**
     * Native Methode zum Ausschalten der Kamera.
     */
    private native void stopCamera();

    /**
     * Natives Touch-Ereignis.
     */
    public native void nativeTouchEvent(int actionType, float x, float y);

    /**
     * Native Methode zur App-Deinitialisierung.
     */
    private native void deinitNative();
}
