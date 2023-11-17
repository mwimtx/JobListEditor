#include "datetimeselector.h"
#include "ui_datetimeselector.h"
#include "statekeypadwindow.h"

#include "keypad.h"
#include <QDebug>
#include <QSettings>
#include <default_values.h>
#include <cmath>

DateTimeSelector::DateTimeSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DateTimeSelector)
{
    ui->setupUi(this);
    ui-> dateEdit -> setDisplayFormat( DefaultValues::DateFormat );
    setupSignals();

    QSettings settings;
    restoreGeometry( settings.value("DateTimeSelector/geometry").toByteArray() );

    // hide expert settings
    ui->gbExpertSettings->setVisible(false);
}

DateTimeSelector::~DateTimeSelector()
{
    QSettings settings;
    settings.setValue("DateTimeSelector/geometry", saveGeometry());
    delete ui;
}

void DateTimeSelector::setupSignals()
{
    QObject::connect( ui -> pbDay,   SIGNAL( clicked() ), this ,SLOT( setDayFromKeypad()   ) );
    QObject::connect( ui -> pbMonth, SIGNAL( clicked() ), this ,SLOT( setMonthFromKeypad() ) );
    QObject::connect( ui -> pbYear,  SIGNAL( clicked() ), this ,SLOT( setYearFromKeypad()  ) );

    QObject::connect( ui -> pbToday,     SIGNAL( clicked() ), this ,SLOT( setTodayFromKeypad()    ) );
    QObject::connect( ui -> pbTomorrow , SIGNAL( clicked() ), this ,SLOT( setTomorrowFromKeypad() ) );

    QObject::connect( ui -> pbHour,    SIGNAL( clicked() ), this ,SLOT( setHourFromKeypad()   ) );
    QObject::connect( ui -> pbMinute,  SIGNAL( clicked() ), this ,SLOT( setMinuteFromKeypad() ) );
    QObject::connect( ui -> pbSecond , SIGNAL( clicked() ), this ,SLOT( setSecondFromKeypad() ) );

    QObject::connect( ui -> dateEdit ,   SIGNAL( dateChanged ( const QDate & ) ) , this , SLOT( setDate ( const QDate & ) ) );

    QObject::connect( ui -> pbCancel ,   SIGNAL( clicked() ), this , SLOT( reject() ) );
    QObject::connect( ui -> pbOk ,       SIGNAL( clicked() ), this , SLOT( accept() ) );

}
    
DateTimeSelector& DateTimeSelector::setDateTime( const QDateTime& dateTime )
{
    mDate   = dateTime.date();
    mHour   = dateTime.time().hour();
    mMinute = dateTime.time().minute();
    mSecond = dateTime.time().second();
    updateTimeDisplay();
    updateDateDisplay();
    return *this;
}

QDateTime DateTimeSelector::getDateTime() const
{
    return QDateTime( mDate, QTime( mHour, mMinute, mSecond ) );
}


void DateTimeSelector::updateTimeDisplay()
{
    ui -> pbHour   -> setText( QString::number( mHour   ).rightJustified( 2 , '0', true ) );
    ui -> pbMinute -> setText( QString::number( mMinute ).rightJustified( 2 , '0', true ) );
    ui -> pbSecond -> setText( QString::number( mSecond ).rightJustified( 2 , '0', true ) );    
}

void DateTimeSelector::updateDateDisplay()
{
    ui -> dateEdit -> setDate( mDate );
    ui -> pbDay   -> setText( QString::number( mDate.day()   ).rightJustified( 2 , '0', true ) );  
    ui -> pbMonth -> setText( QString::number( mDate.month() ).rightJustified( 2 , '0', true ) );  
    ui -> pbYear  -> setText( QString::number( mDate.year()  ).rightJustified( 4 , '0', true ) );  
    
}

// SLOT
void DateTimeSelector::setHourFromKeypad    ()
{
    HourKeypadWindow* pclKeyPad = new HourKeypadWindow ("Hour [00 .. 23]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        mHour = pclKeyPad->text().toInt();
    }
    delete (pclKeyPad);
    updateTimeDisplay();
}

