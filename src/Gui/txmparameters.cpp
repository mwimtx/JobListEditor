
#include <QErrorMessage>
#include <QMessageBox>

#include "txmparameters.h"
#include "ui_txmparameters.h"
#include "tools_types.h"
#include "codemapper.h"
#include "keypad.h"
#include "Strings.h"
#include "freekeypadwindow.h"



// ====================================================
// class TxmParametersUi
TxmParametersUi::TxmParametersUi(txmparameters* parent , const Types::Index& channel __attribute__((unused))) :
    QWidget              (parent),
    mTxmParametersWidget (parent)
{
}


TxmParametersUi& TxmParametersUi::setupGui()
{
    return *this;
}


void TxmParametersUi::setupSignals()
{
    /*
    QObject::connect( pb_channel_enabled , SIGNAL( clicked() ) , this , SLOT( handle_pb_pb_channel_enabled () )  , Qt::UniqueConnection );
    QObject::connect( pb_ext_gain        , SIGNAL( clicked() ) , this , SLOT( handle_pb_ext_gain() )             , Qt::UniqueConnection );
    QObject::connect( pb_chopper         , SIGNAL( clicked() ) , this , SLOT( handle_pb_chopper() )              , Qt::UniqueConnection );
    QObject::connect( pb_rf_filter       , SIGNAL( clicked() ) , this , SLOT( handle_pb_rf_filter() )            , Qt::UniqueConnection );
    QObject::connect( pb_gain1           , SIGNAL( clicked() ) , this , SLOT( handle_pb_gain1() )                , Qt::UniqueConnection );
    QObject::connect( pb_dac             , SIGNAL( clicked() ) , this , SLOT( handle_pb_dac() )                  , Qt::UniqueConnection );
    QObject::connect( pb_lp_hp_filter    , SIGNAL( clicked() ) , this , SLOT( handle_pb_lp_hp_filter() )         , Qt::UniqueConnection );
    QObject::connect( pb_gain2           , SIGNAL( clicked() ) , this , SLOT( handle_pb_gain2() )                , Qt::UniqueConnection );
    */
}

/*
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
*/

// ====================================================
// clas  TxmParameters helper : update gui stuff
struct UpdateTxmParametersStatusFOB
{
    explicit UpdateTxmParametersStatusFOB (DataModel* model, Tools::CodeMapper* codeMapper) :
        mModel      (model) ,
        mCodeMapper (codeMapper)
        {}

    DataModel* mModel;
    Tools::CodeMapper* mCodeMapper;

    void operator() (TxmParametersUi* channelUi __attribute__((unused)))
    {

        /*

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
        */

    }

    void updateGuiElements(TxmParametersUi* channelUi __attribute__((unused)),  JobChannelConfig* channelCfg __attribute__((unused)))
    {
        /*

        bool enabled      = ( not channelCfg == 0 ) and channelCfg -> isEnabled();
        bool dacAvailable = enabled and channelCfg -> getHasDac();

        channelUi -> pb_channel_enabled -> setChecked( enabled );

        channelUi -> pb_ext_gain     -> setEnabled( enabled );
        channelUi -> pb_chopper      -> setEnabled( enabled );
        channelUi -> pb_rf_filter    -> setEnabled( enabled );
        channelUi -> pb_gain1        -> setEnabled( enabled );
        channelUi -> pb_dac          -> setEnabled( dacAvailable );
        channelUi -> pb_lp_hp_filter -> setEnabled( enabled );
        channelUi -> pb_gain2        -> setEnabled( enabled );

        if ( not enabled )
            return;

        channelUi -> pb_ext_gain     -> setText( channelCfg -> getExtGain() );
        channelUi -> pb_chopper      -> setText( channelCfg -> getHChopper() );
        channelUi -> pb_rf_filter    -> setText( mCodeMapper -> toVisual( channelCfg -> getRfFilter() ) );
        channelUi -> pb_gain1        -> setText( channelCfg -> getGain1() );
        channelUi -> pb_dac          -> setText( channelCfg -> getDacVal() );
        channelUi -> pb_lp_hp_filter -> setText( channelCfg -> getLpHpFilter() );
        channelUi -> pb_gain2        -> setText( channelCfg -> getGain2() );
        */

    }

};

