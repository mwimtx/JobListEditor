#include "jobconfigwidget.h"
#include "ui_jobconfigwidget.h"
#include "default_values.h"
#include <QDebug>
#include <QErrorMessage>
#include <QMessageBox>
#include <algorithm>
#include <QSettings>
#include <QFileDialog>

#include "discretevalueselector.h"
#include "fastbuttonselectpad.h"
#include "codemapper.h"
#include "keypad.h"
#include "Strings.h"
#include "freekeypadwindow.h"
#include "importautogainoffsetdialog.h"
#include "tools_types.h"

QString nameSuffix( const QString& prefix, unsigned int suffix )
{
    return prefix + "_" + QString::number( suffix );
}

////////////////////////////////////////////////////////////////////////////////
//
// ChannelConfigUi

const unsigned int channelEnabledColumn = 0;
const unsigned int extGainColumn        = 1;
const unsigned int chopperColumn        = 2;
const unsigned int rfFilterColumn       = 3;
const unsigned int gain1Column          = 4;
const unsigned int dacColumn            = 5;
const unsigned int lpHpFilterColumn     = 6;
const unsigned int gain2Column          = 7;

ChannelConfigUi::ChannelConfigUi( JobConfigWidget* parent , const Types::Index& channel ) :
    QWidget( parent ),
    mJobConfigWidget( parent ),
    mChannelNumber( channel )
{}


ChannelConfigUi& ChannelConfigUi::setupGui()
{

    QGridLayout* theGrid = mJobConfigWidget -> getButtonGrid(); 

    int guiRow = mStartRow + mChannelNumber.get();

    pb_channel_enabled = new QPushButton( mJobConfigWidget );
    pb_channel_enabled -> setObjectName( nameSuffix( "pb_channel_enable" , guiRow ));
    pb_channel_enabled -> setText( QString::number( mChannelNumber.get() ) );
    pb_channel_enabled -> setCheckable( true );
    pb_channel_enabled->setToolTip(tr(C_STRING_TT_CH_CONFIG_ENABLE));

    theGrid -> addWidget( pb_channel_enabled , guiRow , channelEnabledColumn , 1, 1);

    pb_ext_gain = new QPushButton( mJobConfigWidget );
    pb_ext_gain -> setObjectName(nameSuffix( "pb_ext_gain" , mChannelNumber.get() ));
    pb_ext_gain -> setText( QApplication::translate("JobConfigWidget", "ext Gain", 0)) ;
    pb_ext_gain->setToolTip(tr(C_STRING_TT_CH_CONFIG_EXT_GAIN));
    theGrid -> addWidget( pb_ext_gain , guiRow, extGainColumn, 1 , 1 );

    pb_chopper = new QPushButton( mJobConfigWidget );
    pb_chopper -> setObjectName(nameSuffix( "pb_chopper" , mChannelNumber.get() ));
    pb_chopper -> setText( QApplication::translate("JobConfigWidget", "chopper", 0)) ;
    pb_chopper->setToolTip(tr(C_STRING_TT_CH_CONFIG_CHOPPER));
    theGrid -> addWidget( pb_chopper, guiRow, chopperColumn, 1, 1 );

    pb_rf_filter = new QPushButton( mJobConfigWidget );
    pb_rf_filter -> setObjectName(nameSuffix( "pb_rf_filter" , mChannelNumber.get() ));
    pb_rf_filter -> setText( QApplication::translate("JobConfigWidget", "RF filter", 0)) ;
    pb_rf_filter->setToolTip(tr(C_STRING_TT_CH_CONFIG_RF_FILTER));
    theGrid -> addWidget( pb_rf_filter, guiRow, rfFilterColumn, 1, 1 );

    pb_gain1 = new QPushButton( mJobConfigWidget );
    pb_gain1 -> setObjectName(nameSuffix( "pb_gain1" , mChannelNumber.get() ));
    pb_gain1 -> setText( QApplication::translate("JobConfigWidget", "Gain 1", 0)) ;
    pb_gain1->setToolTip(tr(C_STRING_TT_CH_CONFIG_GAIN1));
    theGrid -> addWidget( pb_gain1, guiRow, gain1Column, 1, 1 );

    pb_dac = new QPushButton( mJobConfigWidget );
    pb_dac -> setObjectName(nameSuffix( "pb_dac" , mChannelNumber.get() ));
    pb_dac -> setText( QApplication::translate("JobConfigWidget", "DAC", 0)) ;
    pb_dac->setToolTip(tr(C_STRING_TT_CH_CONFIG_DC_OFFSET_CORR));
    theGrid -> addWidget( pb_dac, guiRow, dacColumn , 1, 1 );

    pb_lp_hp_filter = new QPushButton( mJobConfigWidget );
    pb_lp_hp_filter -> setObjectName(nameSuffix( "pb_lp_hp_filter" , mChannelNumber.get() ));
    pb_lp_hp_filter -> setText( QApplication::translate("JobConfigWidget", "LP/HP Filter", 0)) ;
    pb_lp_hp_filter->setToolTip(tr(C_STRING_TT_CH_CONFIG_LP_HP));
    theGrid -> addWidget( pb_lp_hp_filter, guiRow, lpHpFilterColumn , 1, 1 );

    pb_gain2 = new QPushButton( mJobConfigWidget );
    pb_gain2 -> setObjectName(nameSuffix( "pb_gain2" , mChannelNumber.get() ));
    pb_gain2 -> setText( QApplication::translate("JobConfigWidget", "Gain 2", 0)) ;
    pb_gain2->setToolTip(tr(C_STRING_TT_CH_CONFIG_GAIN2));
    theGrid -> addWidget( pb_gain2 , guiRow, gain2Column, 1, 1 );

    return *this;

}


