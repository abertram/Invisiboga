package de.fhwedel.vr2.invisiboga;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

/**
 * Verarbeitet Nachrichten aus anderen Threads (z.B. aus dem GL-Thread) und
 * führt je nach Nachrichten-Art bestimmte Aktionen in der OverlayView aus.
 * 
 * @author Alexander Bertram
 * 
 */
public class OverlayViewHandler extends Handler {
    /*
     * Konstanten, die für die Unterscheidung von Nachrichten benötigt werden.
     */
    /**
     * Eine View soll angezeigt werden.
     */
    protected static final int SHOW_VIEW = 0;
    /**
     * Eine View soll versteckt werden.
     */
    protected static final int HIDE_VIEW = 1;
    /**
     * Ein Toast soll angezeigt werden.
     */
    protected static final int SHOW_TOAST = 2;
    /**
     * Der Inhalt der currentPlayer-TextView soll verändert werden.
     */
    protected static final int SET_CURRENT_PLAYER_TEXT_VIEW_TEXT = 3;
    /**
     * Die Text-Farbe der currentPlayer-TextView soll verändert werden.
     */
    protected static final int SET_CURRENT_PLAYER_TEXT_VIEW_FONT_COLOR = 4;

    /**
     * Instanz der OverlayView.
     */
    OverlayView mOverlayView;

    /**
     * Konstruktor.
     * 
     * @param overlayView
     *            Instanz der OverlayView.
     */
    public OverlayViewHandler(OverlayView overlayView) {
        mOverlayView = overlayView;
    }

    /**
     * Wird aufgerufen, wenn eine Nachricht verarbeitet werden soll.<br>
     * <br>
     * Die Nachricht-Art wird ausgewertet und die entsprechnende(n) Aktion(en)
     * in der OverlayView ausgeführt.
     */
    @Override
    public void handleMessage(Message message) {
        Log.d("OverlayViewHandler.handleMessage");
        switch (message.what) {
            case SHOW_VIEW:
                mOverlayView.showView((String) message.obj);
                break;
            case HIDE_VIEW:
                mOverlayView.hideView((String) message.obj);
                break;
            case SHOW_TOAST:
                mOverlayView.showToast((CharSequence) message.obj, message.arg1);
                break;
            case SET_CURRENT_PLAYER_TEXT_VIEW_TEXT:
                mOverlayView.setCurrentPlayerTextViewText((CharSequence) message.obj);
                break;
            case SET_CURRENT_PLAYER_TEXT_VIEW_FONT_COLOR:
                Bundle data = message.getData();
                mOverlayView.setCurrentPlayerTextViewTextColor(data.getInt("alpha"),
                    data.getInt("red"), data.getInt("green"), data.getInt("blue"));
                break;
            default:
                break;
        }
    }
}