// SLOT
void DateTimeSelector::setMinuteFromKeypad  ()
{
    MinSecKeypadWindow* pclKeyPad = new MinSecKeypadWindow ("Minute [00 .. 59]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        mMinute = pclKeyPad->text().toInt();
    }
    delete (pclKeyPad);
    updateTimeDisplay();
}

// SLOT
void DateTimeSelector::setSecondFromKeypad ()
{
    MinSecKeypadWindow* pclKeyPad = new MinSecKeypadWindow ("Second [00 .. 59]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        mSecond = pclKeyPad->text().toInt();
    }
    delete (pclKeyPad);
    updateTimeDisplay();
}

// SLOT
void DateTimeSelector::setTodayFromKeypad    ()
{
    mDate = QDate::currentDate();
    updateDateDisplay();
}

// SLOT
void DateTimeSelector::setTomorrowFromKeypad ()
{
    mDate = QDate::currentDate().addDays( 1 );
    updateDateDisplay();
}

// SLOT
void DateTimeSelector::setDayFromKeypad()
{
    DayKeypadWindow* pclKeyPad = new DayKeypadWindow ("day [01 .. 31]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        QDate newDate = QDate(mDate.year(), mDate.month(),  pclKeyPad->dayValue());

        if (newDate.isValid())
        {
            mDate = newDate;
        }
        updateDateDisplay();
    }
    delete (pclKeyPad);
}

void DateTimeSelector::setMonthFromKeypad()
{
    MonthKeypadWindow* pclKeyPad = new MonthKeypadWindow ("Month [01 .. 12]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        QDate newDate = QDate(mDate.year(), pclKeyPad->monthValue(),  mDate.day());

        if (newDate.isValid())
        {
            mDate = newDate;
        }
        updateDateDisplay();
    }
    delete (pclKeyPad);
}

void DateTimeSelector::setYearFromKeypad()
{
    YearKeypadWindow* pclKeyPad = new YearKeypadWindow ("Year [0000 .. 7999]", this);
    if (pclKeyPad->exec() == QDialog::Accepted)
    {
        QDate newDate = QDate(pclKeyPad->yearValue(), mDate.month(),  mDate.day());

        if (newDate.isValid())
        {
            mDate = newDate;
        }
        updateDateDisplay();
    }
    delete (pclKeyPad);
}

void DateTimeSelector::setDate ( const QDate & newDate )
{
    mDate = newDate;
    updateDateDisplay();
}


void DateTimeSelector::on_pbStartTimeExpert_clicked ()
{
    // delcaration of variables
    unsigned int uiXParentCenter = (this->parentWidget()->x() + this->parentWidget()->width ()) / 2;
    unsigned int uiYParentCenter = (this->parentWidget()->y() + this->parentWidget()->height()) / 2;

    if (ui->gbExpertSettings->isVisible() == false)
    {
        ui->gbExpertSettings->setVisible(true);
    }
    else
    {
        ui->gbExpertSettings->setVisible(false);
    }

    this->repaint();
    this->update ();
    this->setGeometry(uiXParentCenter - (this->width() / 2), uiYParentCenter - (this->height() / 2), this->width(), this->height());
}


