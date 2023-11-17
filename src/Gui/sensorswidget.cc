#include "sensorswidget.h"
#include <algorithm>
#include <QApplication>
#include <QLayout>
#include <QStringList>
#include <QMessageBox>
#include <fastbuttonselectpad.h>
#include "keypad.h"
#include "Strings.h"
#include "freekeypadwindow.h"

////////////////////////////////////////////////////////////////////////////////
//
// helper objects

// this is just for init the gui!
struct SetupChannels
{

    unsigned int mChannelNumber;

    explicit SetupChannels() :
        mChannelNumber( 0 )
        {}

    void operator() ( SensorHwChannelUi::Spt cui ) {
            cui -> connectSignals();
            cui -> setChannelNumber( mChannelNumber++ );
            cui -> setEnabled( false );
            cui -> updateDisplay();
    }
};

struct UpdateChannels
{
    explicit UpdateChannels( DataModel* data ) :
        mData( data ) {}

    DataModel* mData;
    
    void operator() ( SensorHwChannelUi::Spt cui ) {

        // FIXME: is this needed at all now?
        if ( not mData -> getSensorHWEnabledInputAtChannel( cui -> getChannelNumber() ) ) {
            cui -> setEnabled( false );
            return;
        }

        cui -> setEnabled( true );
        cui -> updateDisplay();

    }
};

////////////////////////////////////////////////////////////////////////////////
//
// sensors widget

SensorsWidget::SensorsWidget(QWidget *parent) :
    QWidget(parent),
    mData( 0 )
{
    setupObjectPlumbing();

    setupChannels();

    // QObject::connect( mPb_back , SIGNAL( clicked() ) , this , SLOT( handleBackButton() ) , Qt::UniqueConnection );

}

SensorsWidget::~SensorsWidget()
{
}

void SensorsWidget::handleBackButton()
{
    emit goToJobTab();
}

void SensorsWidget::handleSensorConfigLoaded()
{
    SensorsWidget::handleSensorConfigChanged();
}

void SensorsWidget::handleSensorConfigChanged()
{
    if ( mData == 0 ) 
        return;

    int iIndex = Types::AdaptSensorTypes::None;
    for (unsigned int uiCounter = 0; (int)uiCounter < this->pcbAdaptSensorType->count(); uiCounter++)
    {
        if (Types::qstrlAdaptSensorTypesStrings [uiCounter].compare (mData->getAdaptSensorType()) == 0)
        {
            iIndex = uiCounter;
        }
    }
    this->pcbAdaptSensorType->setCurrentIndex(iIndex);

    UpdateChannels updateChannels( mData ) ;
    std::for_each( mSensorHwChannelUiVector.begin(), mSensorHwChannelUiVector.end() , updateChannels );
}

void SensorsWidget::setupChannels()
{
    SetupChannels setupChannels;

    std::for_each( mSensorHwChannelUiVector.begin(), mSensorHwChannelUiVector.end() , setupChannels );
}

void SensorsWidget::setEnabledForChannelTo( const Types::Index& channel, bool enabled )
{
    try {
        mSensorHwChannelUiVector.at( channel.get() ) -> setEnabled( enabled );
    } catch ( const std::out_of_range& e ) {
        qDebug()  << __PRETTY_FUNCTION__ << " " << e.what() << " requested channel : " << channel.get() << " channel count : " << mSensorHwChannelUiVector.size();
        throw;
    }
}

