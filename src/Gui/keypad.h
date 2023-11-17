#ifndef FREEKEYPADWINDOW_H
#define FREEKEYPADWINDOW_H

#include <QDialog>
#include "ui_keypad_free.h"

class Keypad : public QDialog
{
    Q_OBJECT
public:

    enum InputMode 
    {
        UnsignedIntegerMode = 0x01,
        SignedIntegerMode   = 0x02,

        UnsignedFloatMode   = 0x04
    };

    explicit Keypad( QString title, Keypad::InputMode inputMode, QWidget *parent = 0);

    /** \brief Handler for keyboard input.
     * Allows to use keyboard instead of buttons
     * \param event Qt Keyboard event
     */
    void keyPressEvent(QKeyEvent *event);

    /** \brief Access input value as int.
     * \return Converted value
     */
    int         toInt() { return qstrInput.toInt( &mLastConversionOk ); }

    double      toDouble() { return qstrInput.toDouble( &mLastConversionOk ); }

    /** \brief Access input value.
     * \return Current input string
     */
    QString     text()  { return qstrInput; }

    inline bool lastConversionOk() const { return mLastConversionOk; }
    
    inline void setTitle( const QString& newTitle ) { ui.titleLabel->setText( newTitle ); }
    
    Keypad& setInitialValue( const QString&  );
    Keypad& setMaxIntValue( int );
    Keypad& setMinIntValue( int );
    Keypad& setClearOnFirstKey();

    virtual ~Keypad();

private slots:
    /** \brief Handler for button clicks.
     * The calling button is decode in the handler
     */
    void buttonClicked();

protected:

    virtual void closeEvent( QCloseEvent *);
    virtual void showEvent ( QShowEvent * );

    /** \brief Enables the given set of buttons.
     * \param buttons String with chars to enable
     */
    void enableButtons(QString buttons);

    /** \brief Process all input  (Buttons and keys).
     * \param key Key triggered
     */
    void keyPress(QChar key);

    /** \brief Internal helper: map a button to a certain key.
     */
    void mapButton(QPushButton* button, QChar c);

    /** \brief Check which buttons to enable after new input.
     */        void checkState();

    QMap<QPushButton*,QChar>    qmButtons;
    QString             qstrInput;
    QString             qstrLegalKeys;
    bool                mSignedValuesAllowed;
    Ui::KeypadFree      ui;

    bool mLastConversionOk;

    InputMode mInputMode;
    bool mHaveMaxValue;
    int  mMaxValue;
    bool mHaveMinValue;
    int  mMinValue;
    bool mClearOnFirstKey;

};

#endif // FREEKEYPADWINDOW_H
