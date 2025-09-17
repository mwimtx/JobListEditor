#include "sensorpositions.h"
#include "ui_sensorpositions.h"
#include "default_values.h"
#include "positioninputdialog.h"
#include "freekeypadwindow.h"
#include "Strings.h"

#include <QMessageBox>
#include <QPalette>

void SensorChannelPositionsUi::handlePbNorth()
{
    mSensorPositionsWidget->handlePbNorth (getChannelNumber());
}

void SensorChannelPositionsUi::handlePbSouth()
{
    mSensorPositionsWidget->handlePbSouth (getChannelNumber());
}

void SensorChannelPositionsUi::handlePbEast()
{
    mSensorPositionsWidget->handlePbEast (getChannelNumber());
}

void SensorChannelPositionsUi::handlePbWest()
{
    mSensorPositionsWidget->handlePbWest (getChannelNumber());
}


const int channelColumn = 0;
const int typeColumn    = 1;
const int northColumn   = 3;
const int southColumn   = 2;
const int eastColumn    = 5;
const int westColumn    = 4;

SensorPositions::SensorPositions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorPositions)
{
    ui->setupUi(this);
    setupGui();

    ui->pbLinkLFHF->setChecked(true);
    this->on_pbLinkLFHF_clicked();
}

SensorPositions::~SensorPositions()
{
    delete ui;
}

void SensorPositions::setupGui()
{
    QGridLayout* theGrid = ui -> innerGrid;
    
    for ( unsigned row = 0; row < DefaultValues::MaxChannels; ++row ) 
    {
        SensorChannelPositionsUi::Spt scpu = SensorChannelPositionsUiFactory::create( this , row );
        (*scpu)
            .setlbChannelNumber (new QLabel      (this ))
            .setlbChannelType   (new QLabel      (this ))
            .setpbNorth         (new QPushButton (this ))
            .setpbSouth         (new QPushButton (this ))
            .setpbEast          (new QPushButton (this ))
            .setpbWest          (new QPushButton (this ));

        theGrid -> addWidget( scpu -> getLbChannelNumber() , row , channelColumn , 1 , 1 );
        theGrid -> addWidget( scpu -> getLbChannelType  () , row , typeColumn    , 1 , 1 );
        theGrid -> addWidget( scpu -> getPbNorth()         , row , northColumn   , 1 , 1 );
        theGrid -> addWidget( scpu -> getPbSouth()         , row , southColumn   , 1 , 1 );
        theGrid -> addWidget( scpu -> getPbEast()          , row , eastColumn    , 1 , 1 );
        theGrid -> addWidget( scpu -> getPbWest()          , row , westColumn    , 1 , 1 );

        scpu -> getLbChannelNumber() -> setText( QString::number( scpu -> getChannelNumber().get() ) );

        QObject::connect( scpu -> getPbNorth() , SIGNAL( clicked() ), scpu.data() , SLOT( handlePbNorth () ), Qt::UniqueConnection );
        QObject::connect( scpu -> getPbSouth() , SIGNAL( clicked() ), scpu.data() , SLOT( handlePbSouth () ), Qt::UniqueConnection );
        QObject::connect( scpu -> getPbEast()  , SIGNAL( clicked() ), scpu.data() , SLOT( handlePbEast  () ), Qt::UniqueConnection );
        QObject::connect( scpu -> getPbWest()  , SIGNAL( clicked() ), scpu.data() , SLOT( handlePbWest  () ), Qt::UniqueConnection );

        mSensorChannelPositionsUiVector.push_back( scpu );
    }
}

void SensorPositions::updateGui()
{
    if ( mData == 0 )
        return;

    for ( SensorChannelPositionsUiVector::const_iterator it = mSensorChannelPositionsUiVector.begin();
          it != mSensorChannelPositionsUiVector.end();
          ++it )
    {
        
        bool enabled = mData -> getSensorHWEnabledInputAtChannel( (*it) -> getChannelNumber() );

        (*it) -> getLbChannelType() -> setEnabled( enabled );
        (*it) -> getPbNorth()       -> setEnabled( enabled );
        (*it) -> getPbSouth()       -> setEnabled( enabled );
        (*it) -> getPbEast()        -> setEnabled( enabled );
        (*it) -> getPbWest()        -> setEnabled( enabled );

        (*it) -> getLbChannelType() -> setText( mData -> getSensorHWChannelTypeInputAtChannel( (*it) -> getChannelNumber() ) );
        (*it) -> getPbNorth()       -> setText( mData -> getSensorHWPosX2( (*it) -> getChannelNumber() ) );
        (*it) -> getPbSouth()       -> setText( mData -> getSensorHWPosX1( (*it) -> getChannelNumber() ) );
        (*it) -> getPbEast()        -> setText( mData -> getSensorHWPosY2( (*it) -> getChannelNumber() ) );
        (*it) -> getPbWest()        -> setText( mData -> getSensorHWPosY1( (*it) -> getChannelNumber() ) );
    }

    ui->chbAdaptSensorSpacing->setChecked(mData->getAdaptSensorSpacing());

    QPalette qplTmp = ui->chbAdaptSensorSpacing->palette();
    qplTmp.setColor (QPalette::WindowText, QColor (255, 0, 0));
    ui->chbAdaptSensorSpacing->setPalette (qplTmp);
}