////////////////////////////////////////////////////////////////////////////////
//
// JobConfigWidget helper : populate gui initially

struct TxmParameterslUiFactoryFOB
{

    txmparameters* mTxmParametersWidget;
    unsigned int mCounter;
    int mStartRowInGrid;

    explicit TxmParameterslUiFactoryFOB(txmparameters* pclTxmParametersWidget, int startRowInGrid) :
        mTxmParametersWidget (pclTxmParametersWidget),
        mCounter             (0),
        mStartRowInGrid      (startRowInGrid)
        {}

    TxmParametersUi* operator()()
    {
        TxmParametersUi* cui = new TxmParametersUi (mTxmParametersWidget, mCounter++);
        (*cui)
            .setupGui()
            .setupSignals();

        return cui;
    }
};


// ============================================
// class txmparameters
txmparameters::txmparameters(QWidget *parent) :
    QWidget (parent),
    ui      (new Ui::txmparameters),
    mData   (NULL)
{
    // setup GUI
    ui->setupUi(this);

    // connect signals and slots
    pclTXMDipPolMapper = new QSignalMapper (this);

    connect(ui->sspinTXMPolDip1, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip1, 1);

    connect(ui->sspinTXMPolDip2, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip2, 2);

    connect(ui->sspinTXMPolDip3, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip3, 3);

    connect(ui->sspinTXMPolDip4, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip4, 4);

    connect(ui->sspinTXMPolDip5, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip5, 5);

    connect(ui->sspinTXMPolDip6, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip6, 6);

    connect(ui->sspinTXMPolDip7, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip7, 7);

    connect(ui->sspinTXMPolDip8, SIGNAL(valueChanged(double)), pclTXMDipPolMapper, SLOT(map()));
    pclTXMDipPolMapper->setMapping(ui->sspinTXMPolDip8, 8);

    connect (pclTXMDipPolMapper, SIGNAL(mapped(int)), this, SLOT(on_DipolePolChanged(int)));

    // setup jobtable view
    this->setupViews();

    // update tab order
    this->setTabOrder(ui->cbTXMNumDipoles, ui->sspinTXMPolDip1);
    this->setTabOrder(ui->sspinTXMPolDip1, ui->sspinTXMPolDip2);
    this->setTabOrder(ui->sspinTXMPolDip2, ui->sspinTXMPolDip3);
    this->setTabOrder(ui->sspinTXMPolDip3, ui->sspinTXMPolDip4);
    this->setTabOrder(ui->sspinTXMPolDip4, ui->sspinTXMPolDip5);
    this->setTabOrder(ui->sspinTXMPolDip5, ui->sspinTXMPolDip6);
    this->setTabOrder(ui->sspinTXMPolDip6, ui->sspinTXMPolDip7);
    this->setTabOrder(ui->sspinTXMPolDip7, ui->sspinTXMPolDip8);

    // by default, as long as no job is selected, disable input elements
    ui->gbWaveform->setEnabled(false);
    ui->gbSequence->setEnabled(false);

    addToolTips ();
}


txmparameters::~txmparameters()
{
    delete ui;
}


void txmparameters::setupViews()
{
#if QT_VERSION >= 0x050000
    ui->jobListTableView->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);
#else
    ui->jobListTableView->horizontalHeader()->setResizeMode (QHeaderView::Stretch);
#endif
    ui->jobListTableView->setSelectionBehavior              (QAbstractItemView::SelectRows);
    ui->jobListTableView->setSelectionMode                  (QTableView::SingleSelection);
}


void txmparameters::handleJobDataLoaded()
{
    if (mData != NULL)
    {
        try
        {
            ui->jobListTableView->setModel          (mData->getJobListModel   ());
            ui->jobListTableView->setSelectionModel (mData->getSelectionModel ());
            ui->jobListTableView->setColumnHidden   (2, true);
            ui->jobListTableView->setColumnHidden   (3, true);
            ui->jobListTableView->setColumnHidden   (4, true);
            ui->jobListTableView->setColumnHidden   (5, true);
            ui->jobListTableView->resizeRowsToContents   ();
            ui->jobListTableView->setAlternatingRowColors(true);

            QObject::connect(mData -> getSelectionModel(),
                             SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                             this ,
                             SLOT(jobSelectionChanged(const QItemSelection&, const QItemSelection&)),
                             Qt::UniqueConnection);

            QObject::connect(mData, SIGNAL(jobDataChanged()), this, SLOT(handleJobDataChanged()), Qt::UniqueConnection);
        }
        catch (const std::runtime_error& e)
        {
            qDebug() << __PRETTY_FUNCTION__  << e.what();
            QErrorMessage em;
            em.showMessage( QString( e.what() ) );
            em.exec();
        }
    }
}


