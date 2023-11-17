#include "keypad.h"

#include <QSettings>
#include <QVariant>
#include <QKeyEvent>
#include <QDebug>
#include <limits>
#include <QSettings>

Keypad::Keypad( QString title, 
                Keypad::InputMode inputMode, 
                QWidget *parent ) :

    QDialog(parent),
    mInputMode( inputMode ),
    mHaveMaxValue( false ),
    mHaveMinValue( false ),
    mClearOnFirstKey( false )
{
    ui.setupUi(this);
    ui.titleLabel->setText(title);

    QSettings settings;
    restoreGeometry( settings.value("Keypad/geometry").toByteArray() );

    mapButton(ui.backButton, 'B');
    mapButton(ui.clearButton, 'C');
    mapButton(ui.negButton, 'N' );
    mapButton(ui.dotButton, '.' );
    mapButton(ui.plusButton, '+'); 
    mapButton(ui.minusButton, '-');
    mapButton(ui._0Button, '0');
    mapButton(ui._1Button, '1');
    mapButton(ui._2Button, '2');
    mapButton(ui._3Button, '3');
    mapButton(ui._4Button, '4');
    mapButton(ui._5Button, '5');
    mapButton(ui._6Button, '6');
    mapButton(ui._7Button, '7');
    mapButton(ui._8Button, '8');
    mapButton(ui._9Button, '9');

    checkState();
}

Keypad::~Keypad() {
    QSettings settings;
    settings.setValue("Keypad/geometry", saveGeometry());
}

void Keypad::enableButtons(QString buttons)
{
    qstrLegalKeys = buttons;
    foreach(QPushButton* button, qmButtons.keys())
    {
        button->setEnabled(buttons.contains(qmButtons[button]));
    }
}

void Keypad::mapButton(QPushButton *button, QChar c)
{
    qmButtons[button] = c;
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}


void Keypad::buttonClicked()
{
    QChar key = qmButtons[static_cast<QPushButton*>(sender())];
    keyPress(key);    
}

void Keypad::closeEvent(QCloseEvent * event )
{
    QSettings settings;
    settings.setValue("Keypad/geometry", saveGeometry());
    QDialog::closeEvent(event);    
}

void Keypad::showEvent ( QShowEvent * event )
{
    QSettings settings;
    restoreGeometry( settings.value("Keypad/geometry").toByteArray());
    QDialog::showEvent( event );    
}

void Keypad::keyPressEvent(QKeyEvent *event)
{
    QString text(event->text());
    QChar key;

    if (event->key() == Qt::Key_Delete)
        key = 'C';
    else if (event->key() == Qt::Key_Backspace)
        key = 'B';
    else if (event->key() == Qt::Key_Return || 
             event->key() == Qt::Key_Enter)
        accept();
    else if (text.length() == 1)
        key = text[0];
    else {
        QDialog::keyPressEvent(event);
        return;
    }
    if (key==',') 
        key = '.';

    if (qstrLegalKeys.contains(key)) {
        event->accept();
        keyPress(key);
    } else
        QDialog::keyPressEvent(event);
}

Keypad& Keypad::setInitialValue( const QString& initialValue ) 
{ 
    qstrInput = initialValue;
    ui.valueLine->setText(initialValue); 
    qstrInput = ui.valueLine->text();
    checkState();
    return *this;
}

Keypad& Keypad::setMaxIntValue( int maxValue )
{
    mMaxValue     = maxValue;
    mHaveMaxValue = true;
    return *this;
}

Keypad& Keypad::setMinIntValue( int minValue )
{
    mMinValue     = minValue;
    mHaveMinValue = true;
    return *this;
}
 
Keypad& Keypad::setClearOnFirstKey()
{
    mClearOnFirstKey = true;
    return *this;
}

#include "valueCheck.cc"

void Keypad::checkState()
{

    QString buttons("0123456789");

    switch ( mInputMode ) 
    {
    case UnsignedIntegerMode:
        if ( mHaveMaxValue ) {
            buttons = doValueCheck( QString::number( 0 ), QString::number( mMaxValue ) , qstrInput );
            if ( qstrInput.toInt() < mMaxValue )
                buttons += '+';
        } else 
            buttons += '+';
        if ( qstrInput.toInt() > 0 )
            buttons += '-';
        break;
    case SignedIntegerMode:
        if ( mHaveMaxValue or mHaveMinValue )
        {
            QString minVal = QString::number( mHaveMinValue ? mMinValue : std::numeric_limits<int>::min() );
            QString maxVal = QString::number( mHaveMaxValue ? mMaxValue : std::numeric_limits<int>::max() );
            buttons = doValueCheck( minVal , 
                                    maxVal , 
                                    qstrInput );            
        }
        buttons += "N";
        break;
    case UnsignedFloatMode:
        if ( mHaveMaxValue ) 
            buttons = doValueCheck( QString::number( 0 ), QString::number( mMaxValue ) , qstrInput , true );
        else
            buttons += ".";
        break;
    }
    
    if ( not qstrInput.isEmpty() )
        buttons += "BC";
    
    enableButtons(buttons);
}

void Keypad::keyPress(QChar key)
{

    char currentInput = key.toLatin1();

    if ( mClearOnFirstKey and 
         ( currentInput != '-' ) and 
         ( currentInput != '+' ) 
        ) {
        qstrInput.clear();
        mClearOnFirstKey = false;
    }

    switch ( currentInput ) {
    case 'B':
        qstrInput.resize(qstrInput.length()-1);
        break;
    case 'C':
        qstrInput.clear();
        break;
    case '.':
        if (qstrInput.isEmpty())
            qstrInput = '0';
        qstrInput += '.';
        break;
    case 'N':
        qstrInput = ( qstrInput.startsWith( '-' ) ) ?
            qstrInput.remove( 0 , 1 ) :
            QString( "-" ) + qstrInput;
        break;
    case '+':
        if ( mInputMode == UnsignedIntegerMode ) 
        {
            bool conversionOk = false;
            unsigned int value = qstrInput.toUInt( &conversionOk );
            if ( conversionOk ) 
                qstrInput = QString::number( ++value );
        }
        break;
    case '-':
        if ( mInputMode == UnsignedIntegerMode ) 
        {
            bool conversionOk = false;
            unsigned int value = qstrInput.toUInt( &conversionOk );
            if ( conversionOk and ( value > 0 ) ) 
                qstrInput = QString::number( --value );
        }
        break;
    default:
        qstrInput += key;
    }
    ui.valueLine->setText(qstrInput);
    checkState();
}