void SensorPositions::handlePbNorth (const Types::Index& channelNumber)
{
    // declaration of variables
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, true, "Distance North [m]", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if (bOk == true)
        {
            mData->setSensorHWPosX2 (channelNumber, QString::number (dValue));

            // check, if link button is set. if so set the value for the parallel
            // channel, too.
            if (ui->pbLinkLFHF->isChecked() == true)
            {
                mData->setSensorHWPosX2 (Types::Index(channelNumber.getParallelChannel()), QString::number (dValue));
            }
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_INVALID_INPUT_VALUE), QMessageBox::Ok);
        }
    }
    delete (pclKeyPad);

    // deactivate the "use ADU sensor spacing" checkbox, as now the user entered
    // new sensor spacing values manually
    ui->chbAdaptSensorSpacing->setChecked (false);

    updateGui();
}

void SensorPositions::handlePbSouth( const Types::Index& channelNumber)
{
    // declaration of variables
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, true, "Distance South [m] (negative)", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if (bOk == true)
        {
            mData->setSensorHWPosX1 (channelNumber, QString::number (dValue));

            // check, if link button is set. if so set the value for the parallel
            // channel, too.
            if (ui->pbLinkLFHF->isChecked() == true)
            {
                mData->setSensorHWPosX1 (Types::Index(channelNumber.getParallelChannel()), QString::number (dValue));
            }
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_INVALID_INPUT_VALUE), QMessageBox::Ok);
        }
    }
    delete (pclKeyPad);

    // deactivate the "use ADU sensor spacing" checkbox, as now the user entered
    // new sensor spacing values manually
    ui->chbAdaptSensorSpacing->setChecked (false);

    updateGui();
}

void SensorPositions::handlePbEast( const Types::Index& channelNumber)
{
    // declaration of variables
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, true, "Distance East [m]", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if (bOk == true)
        {
            mData->setSensorHWPosY2 (channelNumber, QString::number (dValue));

            // check, if link button is set. if so set the value for the parallel
            // channel, too.
            if (ui->pbLinkLFHF->isChecked() == true)
            {
                mData->setSensorHWPosY2 (Types::Index(channelNumber.getParallelChannel()), QString::number (dValue));
            }
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_INVALID_INPUT_VALUE), QMessageBox::Ok);
        }
    }
    delete (pclKeyPad);

    // deactivate the "use ADU sensor spacing" checkbox, as now the user entered
    // new sensor spacing values manually
    ui->chbAdaptSensorSpacing->setChecked (false);

    updateGui();
}

void SensorPositions::handlePbWest( const Types::Index& channelNumber)
{
    // declaration of variables
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, true, "Distance West [m] (negative)", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if (bOk == true)
        {
            mData->setSensorHWPosY1 (channelNumber, QString::number (dValue));

            // check, if link button is set. if so set the value for the parallel
            // channel, too.
            if (ui->pbLinkLFHF->isChecked() == true)
            {
                mData->setSensorHWPosY1 (Types::Index(channelNumber.getParallelChannel()), QString::number (dValue));
            }
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_INVALID_INPUT_VALUE), QMessageBox::Ok);
        }
    }
    delete (pclKeyPad);

    // deactivate the "use ADU sensor spacing" checkbox, as now the user entered
    // new sensor spacing values manually
    ui->chbAdaptSensorSpacing->setChecked (false);

    updateGui();
}

void SensorPositions::showEvent ( QShowEvent * event )
{
    updateGui();
    QWidget::showEvent( event );
}

void SensorPositions::handleBackButton()
{
    emit goToJobTab();
}


void SensorPositions::on_chbAdaptSensorSpacing_stateChanged (int)
{
    this->mData->setAdaptSensorSpacing(ui->chbAdaptSensorSpacing->isChecked());
}


void SensorPositions::on_pbLinkLFHF_clicked (void)
{
    if (ui->pbLinkLFHF->isChecked() == true)
    {
        QIcon clIcon;
        clIcon.addFile(QString::fromUtf8(":/styles/Resources/pic/Locked.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->pbLinkLFHF->setIcon(clIcon);
    }
    else
    {
        QIcon clIcon;
        clIcon.addFile(QString::fromUtf8(":/styles/Resources/pic/Unlocked.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->pbLinkLFHF->setIcon(clIcon);
    }
}