void txmparameters::handleJobDataChanged()
{
    // update input elements with data of selected job
    updateChannelStatus();
}


void txmparameters::jobSelectionChanged( const QItemSelection &, const QItemSelection&)
{
    // enable input elements as now a job was selected
    ui->gbWaveform->setEnabled(true);
    ui->gbSequence->setEnabled(true);

    // update input elements with data of selected job
    updateChannelStatus();
}


void txmparameters::updateChannelStatus()
{
    // declaration of variables
    int          iIndex;
    unsigned int uiCounter;
    unsigned int uiTmp;

    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Waveform Name:" << pclCurrentJob->getTXMWaveformName();

        iIndex = ui->cbTXMWaveformName->findText(pclCurrentJob->getTXMWaveformName());
        if (iIndex >= 0)
        {
            ui->cbTXMWaveformName->setCurrentIndex(iIndex);
        }
        else
        {
            ui->cbTXMWaveformName->setCurrentIndex(0);
        }

        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Waveform Amplitude:" << pclCurrentJob->getTXMMaxAmpl();
        ui->sspinTXMMaxAmpl->setValue(pclCurrentJob->getTXMMaxAmpl().toDouble());

        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Waveform Base Freq:" << pclCurrentJob->getTXMBaseFreq();
        uiTmp = (unsigned int ) pclCurrentJob->getTXMBaseFreq().toDouble();
        iIndex = ui->cbTXMBaseFreq->findText(QString::number (uiTmp));
        if (iIndex >= 0)
        {
            ui->cbTXMBaseFreq->setCurrentIndex(iIndex);
        }
        else
        {
            ui->cbTXMWaveformName->setCurrentIndex(0);
        }

        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Sequence Number Of Stacks:" << pclCurrentJob->getTXMNumStacks();
        ui->pbTXMNumStacks->setText(pclCurrentJob->getTXMNumStacks());

        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Sequence Dipole TX Time:" << pclCurrentJob->getTXMDipTXTime();
        ui->pbTXMDipTXTime->setText(pclCurrentJob->getTXMDipTXTime());

        qDebug() << "[" << __PRETTY_FUNCTION__ << "] Sequence Number Of Dipoles:" << pclCurrentJob->getTXMNumDipoles();
        iIndex = ui->cbTXMNumDipoles->findText(pclCurrentJob->getTXMNumDipoles());
        if (iIndex >= 0)
        {
            ui->cbTXMNumDipoles->setCurrentIndex(iIndex);
        }
        else
        {
            ui->cbTXMWaveformName->setCurrentIndex(0);
        }

        QVector<QDoubleSpinBox*> vecDipPolSpinbox;
        vecDipPolSpinbox.clear();
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip1);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip2);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip3);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip4);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip5);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip6);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip7);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip8);

        QVector<double> vecDipPolValues = pclCurrentJob->getTXMPolarisations();
        for (uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
        {
            vecDipPolSpinbox [uiCounter]->setMaximum(180.0);
            vecDipPolSpinbox [uiCounter]->setMinimum(-180.0);
            if (uiCounter < pclCurrentJob->getTXMNumDipoles().toUInt())
            {
                vecDipPolSpinbox [uiCounter]->setDisabled(false);
            }
            else
            {
                vecDipPolSpinbox [uiCounter]->setDisabled(true);
            }

            vecDipPolSpinbox [uiCounter]->setValue(vecDipPolValues [uiCounter]);
        }
    }
}


void txmparameters::on_cbTXMWaveformName_currentIndexChanged(int index __attribute__((unused)))
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        pclCurrentJob->setTXMWaveformName(ui->cbTXMWaveformName->currentText());
    }
}


