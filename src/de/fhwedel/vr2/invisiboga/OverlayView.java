package de.fhwedel.vr2.invisiboga;

import android.app.AlertDialog;
import android.app.AlertDialog.Builder;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

/**
 * Zutändig für die Darstellung des User Interfaces.<br>
 * <br>
 * Diese View überlagert die OpenGL-View und bietet dem User somit die
 * Möglichkeit über Android-UI-Elemente mit der App zu interagieren.
 * 
 * @author Alexander Bertram
 * 
 */
public class OverlayView extends RelativeLayout {
    /**
     * Alpha-Wert für die Buttons.
     */
    private static final int BUTTON_ALPHA = (int) (0.5 * 255);

    /**
     * Weiter-Button.
     */
    private Button nextButton;
    /**
     * Würfeln-Button.
     */
    private Button diceButton;
    /**
     * Neustart-Button.
     */
    private Button restartButton;
    /**
     * Neustart-Bestätigung-Dialog.
     */
    private AlertDialog restartConfirmationDialog;
    /**
     * Instanz des Handlers, der Nachrichten aus anderen Threads verarbeiten
     * kann und somit eine Thread-übergreifende User Interface-Manipulation
     * ermöglicht.
     */
    protected OverlayViewHandler mHandler;

    /**
     * Konstruktor.
     * 
     * @param context
     *            Kontext, in dem die View erzeugt wird.
     */
    public OverlayView(Context context) {
        super(context);
        // das in der overlay.xml definierte Layout laden und als View
        // hinzufügen
        LayoutInflater layoutInflater = LayoutInflater.from(context);
        addView(layoutInflater.inflate(R.layout.overlay, null, false));
        // eine Instanz des OverlayViewHandlers erzeugen
        mHandler = new OverlayViewHandler(this);
        // Neustart-Bestädigunsdialog erzeugen
        restartConfirmationDialog = createConfirmationDialog();
        // Buttons initialisieren
        initButtons();
        // View in den Anfangszustand versetzen
        reset();
    }

    /**
     * Initialisert die Buttons.<br>
     * <br>
     * Es wird die Transparenz der Buttons verändert und die
     * OnClick-Ereignis-Handler werden gesetzt.
     */
    private void initButtons() {
        nextButton = (Button) findViewById(R.id.nextButton);
        nextButton.getBackground().setAlpha(BUTTON_ALPHA);
        nextButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // Ereignis weiterreichen an den nativen Code
                nextButtonClickNative();
                // Button verstecken
                nextButton.setVisibility(INVISIBLE);
            }
        });
        diceButton = (Button) findViewById(R.id.diceButton);
        diceButton.getBackground().setAlpha(BUTTON_ALPHA);
        diceButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // Ereignis weiterreichen an den nativen Code
                diceButtonClickNative();
                // Button verstecken
                diceButton.setVisibility(INVISIBLE);
            }
        });
        restartButton = (Button) findViewById(R.id.restartButton);
        restartButton.getBackground().setAlpha(BUTTON_ALPHA);
        restartButton.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
                // Neustart-Bestätigungsdialog anzeigen
                restartConfirmationDialog.show();
            }
        });
    }

    /**
     * Erzeugt eine Instanz des Neustart-Bestätigungsdialogs.
     * 
     * @return Instanz des Neustart-Bestätigungsdialogs.
     */
    private AlertDialog createConfirmationDialog() {
        Builder alertDialogBuilder = new Builder(getContext());
        // den anzuzeigenden Text setzen
        alertDialogBuilder.setMessage("Neustart?").setCancelable(false)
            .setPositiveButton("Ja", new DialogInterface.OnClickListener() {
                // Verhalten beim Klick auf "Ja"
                public void onClick(DialogInterface dialog, int id) {
                    // View in Start-Zustand versetzen
                    reset();
                    // Neustart auf der nativen Ebene
                    restartNative();
                }
            }).setNegativeButton("Nein", new DialogInterface.OnClickListener() {
                // Verhalten beim Klick auf "Nein"
                public void onClick(DialogInterface dialog, int id) {
                    // Dialog ausblenden
                    dialog.cancel();
                }
            });
        return alertDialogBuilder.create();
    }

    /**
     * Zeigt je nach Parameter eine View an oder versteckt diese.
     * 
     * @param name
     *            Name der zu manipulierenden View.
     * @param visibility
     *            True, wenn die View angezeigt werden soll.
     */
    private void setViewVisibility(String name, Boolean visibility) {
        Log.d("OverlayView.setViewVisibility(%s, %s)", name, visibility.toString());
        // Id der View anhand des Namens bestimmen
        int id = getResources().getIdentifier(this.getContext().getPackageName() + ":id/" + name,
            null, null);
        // View suchen
        View view = findViewById(id);
        // Sichtbarkeit setzen
        view.setVisibility(visibility ? VISIBLE : INVISIBLE);
        // Ein Workaround, damit die View sichtbar wird
        if (visibility && view instanceof TextView) {
            TextView textView = (TextView) view;
            textView.setText(textView.getText());
        }
    }

    /**
     * Zeigt eine View an.
     * 
     * @param name
     *            View-Name.
     */
    protected void showView(String name) {
        Log.d("OverlayView.showView(%s)", name);
        setViewVisibility(name, true);
    }

    /**
     * Versteckt eine View.
     * 
     * @param name
     *            View-Name.
     */
    protected void hideView(String name) {
        Log.d("OverlayView.hideView(%s)", name);
        setViewVisibility(name, false);
    }

    /**
     * Zeigt einen Hinweis, einen "Toast" an.
     * 
     * @param text
     *            Hinweis-Text.
     * @param duration
     *            Dauer der Anzeige, entweder Toast.LENGTH_SHORT oder
     *            Toast.LENGTH_LONG.
     */
    protected void showToast(CharSequence text, int duration) {
        Toast.makeText(getContext(), text, duration).show();
    }

    /**
     * Setzt den Text der currentPlayer-TextView.
     * 
     * @param text
     *            Der anzuzeigende Text.
     */
    protected void setCurrentPlayerTextViewText(CharSequence text) {
        Log.d("OverlayView.setCurrentPlayerTextViewText(text: %s)", text);
        ((TextView) findViewById(R.id.currentPlayerTextView)).setText(text);
    }

    /**
     * Setzt die Text-Farbe der currentPlayer-TextView.
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
    protected void setCurrentPlayerTextViewTextColor(int alpha, int red, int green, int blue) {
        Log.d("OverlayView.setCurrentPlayerTextColor(alpha: %d, red: %d, green: %d, blue: %d)",
            alpha, red, green, blue);
        ((TextView) findViewById(R.id.currentPlayerTextView)).setTextColor(Color.argb(alpha, red,
            green, blue));
    }

    /**
     * Versetzt die View in den Start-Zustand, in dem alle Views versteckt
     * werden.
     */
    private void reset() {
        hideView("diceButton");
        hideView("nextButton");
        hideView("restartButton");
        hideView("currentPlayerTextView");
    }

    /**
     * Native Methode, die nach einem Klick auf den Weiter-Button ausgeführt
     * wird.
     */
    private native void nextButtonClickNative();

    /**
     * Native Methode, die nach einem Klick auf den Würfeln-Button ausgeführt
     * wird.
     */
    private native void diceButtonClickNative();

    /**
     * Native Methode, die für einen Neustart auf der nativen Ebene sorgt.
     */
    private native void restartNative();
}
