#include "dialdialog.h"
#include "timer.h"
#include "ui_dialdialog.h"

#include <QDebug>


DialDialog::DialDialog(const double dMax, const double dMin, const double dStep, const double dInitValue, const QString qstrHeading, const QString qstrSuffix, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialDialog)
{
    ui->setupUi(this);

    // initialise the GUI elements
    ui->dspValue->setMaximum   (dMax);
    ui->dspValue->setMinimum   (dMin);
    ui->dspValue->setSingleStep(dStep);
    ui->dspValue->setValue     (dInitValue);
    ui->dspValue->setSuffix    (qstrSuffix);

    QPalette qplTmp = ui->labHeading->palette();
    qplTmp.setColor(ui->labHeading->backgroundRole(), Qt::white);
    ui->labHeading->setAutoFillBackground(true);
    ui->labHeading->setPalette           (qplTmp);
    ui->labHeading->setText              (qstrHeading);

    this->setWindowTitle ("JobListEditor");

    this->uiNumStepsIncDec     = 0;
    this->uiNumStepsLastIncDec = 0;
    this->bRetValueValid       = false;
    this->dValue               = dInitValue;

}


DialDialog::~DialDialog()
{
    delete ui;
}


void DialDialog::scrollSpinBoxValue (const bool bIncrement)
{
    // declaration of variables

    if ((this->uiNumStepsIncDec == 0) ||
       ((this->uiNumStepsIncDec - this->uiNumStepsLastIncDec) >= 2))
    {
        if (bIncrement == true)
        {
            ui->dspValue->stepUp();
        }
        else
        {
            ui->dspValue->stepDown();
        }
        this->dValue = ui->dspValue->value();
        this->repaint();

        this->uiNumStepsLastIncDec = this->uiNumStepsIncDec;
    }


    if (this->uiNumStepsIncDec < 8)
    {
        Timer::msleep(150);
    }
    else if ((this->uiNumStepsIncDec >= 8) && (this->uiNumStepsIncDec < 20))
    {
        Timer::msleep(75);
    }
    else
    {
        Timer::msleep(33);
    }
    this->uiNumStepsIncDec++;
}


void DialDialog::on_pbUp_pressed (void)
{
    this->scrollSpinBoxValue(true);
}


void DialDialog::on_pbDown_pressed (void)
{
    this->scrollSpinBoxValue(false);
}


void DialDialog::on_pbUp_released (void)
{
    if (ui->pbUp->isDown() == false)
    {
        this->uiNumStepsIncDec     = 0;
        this->uiNumStepsLastIncDec = 0;
    }
}


void DialDialog::on_pbDown_released (void)
{
    if (ui->pbDown->isDown() == false)
    {
        this->uiNumStepsIncDec     = 0;
        this->uiNumStepsLastIncDec = 0;
    }
}


void DialDialog::on_pbCancel_clicked (void)
{
    this->close();
}


void DialDialog::on_pbOK_clicked (void)
{
    this->bRetValueValid = true;
    this->dValue         = ui->dspValue->value();
    this->close();
}