void txmparameters::on_sspinTXMMaxAmpl_valueChanged(double dValue)
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    qDebug () << "[" << __PRETTY_FUNCTION__ << "] new value:" << dValue;

    if (pclCurrentJob != NULL)
    {

        pclCurrentJob->setTXMMaxAmpl(QString::number (dValue));
    }
}


void txmparameters::on_cbTXMBaseFreq_currentIndexChanged(int index __attribute__((unused)))
{
    // declaration of variables
    QString      qstrTmp;
    unsigned int uiTmp;
    double       dTmp;

    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        pclCurrentJob->setTXMBaseFreq(ui->cbTXMBaseFreq->currentText());

        updateTXMIterations ();
    }

    // create warning, if in CSMT mode, if sample frequency is smaller
    // 4 * base frequency
    if (this->mData->getJoblistMode() == C_JOBLIST_MODE_CSMT)
    {
        uiTmp  = pclCurrentJob->getFrequency(qstrTmp, dTmp);
        uiTmp /= 4;
        if (ui->cbTXMBaseFreq->currentText().toUInt() > uiTmp)
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_WARNING_TX_FREQ_TOO_BIG), QMessageBox::Ok);
        }
    }
}


void txmparameters::on_pbTXMDipTXTime_clicked ()
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        // declaration of variables
        bool    bOk;
        QString qstrTmp;

        KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Dipole TX Time [s]", this);
        if (pclKeyPad->exec() == true)
        {
            (void) pclKeyPad->text().toInt(&bOk);
            if (bOk == true)
            {
                this->mData->setTXMDipTXTime  (QString::number(pclKeyPad->text().toInt()));
                ui->pbTXMDipTXTime->setText   (QString::number(pclKeyPad->text().toInt()));
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

        updateTXMIterations ();
    }
}


void txmparameters::on_pbTXMNumStacks_clicked ()
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        // declaration of variables
        bool    bOk;
        QString qstrTmp;

        KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Number Of Stacks", this);
        if (pclKeyPad->exec() == true)
        {
            (void) pclKeyPad->text().toInt(&bOk);
            if (bOk == true)
            {
                this->mData->setTXMNumStacks(QString::number(pclKeyPad->text().toInt()));
                ui->pbTXMNumStacks->setText (QString::number(pclKeyPad->text().toInt()));
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

        updateTXMIterations ();
    }
}


void txmparameters::updateTXMIterations (void)
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();
    double dTXTime;
    double dNumStacks;
    double dBaseFreq;
    double dNumDipoles;
    double dIterations;

    if (pclCurrentJob != NULL)
    {
        dTXTime     = pclCurrentJob->getTXMDipTXTime().toDouble  ();
        dNumStacks  = pclCurrentJob->getTXMNumStacks().toDouble  ();
        dBaseFreq   = pclCurrentJob->getTXMBaseFreq().toDouble   ();
        dNumDipoles = pclCurrentJob->getTXMNumDipoles().toDouble ();

        dIterations = dTXTime * dNumStacks * dNumDipoles * dBaseFreq;
        pclCurrentJob->setTXMIterations(QString::number ((unsigned int) dIterations));

        if (this->mData->getMeasurementMode() == ControlData::CSAMTMeasurementMode)
        {
            Types::TimeInterval clDuration;
            clDuration.setSeconds((unsigned int) dTXTime * dNumStacks * dNumDipoles);
            pclCurrentJob->setDuration(clDuration);
        }
    }
}


void txmparameters::on_cbTXMNumDipoles_currentIndexChanged(int index __attribute__((unused)))
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();
    unsigned int uiCounter;

    if (pclCurrentJob != NULL)
    {
        this->mData->setTXMNumDipoles(ui->cbTXMNumDipoles->currentText());

        QVector<QDoubleSpinBox*> vecDipPolSpinbox;
        vecDipPolSpinbox.clear();
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip1);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip2);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip3);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip4);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip5);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip6);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip7);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip8);
        for (uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
        {
            if (uiCounter < pclCurrentJob->getTXMNumDipoles().toUInt())
            {
                vecDipPolSpinbox [uiCounter]->setDisabled(false);
            }
            else
            {
                vecDipPolSpinbox [uiCounter]->setDisabled(true);
            }
        }

        updateTXMIterations ();
    }
}