void ChannelConfigUi::setupSignals()
{
    QObject::connect( pb_channel_enabled , SIGNAL( clicked() ) , this , SLOT( handle_pb_pb_channel_enabled () )  , Qt::UniqueConnection );
    QObject::connect( pb_ext_gain        , SIGNAL( clicked() ) , this , SLOT( handle_pb_ext_gain() )             , Qt::UniqueConnection );
    QObject::connect( pb_chopper         , SIGNAL( clicked() ) , this , SLOT( handle_pb_chopper() )              , Qt::UniqueConnection );
    QObject::connect( pb_rf_filter       , SIGNAL( clicked() ) , this , SLOT( handle_pb_rf_filter() )            , Qt::UniqueConnection );
    QObject::connect( pb_gain1           , SIGNAL( clicked() ) , this , SLOT( handle_pb_gain1() )                , Qt::UniqueConnection );
    QObject::connect( pb_dac             , SIGNAL( clicked() ) , this , SLOT( handle_pb_dac() )                  , Qt::UniqueConnection );
    QObject::connect( pb_lp_hp_filter    , SIGNAL( clicked() ) , this , SLOT( handle_pb_lp_hp_filter() )         , Qt::UniqueConnection );
    QObject::connect( pb_gain2           , SIGNAL( clicked() ) , this , SLOT( handle_pb_gain2() )                , Qt::UniqueConnection );
}

void ChannelConfigUi::handle_pb_pb_channel_enabled()
{
    mJobConfigWidget-> handleEnableChannel( getChannelNumber(), pb_channel_enabled -> isChecked() );
}