void DateTimeSelector::setStartMode(const QString& qstrStartMode)
{
    // declaration of variables
    int      iIndex = Types::StartModes::Invalid;
    unsigned uiCounter;

    for (uiCounter = 0; (int)uiCounter < Types::qstrlStartModeStrings.size(); uiCounter++)
    {
        if (qstrStartMode.compare(Types::qstrlStartModeStrings [uiCounter]) == 0)
        {
            iIndex = uiCounter;
            break;
        }
    }

    if (iIndex == Types::StartModes::Invalid)
    {
        ui->cbStartMode->setCurrentIndex(Types::StartModes::Adjusted);
    }
    else
    {
        ui->cbStartMode->setCurrentIndex(iIndex);

        unsigned int uiTmp = this->getDateTime().toTime_t();
        uiTmp  = floor (uiTmp / 86400);
        uiTmp *= uiTmp * 86400;

        /*
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] Date is :" << this->getDateTime().date().year() << this->getDateTime().date().month() << this->getDateTime().date().day();
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] Date is :" << this->getDateTime().toTime_t();
        qDebug () << "[" << __PRETTY_FUNCTION__ << "] Date is :" << uiTmp;
        */

        if ((uiTmp == 0) && (iIndex == Types::StartModes::Adjusted))
        {
            ui->cbStartMode->setCurrentIndex (Types::StartModes::AdjustedToday);
        }
    }
}


const QString DateTimeSelector::getStartMode(void)
{
    return (Types::qstrlStartModeStrings [ui->cbStartMode->currentIndex()]);
}


void DateTimeSelector::setCyclicJobParameters(const QString& qstrCyclicEvent, const QString& qstrCyclicDuration, const QString& qstrCyclicUnit)
{
    // declaration of variables
    int      iIndex = Types::CyclicModeUnits::Invalid;
    bool     bOK;
    unsigned uiCounter;

    for (uiCounter = 0; (int)uiCounter < Types::qstrlCyclicModeUnitsStrings.size(); uiCounter++)
    {
        if (qstrCyclicUnit.compare(Types::qstrlCyclicModeUnitsStrings [uiCounter]) == 0)
        {
            iIndex = uiCounter;
        }
    }

    if (iIndex == Types::CyclicModeUnits::Invalid)
    {
        ui->cbCyclicUnit->setCurrentIndex(Types::CyclicModeUnits::Seconds);
    }
    else
    {
        ui->cbCyclicUnit->setCurrentIndex(iIndex);
    }

    ui->sspinCyclicDuration->setMaximum(365);
    ui->sspinCyclicDuration->setMinimum  (0);
    ui->sspinCyclicDuration->setValue(qstrCyclicDuration.toUInt(&bOK));
    if (bOK == false)
    {
        ui->sspinCyclicDuration->setValue (0);
    }

    if (qstrCyclicEvent.toUInt() == 1)
    {
        ui->chbCyclicOnOff->setChecked(true);
    }
    else
    {
        ui->chbCyclicOnOff->setChecked(false);
    }
}


const QString DateTimeSelector::getCyclicEvent (void)
{
    if (ui->chbCyclicOnOff->isChecked() == true)
    {
        return (QString::number (1));
    }
    else
    {
        return (QString::number (0));
    }
}


const QString DateTimeSelector::getCyclicDuration (void)
{
    return (QString::number (ui->sspinCyclicDuration->value()));
}


const QString DateTimeSelector::getCyclicUnit     (void)
{
    return (Types::qstrlCyclicModeUnitsStrings [ui->cbCyclicUnit->currentIndex()]);
}


void DateTimeSelector::on_cbStartMode_currentIndexChanged(int index)
{

    // by default activate ALL input elements
    ui->gbStartDate->setEnabled (true);
    ui->gbStartTime->setEnabled (true);

    switch (index)
    {
        case Types::StartModes::Relative:
            {
                // mode is relative
                ui->gbStartDate->setEnabled (false);
                ui->gbStartTime->setEnabled (false);
                break;
            }

        case Types::StartModes::AdjustedToday:
        case Types::StartModes::Grid:
            {
                // mode is relative
                ui->gbStartDate->setEnabled (false);
                ui->gbStartTime->setEnabled (true);

                this->setDate (QDate::fromString("1970-01-01", DefaultValues::DateFormat));
                break;
            }
    }
}


bool DateTimeSelector::getCleanJobTables(void)
{
    return (ui->chbCleanJobTable->isChecked());
}


bool DateTimeSelector::setCleanJobTable(const bool bFlag)
{
    ui->chbCleanJobTable->setChecked(bFlag);
    return (true);
}



