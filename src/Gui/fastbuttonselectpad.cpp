#include "fastbuttonselectpad.h"
#include "ui_fastbuttonselectpad.h"

#include "QPushButton"
#include "QDebug"

FastButtonSelectPad::FastButtonSelectPad(QStringList& qstrlButtonTexts, const QString& qstrHeading, QWidget *parent, const bool bShowCancelButton) :
    QDialog             (parent),
    qstrlButtons        (qstrlButtonTexts),
    bShowCancelButton   (bShowCancelButton),
    ui                  (new Ui::FastButtonSelectPad)
{
    ui->setupUi(this);

    QPalette qplTmp = ui->labHeading->palette();
    qplTmp.setColor(ui->labHeading->backgroundRole(), Qt::white);
    ui->labHeading->setAutoFillBackground(true);
    ui->labHeading->setPalette(qplTmp);
    ui->labHeading->setText(qstrHeading);

    qstrRetValue.clear();
    bValueSelected = false;

    this->setWindowTitle("");

    this->buildGui();
}

FastButtonSelectPad::~FastButtonSelectPad()
{
    delete ui;
}

void FastButtonSelectPad::buildGui (void)
{
    // declaration of variables
    ssize_t sCounter;
    ssize_t sRowCounter = 0;
    ssize_t sColCounter = 0;

    for (sCounter = 0; sCounter < qstrlButtons.size (); sCounter++)
    {
        QPushButton* pclButton = new QPushButton (this);
        pclButton->setText      (qstrlButtons[sCounter]);
        ui->glButtons->addWidget(pclButton, sRowCounter, sColCounter);

        connect(pclButton, SIGNAL (clicked ()), &(this->clButtonSigMapper), SLOT(map ()));
        this->clButtonSigMapper.setMapping(pclButton, sCounter);

        sColCounter++;
        if (sColCounter > 4)
        {
            sColCounter = 0;
            sRowCounter++;
        }
    }

    connect (&(this->clButtonSigMapper), SIGNAL(mapped(int)), this, SLOT(buttonClicked(int)));

    this->ui->pbCancel->setVisible (this->bShowCancelButton);
    if (this->bShowCancelButton == false)
    {
        this->setWindowFlags(((this->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint));
    }
}

void FastButtonSelectPad::buttonClicked (int iIndex)
{
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] button was clicked:" << iIndex << "/" << this->qstrlButtons [iIndex];

    qstrRetValue   = this->qstrlButtons [iIndex];
    bValueSelected = true;
    this->close();
}

void FastButtonSelectPad::on_pbCancel_clicked (void)
{
    qstrRetValue.clear();
    bValueSelected = false;
    this->close();
}