void SensorsWidget::setupObjectPlumbing()
{
    const int inputColumn              = 0;
    const int channelColumn            = 1;
    const int nameColumn               = 2;
    const int sensorTypeColumn         = 3;
    const int sensorSerialNumberColumn = 4;

    QFrame* pFrameSensors    = new QFrame();
    QFrame* pFrameLinkButton = new QFrame();

    this->pbLinkLFHF = new QPushButton ();

    pFrameLinkButton->setMinimumWidth(95);
    pFrameLinkButton->setMaximumWidth(95);
    pFrameLinkButton->setFrameStyle(QFrame::Raised);
    pFrameLinkButton->setLayout(new QHBoxLayout());
    pFrameLinkButton->layout()->addWidget(this->pbLinkLFHF);
    this->pbLinkLFHF->setMaximumWidth(85);
    this->pbLinkLFHF->setSizePolicy  (QSizePolicy::Fixed, QSizePolicy::Expanding);
    this->pbLinkLFHF->setCheckable   (true);
    this->pbLinkLFHF->setChecked     (true);
    connect (pbLinkLFHF, SIGNAL(clicked()), this, SLOT(on_pbLinkLFHF_clicked()));
    this->on_pbLinkLFHF_clicked();

    mInnerHBoxLayout = new QHBoxLayout(this);

    mInnerHBoxLayout->addWidget(pFrameSensors);
    mInnerHBoxLayout->addWidget(pFrameLinkButton);

    mInnerGridLayout = new QGridLayout(pFrameSensors);

    QLabel* lb_input_hl = new QLabel(this);
    lb_input_hl->setObjectName(QString::fromUtf8("lb_input_hl"));
    lb_input_hl->setAlignment(Qt::AlignCenter);
    lb_input_hl->setText(QApplication::translate("SensorsWidget", "Input", 0));

    QLabel* lb_channel_hl = new QLabel(this);
    lb_channel_hl->setObjectName(QString::fromUtf8("lb_channel_hl"));
    lb_channel_hl->setAlignment(Qt::AlignCenter);
    lb_channel_hl->setText(QApplication::translate("SensorsWidget", "Channel", 0));

    QLabel* lb_name_hl = new QLabel(this);
    lb_name_hl->setObjectName(QString::fromUtf8("lb_name_hl"));
    lb_name_hl->setAlignment(Qt::AlignCenter);
    lb_name_hl->setText(QApplication::translate("SensorsWidget", "Name", 0));

    QLabel* lb_sensor_type_hl = new QLabel(this);
    lb_sensor_type_hl->setObjectName(QString::fromUtf8("lb_sensor_type_hl"));
    lb_sensor_type_hl->setAlignment(Qt::AlignCenter);
    lb_sensor_type_hl->setText(QApplication::translate("SensorsWidget", "Sensor Type", 0));

    QLabel* lb_sensor_serial_hl = new QLabel(this);
    lb_sensor_serial_hl -> setObjectName(QString::fromUtf8("lb_sensor_serial_hl"));
    lb_sensor_serial_hl -> setAlignment(Qt::AlignCenter);
    lb_sensor_serial_hl -> setText(QApplication::translate("SensorsWidget", "Serial Number", 0));

    mInnerGridLayout -> addWidget(lb_input_hl         , 0, inputColumn              , 1, 1 );
    mInnerGridLayout -> addWidget(lb_channel_hl       , 0, channelColumn            , 1, 1 );
    mInnerGridLayout -> addWidget(lb_name_hl          , 0, nameColumn               , 1, 1 );
    mInnerGridLayout -> addWidget(lb_sensor_type_hl   , 0, sensorTypeColumn         , 1, 1 );
    //mInnerGridLayout -> addWidget(lb_sensor_config_hl , 0, sensorConfigColumn       , 1, 1 );
    mInnerGridLayout -> addWidget(lb_sensor_serial_hl , 0, sensorSerialNumberColumn , 1, 1 );

    SensorHwChannelUi::Spt cui;

    unsigned guiRow = 0;

    for ( unsigned channel = 0; channel  < DefaultValues::MaxChannels; ++channel ) {

        QPushButton* pb_Input         = new QPushButton( this );
        pb_Input -> setText( DefaultValues::Undefined );
        pb_Input->setToolTip    (tr(C_STRING_TT_SENSOR_CONF_INPUT));

        QLabel* lb_channel       = new QLabel( this );
        lb_channel -> setText( QString::number( channel ) );
        lb_channel -> setAlignment  ( Qt::AlignCenter );
        lb_channel -> setFrameShape( QFrame::StyledPanel  );
        lb_channel -> setFrameShadow( QFrame::Raised  ); // QFrame::Sunken ?

        QPushButton* pb_sensor_type   = new QPushButton( this );
        pb_sensor_type -> setText( DefaultValues::DefaultSensor );
        pb_sensor_type->setToolTip(tr (C_STRING_TT_SENSOR_CONF_TYPE));
        
        QPushButton* pb_name          = new QPushButton( this );
        pb_name -> setText( DefaultValues::DefaultChannelType );
        pb_name->setToolTip (tr(C_STRING_TT_SENSOR_CONF_NAME));

        QPushButton* pb_sensor_serial = new QPushButton( this );
        pb_sensor_serial -> setText( DefaultValues::DefaultSerialNumber );
        
        guiRow = channel + 1;
        
        mInnerGridLayout -> addWidget( pb_Input         , guiRow, inputColumn              , 1, 1 );
        mInnerGridLayout -> addWidget( lb_channel       , guiRow, channelColumn            , 1, 1 );
        mInnerGridLayout -> addWidget( pb_name          , guiRow, nameColumn               , 1, 1 );
        mInnerGridLayout -> addWidget( pb_sensor_type   , guiRow, sensorTypeColumn         , 1, 1 );
        mInnerGridLayout -> addWidget( pb_sensor_serial , guiRow, sensorSerialNumberColumn , 1, 1 );

        cui = SensorHwChannelUiFactory::create( this );
        (*cui)
            .setPbInput( pb_Input )
            .setPbSensorSerialNumber( pb_sensor_serial )
            .setPbSensorType( pb_sensor_type ) 
            .setPbChannelName( pb_name)
            .setLbChannelNumber( lb_channel )
            .setChannelNumber( channel );

        mSensorHwChannelUiVector.push_back( cui );
        connect (((SensorHwChannelUi*)mSensorHwChannelUiVector.back().data ()), SIGNAL(sigUpdateDisplayAllChannels()), this, SLOT(slotUpdateDisplayAllChannels()));

    }

    // add combo box to select the update of the Sensor Type
    QFrame* pfrTmp = new QFrame (this);
    pfrTmp->setFrameStyle(QFrame::Panel);
    pfrTmp->setFrameShape(QFrame::Panel);
    pfrTmp->setFrameShadow(QFrame::Raised);
    pfrTmp->setLayout(new QVBoxLayout ());

    this->pcbAdaptSensorType = new QComboBox (this);
    this->pcbAdaptSensorType->insertItem(0, C_STRING_GUI_SENSOR_TYPE_ADAPT_NONE);
    this->pcbAdaptSensorType->insertItem(1, C_STRING_GUI_SENSOR_TYPE_ADAPT_E_SERIES);
    this->pcbAdaptSensorType->insertItem(2, C_STRING_GUI_SENSOR_TYPE_ADAPT_ALL);

    connect (this->pcbAdaptSensorType,
             SIGNAL (currentIndexChanged (int)),
             this,
             SLOT (handleAdaptSensorType()));

    pfrTmp->layout()->addWidget(this->pcbAdaptSensorType);
    mInnerGridLayout->addWidget(pfrTmp, ++guiRow, 0, 1, 6);

/*
    mLine = new QFrame( this );
    mLine->setObjectName(QString::fromUtf8("line"));
    mLine->setFrameShape(QFrame::HLine);
    mLine->setFrameShadow(QFrame::Sunken);
    mInnerGridLayout -> addWidget( mLine, ++guiRow, 0 , 1, 6 );
*/
}