void txmparameters::on_DipolePolChanged (const int& iIndex)
{
    // get data for actually selected job
    QSharedPointer<Job> pclCurrentJob = this->mData->getSelectedJob();

    if (pclCurrentJob != NULL)
    {
        pclCurrentJob->setTXMNumDipoles(ui->cbTXMNumDipoles->currentText());

        QVector<QDoubleSpinBox*> vecDipPolSpinbox;
        vecDipPolSpinbox.clear();
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip1);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip2);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip3);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip4);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip5);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip6);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip7);
        vecDipPolSpinbox.push_back(ui->sspinTXMPolDip8);

        QVector<double> vecDiPolValues = pclCurrentJob->getTXMPolarisations();
        vecDiPolValues [iIndex -1] = vecDipPolSpinbox [iIndex - 1]->value();

        pclCurrentJob->setTXMPolarisations(vecDiPolValues);
    }
}


void txmparameters::on_pbTXMDipoleChangeAll_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    unsigned int uiCounter;
    int          iIndex;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_TXM_CHANGE_ALL_DIP_POL),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {

        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                pclCurrentJob->setTXMNumDipoles(ui->cbTXMNumDipoles->currentText());

                QVector<QDoubleSpinBox*> vecDipPolSpinbox;
                vecDipPolSpinbox.clear();
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip1);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip2);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip3);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip4);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip5);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip6);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip7);
                vecDipPolSpinbox.push_back(ui->sspinTXMPolDip8);

                QVector<double> vecDiPolValues;
                vecDiPolValues.clear ();

                for (uiCounter = 0; uiCounter < DefaultValues::TXMMaxNumDipoles; uiCounter++)
                {
                    if (uiCounter < pclCurrentJob->getTXMNumDipoles().toUInt())
                    {
                        vecDipPolSpinbox [uiCounter]->setDisabled(false);
                    }
                    else
                    {
                        vecDipPolSpinbox [uiCounter]->setDisabled(true);
                    }
                    vecDiPolValues.push_back(vecDipPolSpinbox[uiCounter]->value());
                }

                pclCurrentJob->setTXMPolarisations(vecDiPolValues);

                updateTXMIterations ();
            }
        }
    }
}


void txmparameters::on_pbTXMWaveformChangeAll_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    int          iIndex;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_TXM_CHANGE_ALL_WAVEFORM),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {
        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                pclCurrentJob->setTXMWaveformName(ui->cbTXMWaveformName->currentText());
            }
        }
    }
}


void txmparameters::on_pbTXMAmplChangeAll_clicked ()
{
    // declaration of variables
    unsigned int uiNumJobs;
    unsigned int uiJobCounter;
    int          iIndex;
    QSharedPointer<Job> pclCurrentJob;

    iIndex = QMessageBox::question(this, tr (C_STRING_GUI_TXM_CHANGE_ALL_MAX_AMPL),
                                         tr (C_STRING_GUI_TXM_CHANGE_ALL),
                                         QMessageBox::Yes, QMessageBox::No);
    if (iIndex == QMessageBox::Yes)
    {
        uiNumJobs = this->mData->jobCount();
        for (uiJobCounter = 0; uiJobCounter < uiNumJobs; uiJobCounter++)
        {
            pclCurrentJob = this->mData->getJobAt(uiJobCounter);
            if (pclCurrentJob != NULL)
            {
                pclCurrentJob->setTXMMaxAmpl(QString::number (ui->sspinTXMMaxAmpl->value ()));
            }
        }
    }
}


void txmparameters::on_atBeginButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }
    ui->jobListTableView->selectRow(0);
}


void txmparameters::on_atEndButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }

    int newRow = ui->jobListTableView->model()->rowCount() - 1;
    ui->jobListTableView->selectRow(newRow);
}


void txmparameters::on_oneBackButton_clicked ()
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


void txmparameters::on_oneForewardButton_clicked ()
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


void txmparameters::addToolTips (void)
{
    ui->gbWaveform->setToolTip  (tr (C_STRING_TT_TXM_PARAM_WAVEFORM));
    ui->gbSequence->setToolTip  (tr (C_STRING_TT_TXM_PARAM_SEQUENCE));
}