void ChannelConfigUi::handle_pb_ext_gain()
{
    mJobConfigWidget -> setExtGainForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_chopper()
{
    mJobConfigWidget -> setChopperForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_rf_filter()
{
    mJobConfigWidget -> setRfFilterForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_gain1()
{
    mJobConfigWidget -> setGain1ForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_dac()
{
    mJobConfigWidget -> setDacForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_lp_hp_filter()
{
    mJobConfigWidget -> setLpHpFilterForChannel( getChannelNumber() );
}
void ChannelConfigUi::handle_pb_gain2()
{
    mJobConfigWidget -> setGain2ForChannel( getChannelNumber() );
}

////////////////////////////////////////////////////////////////////////////////
//
// JobConfigWidget helper : update gui stuff

struct UpdateChannelStatusFOB
{
    explicit UpdateChannelStatusFOB( DataModel* model , Tools::CodeMapper* codeMapper ) :        
        mModel( model ) ,
        mCodeMapper( codeMapper )
        {}

    DataModel* mModel;
    Tools::CodeMapper* mCodeMapper;

    void operator() ( ChannelConfigUi* channelUi ) {

        JobChannelConfig* channelCfg = 0;

        SensorInfo::Spt sensorSpt     = mModel ->  getSensorInfoForChannel  ( channelUi -> getChannelNumber() );        
        Job::Spt currentJob           = mModel ->  getSelectedJob(); 

        if ( sensorSpt.isNull() or currentJob.isNull() ) {
            channelUi -> pb_channel_enabled -> setEnabled( false );
            updateGuiElements( channelUi, channelCfg );
            return;
        }

        channelUi -> pb_channel_enabled -> setEnabled( true ); // enable the switch...
        channelCfg = currentJob -> getJobChannelConfigForChannel( channelUi -> getChannelNumber().get() );
        updateGuiElements( channelUi, channelCfg );

    }

    void updateGuiElements( ChannelConfigUi* channelUi,  JobChannelConfig* channelCfg )
    {

        bool enabled      = ( not channelCfg == 0 ) and channelCfg -> isEnabled();
        bool dacAvailable = enabled and channelCfg -> getHasDac();

        channelUi -> pb_channel_enabled -> setChecked( enabled );

        channelUi -> pb_ext_gain     -> setEnabled( enabled );
        channelUi -> pb_chopper      -> setEnabled( enabled );
        channelUi -> pb_rf_filter    -> setEnabled( enabled );
        channelUi -> pb_lp_hp_filter -> setEnabled( enabled );

        // if AdaptChannelConfig is active, gains and DC offset correction is configured inside
        // ADU
        if (mModel->getAdaptChannelConfig() == true)
        {
            channelUi-> pb_gain1-> setEnabled (false);
            channelUi-> pb_gain2-> setEnabled (false);
            channelUi-> pb_dac-> setEnabled   (false);
        }
        else
        {
            channelUi->pb_gain1->setEnabled (enabled);
            channelUi->pb_gain2->setEnabled (enabled);
            channelUi->pb_dac->setEnabled   (dacAvailable);
        }

        // if we do not have a valid job config, stop here
        if (not enabled)
        {
            return;
        }

        channelUi -> pb_ext_gain     -> setText( channelCfg -> getExtGain() );
        channelUi -> pb_chopper      -> setText( channelCfg -> getHChopper() );
        channelUi -> pb_rf_filter    -> setText( mCodeMapper -> toVisual( channelCfg -> getRfFilter() ) );
        channelUi -> pb_lp_hp_filter -> setText( channelCfg -> getLpHpFilter() );

        // if AdaptChannelConfig is active, gains and DC offset correction is configured inside
        // ADU
        if (mModel->getAdaptChannelConfig() == true)
        {
            channelUi->pb_gain1-> setText (C_STRING_GUI_CHANNEL_CONF_AUTO);
            channelUi->pb_gain2-> setText (C_STRING_GUI_CHANNEL_CONF_AUTO);
            channelUi->pb_dac-> setText   (C_STRING_GUI_CHANNEL_CONF_AUTO);
        }
        else
        {
            channelUi->pb_gain1-> setText (channelCfg->getGain1());
            channelUi->pb_gain2-> setText (channelCfg->getGain2());
            channelUi->pb_dac-> setText   (channelCfg->getDacVal());
        }
    }

};

////////////////////////////////////////////////////////////////////////////////
//
// JobConfigWidget helper : populate gui initially

struct ChannelUiFactoryFOB
{

    JobConfigWidget* mJobConfigWidget;
    unsigned int mCounter;
    int mStartRowInGrid;

    explicit ChannelUiFactoryFOB( JobConfigWidget* jobConfigWidget , int startRowInGrid ) :
        mJobConfigWidget( jobConfigWidget ),
        mCounter( 0 ), 
        mStartRowInGrid( startRowInGrid )
        {}
        
    ChannelConfigUi* operator()() { 

        ChannelConfigUi* cui = new ChannelConfigUi( mJobConfigWidget , mCounter++ );

        (*cui)
            .setStartRowInGrid( mStartRowInGrid )
            .setStartColumnInGrid( 0 )
            .setupGui()
            .setupSignals();

        return cui;

    }
};

////////////////////////////////////////////////////////////////////////////////
//
// JobConfigWidget 

JobConfigWidget::JobConfigWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::JobConfigWidget),
    mChannelConfigVector( DefaultValues::MaxChannels ),
    mData( 0 )
{
    ui->setupUi(this);
    setupChannelConfigGrid();
    setupViews();

    addToolTips();

    mCodeMapperLF
        .addMapping( "LF_RF_1" , "3000" )
        .addMapping( "LF_RF_2" , "1000" )
        .addMapping( "LF_RF_3" , "500" )
        .addMapping( "LF_RF_4" , "200" );

    mCodeMapperMF
        .addMapping( "ADU07_MF_RF_1" , "3000" )
        .addMapping( "ADU07_MF_RF_2" , "500" );
}

// SLOT
JobConfigWidget::~JobConfigWidget()
{
    delete ui;
}

// SLOT
void JobConfigWidget::handleJobDataLoaded()
{
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__;
    #endif
    if ( mData == 0 )
        return;
    try {
        ui -> jobListTableView -> setModel          ( mData -> getJobListModel() );
        ui -> jobListTableView -> setSelectionModel ( mData -> getSelectionModel() );
        ui -> jobListTableView -> setColumnHidden   ( 2 , true ); // hide duration column    
        ui -> jobListTableView -> setColumnHidden   ( 3 , true );
        ui -> jobListTableView -> setColumnHidden   ( 4 , true );
        ui -> jobListTableView -> setColumnHidden   ( 5 , true );
        ui->jobListTableView->resizeRowsToContents   ();
        ui->jobListTableView->setAlternatingRowColors(true);

        QObject::connect( mData -> getSelectionModel(), 
                          SIGNAL( selectionChanged ( const QItemSelection & , const QItemSelection &  ) ),
                          this ,
                          SLOT( jobSelectionChanged( const QItemSelection & , const QItemSelection & ) ),
                          Qt::UniqueConnection );
        
        QObject::connect( mData , SIGNAL( jobDataChanged() ), this , SLOT( handleJobDataChanged() ) , Qt::UniqueConnection );
    } catch ( const std::runtime_error& e ) {
        qDebug() << __PRETTY_FUNCTION__  << e.what();
        QErrorMessage em;
        em.showMessage( QString( e.what() ) );
        em.exec();
    }

}

// SLOT
void JobConfigWidget::handleJobDataChanged()
{
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__;
    #endif
    updateChannelStatus();
}

// SLOT
void JobConfigWidget::handleSensorConfigLoaded()
{
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__;
    #endif
    handleSensorConfigChanged();
}

void JobConfigWidget::handleSensorConfigChanged()
{
    updateChannelStatus();
}

void JobConfigWidget::handleEnableChannel( const Types::Index& channelNumber, bool enable )
{
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__ ;
    #endif
    mData -> enableBoardChannelForCurrentJob( channelNumber.get() , enable );
}

void JobConfigWidget::setExtGainForChannel ( const Types::Index& channelNumber )
{
    // declaration of variables
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, false, "External Gain [satellite box]", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if (bOk == true)
        {
            mData->setExtGainForChannel (channelNumber, QString::number (dValue));
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
}

void JobConfigWidget::setChopperForChannel           ( const Types::Index&  channelNumber )
{
    if ( mData == 0 )
        return;
    
    QStringList chopperList = mData -> allowedChopperValuesAtChannel( channelNumber );
    QString currentChopper  = mData -> getChopperForChannel( channelNumber );
    int index = chopperList.indexOf( currentChopper );
    if ( index == -1 ) // current value not found for unknown reasons...
        mData -> setChopperForChannel( channelNumber , chopperList.empty() ? "-" : chopperList[ 0 ] );
    bool isLast = ( index == ( chopperList.size() - 1 ) );
    index = isLast ? 0 : index + 1;

    mData -> setChopperForChannel( channelNumber , chopperList[ index ] );
}

void JobConfigWidget::setRfFilterForChannel          ( const Types::Index& channelNumber )
{
    if ( mData == 0 )
        return;
    QStringList filterList = mData -> allowedRfFilterValuesAtChannel( channelNumber );
    QString     currentValue = mData -> getRfFilterForChannel( channelNumber );
    int         index = filterList.indexOf( currentValue );
    if ( index == -1 ) { // current value not found: 
        mData -> setRfFilterForChannel( channelNumber , DefaultValues::Off );
        return;
    }
    bool isLast = ( index == ( filterList.size() - 1 ) );
    index = isLast ? 0 : index + 1;

    mData -> setRfFilterForChannel( channelNumber , filterList[ index ] );
}

void JobConfigWidget::setGain1ForChannel (const Types::Index& channelNumber)
{
    // declaration of variables
    QStringList qstrlValidGains;

    if (mData != NULL)
    {
        qstrlValidGains = mData ->allowedDiscreteValuesForGain1AtChannel(channelNumber);

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidGains, "Select GainStage-1 Gain");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            mData->setGain1ForChannel (channelNumber.get(), pclKeyPad->qstrRetValue);
        }

        delete (pclKeyPad);
    }
}

void JobConfigWidget::setDacForChannel ( const Types::Index&  channelNumber )
{
    if ( mData == 0 )
        return; 

    QString dacMin     = mData -> getMinDacAtChannel( channelNumber );
    QString dacMax     = mData -> getMaxDacAtChannel( channelNumber );
    QString currentDac = mData -> getDacForChannel( channelNumber );

    Keypad keyPad ( tr( "Dac") , Keypad::SignedIntegerMode );
    keyPad
        .setInitialValue( currentDac  )
        .setMaxIntValue( dacMax.toInt() )
        .setMinIntValue( dacMin.toInt () )
        .setClearOnFirstKey();

    if (not (keyPad.exec() == QDialog::Accepted))
    {
        return;
    }

    double newDac = keyPad.toDouble() ;

    mData -> setDacForChannel( channelNumber , QString::number( newDac ) );

}

void JobConfigWidget::setLpHpFilterForChannel        ( const Types::Index& channelNumber )
{
    if ( mData == 0 )
        return;

    QStringList filterList = mData -> allowedLpHpFilterValuesAtChannel( channelNumber );
    QString     currentValue = mData -> getLpHpFilterForChannel( channelNumber );
    int         index = filterList.indexOf( currentValue );

    if ( index == -1 ) { // current value not found: 
        mData -> setLpHpFilterForChannel( channelNumber , DefaultValues::Off );
        return;
    }

    bool isLast = ( index == ( filterList.size() - 1 ) );
    index = isLast ? 0 : index + 1;
    mData -> setLpHpFilterForChannel( channelNumber , filterList[ index ] );
}

void JobConfigWidget::setGain2ForChannel             ( const Types::Index& channelNumber )
{
    // declaration of variables
    QStringList qstrlValidGains;

    if (mData != NULL)
    {
        qstrlValidGains = mData ->allowedDiscreteValuesForGain2AtChannel(channelNumber);

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidGains, "Select GainStage-2 Gain");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            mData->setGain2ForChannel (channelNumber.get(), pclKeyPad->qstrRetValue);
        }

        delete (pclKeyPad);
    }
}



// SLOT
void JobConfigWidget::jobSelectionChanged( const QItemSelection & , const QItemSelection & )
{
    updateChannelStatus();
}


// SLOT
void JobConfigWidget::handleBackButton()
{
    emit goToJobTab();
}

void JobConfigWidget::showEvent ( QShowEvent * event ) {
    #ifdef DEBUG_OUTPUT
    qDebug()  << __PRETTY_FUNCTION__ ;
    #endif
    updateChannelStatus();
    QWidget::showEvent(event);
}

void JobConfigWidget::updateChannelStatus()
{
    #ifdef DEBUG_OUTPUT
        qDebug()  << __PRETTY_FUNCTION__;
    #endif

    if (mData == 0)
    {
        return;
    }

    // set / reset the adapt channel config checkbox
    ui->chbUseAutoGainAutoOffset->setChecked(mData->getAdaptChannelConfig());

    // if Joblist is made for TXM, hide AutoGain / AutoOffset buttons
    if (this->mData->isTXM() == true)
    {
        ui->chbUseAutoGainAutoOffset->setVisible(false);
        ui->pbImportAutoGainOffset->setVisible  (false);
    }
    else
    {
        ui->chbUseAutoGainAutoOffset->setVisible(true);
        ui->pbImportAutoGainOffset->setVisible  (true);
    }

    // update all buttons
    if (this->mData->isMF() == true)
    {
        UpdateChannelStatusFOB updateChannelStatusFOB (mData , &mCodeMapperMF);
        std::for_each(mChannelConfigVector.begin(), mChannelConfigVector.end(), updateChannelStatusFOB);
    }
    else
    {
        UpdateChannelStatusFOB updateChannelStatusFOB (mData , &mCodeMapperLF);
        std::for_each(mChannelConfigVector.begin(), mChannelConfigVector.end(), updateChannelStatusFOB);
    }
}


void JobConfigWidget::setupChannelConfigGrid()
{    

    QGridLayout* theGrid = getButtonGrid();

    QLabel* lChannel = new QLabel( this );
    lChannel -> setText( "Channel" );
    theGrid -> addWidget( lChannel , 1 , channelEnabledColumn , 1 , 1 );

    QLabel* lExtGain = new QLabel( this );
    lExtGain -> setText( "ext Gain" );
    theGrid -> addWidget( lExtGain , 1 , extGainColumn, 1 , 1 );

    QLabel* lChopper = new QLabel( this );
    lChopper -> setText( "Chopper" );
    theGrid -> addWidget( lChopper , 1 , chopperColumn , 1 , 1 );

    QLabel* lrfFilter = new QLabel( this );
    lrfFilter -> setText( "RF filter" );
    theGrid -> addWidget( lrfFilter , 1 , rfFilterColumn , 1 , 1 );

    QLabel* lgain1 = new QLabel( this );
    lgain1 -> setText( "Gain 1" );
    theGrid -> addWidget( lgain1 , 1 , gain1Column , 1 , 1 );

    QLabel* ldac = new QLabel( this );
    ldac -> setText( "DAC" );
    theGrid -> addWidget( ldac , 1 , dacColumn , 1 , 1 );

    QLabel* lLpHpFilter = new QLabel( this );
    lLpHpFilter -> setText( "LP/HP Filer" );
    theGrid -> addWidget( lLpHpFilter , 1 , lpHpFilterColumn , 1 , 1 );

    QLabel* lgain2 = new QLabel( this );
    lgain2 -> setText( "Gain 2" );
    theGrid -> addWidget( lgain2 , 1 , gain2Column , 1 , 1 );

    ChannelUiFactoryFOB fob( this , 2 );
    std::generate( mChannelConfigVector.begin(), mChannelConfigVector.end(), fob );
}

void JobConfigWidget::setupViews()
{
#if QT_VERSION >= 0x050000
    ui -> jobListTableView -> horizontalHeader() -> setSectionResizeMode(QHeaderView::Stretch);
#else
    ui -> jobListTableView -> horizontalHeader() -> setResizeMode(QHeaderView::Stretch);
#endif
    ui -> jobListTableView -> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui -> jobListTableView -> setSelectionMode(QTableView::SingleSelection);        
}

QGridLayout* JobConfigWidget::getButtonGrid() const 
{ 
    return ui -> gridLayout; 
}

void JobConfigWidget::on_atBeginButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }
    ui->jobListTableView->selectRow(0);
}