void SensorsWidget::handleAdaptSensorType ()
{
    this->mData->setAdaptSensorType(Types::qstrlAdaptSensorTypesStrings [this->pcbAdaptSensorType->currentIndex()]);
}


void SensorsWidget::on_pbLinkLFHF_clicked (void)
{
    if (this->pbLinkLFHF->isChecked() == true)
    {
        QIcon clIcon;
        clIcon.addFile(QString::fromUtf8(":/styles/Resources/pic/Locked.png"), QSize(), QIcon::Normal, QIcon::Off);
        this->pbLinkLFHF->setIcon(clIcon);
        this->pbLinkLFHF->setIconSize(QSize (70,300));
    }
    else
    {
        QIcon clIcon;
        clIcon.addFile(QString::fromUtf8(":/styles/Resources/pic/Unlocked.png"), QSize(), QIcon::Normal, QIcon::Off);
        this->pbLinkLFHF->setIcon(clIcon);
        this->pbLinkLFHF->setIconSize(QSize (70,300));
    }
}


void SensorsWidget::slotUpdateDisplayAllChannels (void)
{
    UpdateChannels updateChannels(mData);
    std::for_each(mSensorHwChannelUiVector.begin(), mSensorHwChannelUiVector.end() , updateChannels );
}



////////////////////////////////////////////////////////////////////////////////
//
// channel stuff...

