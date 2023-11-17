#include "simple_job_popup.h"
#include "ui_simple_job_popup.h"

#include <QDebug>
#include "keypad.h"
#include "statekeypadwindow.h"


SimpleJobPopup::SimpleJobPopup(QStringList qstrlValidSampleFreqs, QWidget *parent) :
    QDialog    (parent),
    ui         (new Ui::SimpleJobPopup),
    mHour      (0),
    mMinute    (0),
    mSecond    (0),
    qstrlSampleFreqs (qstrlValidSampleFreqs)

{
    // declaration of variables

    ui->setupUi(this);

    // init list of sample frequencies

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Sample Frequenies:" << qstrlSampleFreqs;

    mFrequency = 0;
    ui->cbSampleFreq->clear           ();
    ui->cbSampleFreq->addItems        (qstrlSampleFreqs);
    ui->cbSampleFreq->setCurrentIndex (0);
    this->setFrequency();

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Sample Frequenies:" << this->mFrequency;

    QObject::connect (ui-> pb_accept     ,SIGNAL(clicked()), this, SLOT( accept() ) , Qt::UniqueConnection );
    QObject::connect (ui-> pb_cancel     ,SIGNAL(clicked()), this, SLOT( reject() ) , Qt::UniqueConnection );

    QObject::connect (ui->cbSampleFreq   ,SIGNAL(currentIndexChanged(int)), this, SLOT( setFrequency() ) , Qt::UniqueConnection );

    QObject::connect (ui-> pb_duration_hour     ,SIGNAL(clicked()), this, SLOT( setDurationHour ()   ), Qt::UniqueConnection );
    QObject::connect (ui-> pb_duration_minute   ,SIGNAL(clicked()), this, SLOT( setDurationMinute()  ), Qt::UniqueConnection );
    QObject::connect (ui-> pb_duration_second   ,SIGNAL(clicked()), this, SLOT( setDurationSecond () ), Qt::UniqueConnection );

}

SimpleJobPopup& SimpleJobPopup::setDurationAsInterval( const Types::TimeInterval& duration )
{
    mHour   = duration.getHoursRemainder  ();
    mMinute = duration.getMinutesRemainder();
    mSecond = duration.getSecondsRemainder();
    return *this;
}

Types::TimeInterval SimpleJobPopup::getDuration() const
{
    Types::TimeInterval duration;
    duration
        .addHours( mHour )
        .addMinutes( mMinute )
        .addSeconds( mSecond );
    return duration;
}

void SimpleJobPopup::setDurationHour   ()
{
    Keypad keyPad ( tr( "hour") , Keypad::UnsignedIntegerMode );
    keyPad
        .setInitialValue( QString::number( mHour) )
        .setMaxIntValue( 999 )
        .setClearOnFirstKey();
        
    if (not (keyPad.exec() == QDialog::Accepted))
        return;

    mHour = keyPad.toInt();
    updateDurationDisplay();
    // press ENTER again to accept
    this->ui->pb_accept->setFocus();

}
    
void SimpleJobPopup::setDurationMinute ()
{
    Keypad keyPad ( tr( "minute") , Keypad::UnsignedIntegerMode );
    keyPad
        .setInitialValue( QString::number( mMinute) )
        .setMaxIntValue( 59 )
        .setClearOnFirstKey();
        
    if (not (keyPad.exec() == QDialog::Accepted))
        return;

    mMinute = keyPad.toInt();
    updateDurationDisplay();
    // press ENTER again to accept
    this->ui->pb_accept->setFocus();
}
    
void SimpleJobPopup::setDurationSecond ()
{
    Keypad keyPad ( tr( "second") , Keypad::UnsignedIntegerMode );
    keyPad
        .setInitialValue( QString::number( mSecond) )
        .setMaxIntValue( 59 )
        .setClearOnFirstKey();
        
    if (not (keyPad.exec() == QDialog::Accepted))
        return;

    mSecond = keyPad.toInt();
    updateDurationDisplay();
    // press ENTER again to accept
    this->ui->pb_accept->setFocus();
}

void SimpleJobPopup::setFrequency()
{
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] setting sample frequency" << ui->cbSampleFreq->currentText().toInt();
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] setting sample frequency" << ui->cbSampleFreq->currentText();
    mFrequency = ui->cbSampleFreq->currentText().toInt();
    this->ui->pb_accept->setFocus();

}

void SimpleJobPopup::updateDurationDisplay()
{
    ui -> pb_duration_hour   -> setText( QString::number( mHour   ).rightJustified( 3 , '0', true ) );
    ui -> pb_duration_minute -> setText( QString::number( mMinute ).rightJustified( 2 , '0', true ) );
    ui -> pb_duration_second -> setText( QString::number( mSecond ).rightJustified( 2 , '0', true ) );

}

void SimpleJobPopup::updateFrequencyDisplay()
{
    ui->cbSampleFreq->setCurrentIndex (0);
    this->setFrequency();
}

void SimpleJobPopup::updateDisplay()
{
    updateDurationDisplay();
    updateFrequencyDisplay();
}


SimpleJobPopup::~SimpleJobPopup()
{
    delete ui;
}