void JobConfigWidget::on_atEndButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }

    int newRow = ui->jobListTableView->model()->rowCount() - 1;
    ui->jobListTableView->selectRow(newRow);
}


void JobConfigWidget::on_oneBackButton_clicked ()
{
    QItemSelectionModel* selectionModel = ui->jobListTableView->selectionModel();
    QModelIndexList      selectionList  = selectionModel->selectedRows();

    if (selectionList.size() > 1)
    {
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    }

    if (selectionList.size() == 0)
    {
        return;
    }

    int newRow = selectionList[0].row() - 1;
    if (newRow < 0)
    {
        return;
    }

    ui->jobListTableView->selectRow(newRow);
}


void JobConfigWidget::on_oneForewardButton_clicked ()
{
    QItemSelectionModel* selectionModel = ui->jobListTableView->selectionModel();
    QModelIndexList      selectionList  = selectionModel->selectedRows();

    if (selectionList.size() > 1)
    {
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    }

    if (selectionList.size() == 0)
    {
        return;
    }

    int newRow = selectionList[0].row() + 1;
    if (newRow >= ui->jobListTableView->model()->rowCount())
    {
        return;
    }

    ui->jobListTableView->selectRow(newRow);
}


void JobConfigWidget::on_chbUseAutoGainAutoOffset_stateChanged (void)
{
    // declaration of variables
    QString qstrTmp;

    if (ui->chbUseAutoGainAutoOffset->isChecked() == true)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_CHANNEL_CONF_ACTIVATE_AUTO), QMessageBox::Ok);
    }

    mData->setAdaptChannelConfig(ui->chbUseAutoGainAutoOffset->isChecked());
    this->updateChannelStatus();
}