SensorHwChannelUi& SensorHwChannelUi::setEnabled( bool enable ) 
{
    mIsEnabled = enable;

    mPbInput              -> setEnabled( true   );
    mLbChannelNumber      -> setEnabled( enable );
    mPbChannelName        -> setEnabled( enable );
    mPbSensorType         -> setEnabled( enable );
    mPbSensorSerialNumber -> setEnabled( enable ); 
    return *this;
}

void SensorHwChannelUi::updateDisplay()
{
    DataModel* data = mParent -> getDataModel();
    if ( data == 0 )
        return;

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Input........:" << data -> getSensorHWSelectedInputAtChannel    (getChannelNumber());
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Channel Name.:" << data -> getSensorHWChannelTypeInputAtChannel (getChannelNumber());
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Sensor Type..:" << data -> getSensorHWSensorTypeInputAtChannel  (getChannelNumber());
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] Sensor Serial:" << data -> getSensorHWSerialInputAtChannel      (getChannelNumber());

    mPbInput              -> setText( data -> getSensorHWSelectedInputAtChannel    ( getChannelNumber() ) );
    mPbChannelName        -> setText( data -> getSensorHWChannelTypeInputAtChannel ( getChannelNumber() ) );
    mPbSensorType         -> setText( data -> getSensorHWSensorTypeInputAtChannel  ( getChannelNumber() ) );
    mPbSensorSerialNumber -> setText( data -> getSensorHWSerialInputAtChannel      ( getChannelNumber() ) );

    setEnabled( data -> getSensorHWEnabledInputAtChannel( getChannelNumber() ) );

}

void SensorHwChannelUi::connectSignals()
{
    QObject::connect( mPbInput               , SIGNAL( clicked() ) , this , SLOT( handlePbInput()              ) , Qt::UniqueConnection );
    QObject::connect( mPbChannelName         , SIGNAL( clicked() ) , this , SLOT( handlePbChannelName()        ) , Qt::UniqueConnection );
    QObject::connect( mPbSensorType          , SIGNAL( clicked() ) , this , SLOT( handlePbSensorType()         ) , Qt::UniqueConnection );
    QObject::connect( mPbSensorSerialNumber  , SIGNAL( clicked() ) , this , SLOT( handlePbSensorSerialNumber() ) , Qt::UniqueConnection );
}

SensorHwChannelUi&  SensorHwChannelUi::setSensorType( const QString& sensorType )
{
    mSensorType = sensorType;
    return *this;
}

DataModel* SensorHwChannelUi::getDataModel() const 
{ 
    return getIsEnabled() ? mParent -> getDataModel() : 0 ; 
}

// SLOT
void SensorHwChannelUi::handlePbInput()
{
    DataModel* data = mParent->getDataModel ();
    if (data == 0)
    {
        return;
    }

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] channel number:" << getChannelNumber();
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] next value....:" << getNextValue (DefaultValues::SelectedInputVector(), data->getSensorHWSelectedInputAtChannel(getChannelNumber()));
    qDebug () << "[" << __PRETTY_FUNCTION__ << "] actual value..:" << data->getSensorHWSelectedInputAtChannel(getChannelNumber());

    data->setSensorHWSelectedInputAtChannel (getChannelNumber(),
                                             getNextValue (DefaultValues::SelectedInputVector(),
                                             data->getSensorHWSelectedInputAtChannel(getChannelNumber())));

    if (this->mParent->pbLinkLFHF->isChecked() == true)
    {
        data->setSensorHWSelectedInputAtChannel (DefaultValues::aruiParallelChannels[getChannelNumber()],
                                                 getNextValue (DefaultValues::SelectedInputVector(),
                                                 data->getSensorHWSelectedInputAtChannel(DefaultValues::aruiParallelChannels[getChannelNumber()])));
    }

    updateDisplay();
    emit (this->sigUpdateDisplayAllChannels());
}

// SLOT
void SensorHwChannelUi::handlePbChannelName()
{
    // declaration of variables
    QStringList qstrlValidChannelNames;
    size_t      sCount;

    DataModel* pclData = mParent->getDataModel();
    if (pclData != NULL)
    {
        qstrlValidChannelNames.clear();
        for (sCount = 0; sCount < DefaultValues::ChannelTypeVector().size(); sCount++)
        {
            qstrlValidChannelNames.append(DefaultValues::ChannelTypeVector() [sCount]);
        }

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidChannelNames, "Select Sensor Name");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            pclData->setSensorHWChannelTypeInputAtChannel(getChannelNumber(), pclKeyPad->qstrRetValue);

            if (this->mParent->pbLinkLFHF->isChecked() == true)
            {
                pclData->setSensorHWChannelTypeInputAtChannel(DefaultValues::aruiParallelChannels[getChannelNumber()], pclKeyPad->qstrRetValue);
            }
        }

        delete (pclKeyPad);
    }

    updateDisplay();
    emit (this->sigUpdateDisplayAllChannels());
}


