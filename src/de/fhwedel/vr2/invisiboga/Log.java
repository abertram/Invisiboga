package de.fhwedel.vr2.invisiboga;

/**
 * Kapselt die Log-Methoden.
 * 
 * @author Alexander Bertram
 *
 */
public class Log {
    /**
     * Tag, unter dem die Log-Einträge erscheinen sollen.
     */
    private static final String TAG = "Invisiboga";

    /**
     * Erzeugt einen Log-Eintrag auf dem Debug-Level.
     * 
     * @param message Log-Eintrag
     */
    public static final void d(String message) {
        android.util.Log.d(TAG, message);
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Debug-Level.
     * 
     * @param format Format-String
     * @param args Liste der Argumente für den Format-String
     */
    public static final void d(String format, Object... args) {
        d(String.format(format, args));
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Error-Level.
     * 
     * @param message Log-Eintrag
     */
    public static final void e(String message) {
        android.util.Log.e(TAG, message);
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Error-Level.
     * 
     * @param format Format-String
     * @param args Liste der Argumente für den Format-String
     */
    public static final void e(String format, Object... args) {
        e(String.format(format, args));
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Info-Level.
     * 
     * @param message Log-Eintrag
     */
    public static final void i(String message) {
        android.util.Log.i(TAG, message);
    }
    
    /**
     * Erzeugt einen Log-Eintrag auf dem Info-Level.
     * 
     * @param format Format-String
     * @param args Liste der Argumente für den Format-String
     */
    public static final void i(String format, Object... args) {
        i(String.format(format, args));
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Verbose-Level.
     * 
     * @param message Log-Eintrag
     */
    public static final void v(String message) {
        android.util.Log.v(TAG, message);
    }

    /**
     * Erzeugt einen Log-Eintrag auf dem Warn-Level.
     * 
     * @param message Log-Eintrag
     */
    public static final void w(String message) {
        android.util.Log.w(TAG, message);
    }
}