void JobConfigWidget::addToolTips (void)
{
    ui->frAutoGainOffset->setToolTip            (tr(C_STRING_TT_CH_CONFIG_AUTOGAIN));
    ui->chbUseAutoGainAutoOffset->setToolTip    (tr(C_STRING_TT_CH_CONFIG_AUTOGAIN));
}


void JobConfigWidget::on_pbImportAutoGainOffset_clicked (void)
{

    // set use auto gain from ADU to false in this case
    ui->chbUseAutoGainAutoOffset->setChecked(false);
    mData->setAdaptChannelConfig(ui->chbUseAutoGainAutoOffset->isChecked());
    this->updateChannelStatus();


    // declaration of variables
    QString                     qstrPathToXML   = "";
    QString                     qstrFilter;
    QString                     qstrTmp;
    ImportAutoGainOffsetDialog* pclDialog       = NULL;

    // try to get path to ADU07ChannelConfig XML file

    // read last open path from settings class
    pclSettings->beginGroup (C_SETTINGS_GROUP_IMPORT_AUTOGAIN);
    qstrTmp = pclSettings->value(C_SETTINGS_LAST_ADU07CHANNELCONFIG_DIR).toString();
    pclSettings->endGroup();

    qstrFilter    = "ADU07ChannelConfig File (ADU07ChannelConfig.xml)";
    qstrPathToXML = QFileDialog::getOpenFileName(this, C_STRING_GUI_OPEN_JOBLIST_CMP_DIALOG, qstrTmp, tr("ADU07ChannelConfig File (ADU07ChannelConfig.xml)"), &qstrFilter);
    if (qstrPathToXML.isEmpty() == false)
    {
        // store new path in settings object
        pclSettings->beginGroup(C_SETTINGS_GROUP_IMPORT_AUTOGAIN);
        pclSettings->setValue(C_SETTINGS_LAST_ADU07CHANNELCONFIG_DIR, qstrPathToXML);
        pclSettings->endGroup();

        if (pclDialog != NULL)
        {
            delete (pclDialog);
        }
        pclDialog = new ImportAutoGainOffsetDialog (this->mData, qstrPathToXML, this);

        pclDialog->setModal(true);
        pclDialog->exec    ();
        this->handleJobDataChanged();
        if (pclDialog != NULL)
        {
            delete (pclDialog);
        }


    }




}