// SLOT
void SensorHwChannelUi::handlePbSensorType()
{
    // declaration of variables
    QStringList qstrlValidSensorTypes;
    size_t      sCount;

    DataModel* pclData = mParent->getDataModel();
    if (pclData != NULL)
    {
        qstrlValidSensorTypes.clear();
        for (sCount = 0; sCount < DefaultValues::SensorTypeVector().size(); sCount++)
        {
            qstrlValidSensorTypes.append(DefaultValues::SensorTypeVector() [sCount]);
        }

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidSensorTypes, "Select Sensor Type");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            // check, if a FGS or SHFT sensor was selected, if so, set input to 1 before
            if ((pclKeyPad->qstrRetValue.contains ("FGS")) ||
                (pclKeyPad->qstrRetValue.contains ("SHFT")))
            {
                if (pclData->getSensorHWSelectedInputAtChannel (getChannelNumber()).toUInt () != 1)
                {
                    pclData->setSensorHWSelectedInputAtChannel (getChannelNumber(),
                                                                getNextValue (DefaultValues::SelectedInputVector(),
                                                                pclData->getSensorHWSelectedInputAtChannel(getChannelNumber())));
                }

                if (this->mParent->pbLinkLFHF->isChecked() == true)
                {
                    if (pclData->getSensorHWSelectedInputAtChannel (DefaultValues::aruiParallelChannels[getChannelNumber()]).toUInt () != 1)
                    {
                        pclData->setSensorHWSelectedInputAtChannel (DefaultValues::aruiParallelChannels[getChannelNumber()],
                                                                    getNextValue (DefaultValues::SelectedInputVector(),
                                                                    pclData->getSensorHWSelectedInputAtChannel(DefaultValues::aruiParallelChannels[getChannelNumber()])));
                    }
                }
            }

            pclData->setSensorHWSensorTypeInputAtChannel(getChannelNumber(), pclKeyPad->qstrRetValue);

            if (this->mParent->pbLinkLFHF->isChecked() == true)
            {
                pclData->setSensorHWSensorTypeInputAtChannel(DefaultValues::aruiParallelChannels[getChannelNumber()], pclKeyPad->qstrRetValue);
            }

        }

        delete (pclKeyPad);
    }

    updateDisplay();
    emit (this->sigUpdateDisplayAllChannels());
}

void SensorHwChannelUi::handlePbSensorSerialNumber()
{
    DataModel* data = mParent -> getDataModel();
    if ( data == 0 )
        return;

    // declaration of variables
    int     iValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Enter Sensor Serial Number", NULL);
    if (pclKeyPad->exec() == true)
    {
        iValue = pclKeyPad->text().toInt(&bOk);
        if (bOk == true)
        {
            data -> setSensorHWSerialInputAtChannel(getChannelNumber() , QString::number(iValue));

            if (this->mParent->pbLinkLFHF->isChecked() == true)
            {
                data -> setSensorHWSerialInputAtChannel(DefaultValues::aruiParallelChannels[getChannelNumber()], QString::number(iValue));
            }
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(NULL, qstrTmp, tr (C_STRING_GUI_INVALID_INPUT_VALUE), QMessageBox::Ok);
        }
    }
    delete (pclKeyPad);

    updateDisplay();
    emit (this->sigUpdateDisplayAllChannels());
}


////////////////////////////////////////////////////////////////////////////////
//
// helper stuff

QString SensorHwChannelUi::getNextValue( const DefaultValues::VectorOfQStrings& v, const QString& value )
{
    if ( v.empty() )
        return DefaultValues::Undefined;

    DefaultValues::VectorOfQStrings::const_iterator it = std::find( v.begin(),v.end(), value );
    
    if ( it == v.end() )
        return (*v.begin());

    return  ( ++it == v.end() ) ? (*v.begin() ) : (*it) ;

    
}
