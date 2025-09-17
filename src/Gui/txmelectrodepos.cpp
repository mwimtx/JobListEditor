#include "txmelectrodepos.h"
#include "ui_txmelectrodepos.h"
#include "tools_types.h"
#include "codemapper.h"
#include "keypad.h"
#include "statekeypadwindow.h"
#include "Strings.h"
#include "geocoordinates.h"
#include "freekeypadwindow.h"

#include <QMessageBox>

// ====================================================
// class TxmElectrodePosUi
TxmElectrodePosUi::TxmElectrodePosUi(TxmElectrodePos* parent , const Types::Index& channel __attribute__((unused))) :
    QWidget                (parent),
    mTxmElectrodePosWidget (parent)
{
}


TxmElectrodePosUi& TxmElectrodePosUi::setupGui()
{
    return *this;
}


void TxmElectrodePosUi::setupSignals()
{
}


// ====================================================
// clas  TxmParameters helper : update gui stuff
struct UpdateTxmElectrodePosStatusFOB
{
    explicit UpdateTxmElectrodePosStatusFOB (DataModel* model, Tools::CodeMapper* codeMapper) :
        mModel      (model) ,
        mCodeMapper (codeMapper)
        {}

    DataModel* mModel;
    Tools::CodeMapper* mCodeMapper;

    void operator() (TxmElectrodePosUi* channelUi __attribute__((unused)))
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

    void updateGuiElements(TxmElectrodePosUi* channelUi __attribute__((unused)),  JobChannelConfig* channelCfg __attribute__((unused)))
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

struct TxmElectrodePosUiFactoryFOB
{

    TxmElectrodePos* mTxmElectrodePosWidget;
    unsigned int mCounter;
    int mStartRowInGrid;

    explicit TxmElectrodePosUiFactoryFOB(TxmElectrodePos* pclTxmElectrodePosWidget, int startRowInGrid) :
        mTxmElectrodePosWidget (pclTxmElectrodePosWidget),
        mCounter             (0),
        mStartRowInGrid      (startRowInGrid)
        {}

    TxmElectrodePosUi* operator()()
    {
        TxmElectrodePosUi* cui = new TxmElectrodePosUi (mTxmElectrodePosWidget, mCounter++);
        (*cui)
            .setupGui()
            .setupSignals();

        return cui;
    }
};


// ====================================================
// class TxmElectrodePos
TxmElectrodePos::TxmElectrodePos(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TxmElectrodePos)
{
    mData = NULL;

    ui->setupUi(this);

    // initialise signal mapper and button vector
    qvecLatButtons.clear();
    qvecLatButtons.push_back(ui->pbLatCenter);
    qvecLatButtons.push_back(ui->pbLatE1);
    qvecLatButtons.push_back(ui->pbLatE2);
    qvecLatButtons.push_back(ui->pbLatE3);

    qvecLonButtons.clear();
    qvecLonButtons.push_back(ui->pbLonCenter);
    qvecLonButtons.push_back(ui->pbLonE1);
    qvecLonButtons.push_back(ui->pbLonE2);
    qvecLonButtons.push_back(ui->pbLonE3);

    qvecElevButtons.clear();
    qvecElevButtons.push_back(ui->pbElevationCenter);
    qvecElevButtons.push_back(ui->pbElevationE1);
    qvecElevButtons.push_back(ui->pbElevationE2);
    qvecElevButtons.push_back(ui->pbElevationE3);

    connect(qvecLatButtons [C_TXM_ELECTRODE_CENTER], SIGNAL (clicked ()), &(this->qsmapLatButtons), SLOT(map ()));
    this->qsmapLatButtons.setMapping(qvecLatButtons [C_TXM_ELECTRODE_CENTER], C_TXM_ELECTRODE_CENTER);

    connect(qvecLatButtons [C_TXM_ELECTRODE_E1], SIGNAL (clicked ()), &(this->qsmapLatButtons), SLOT(map ()));
    this->qsmapLatButtons.setMapping(qvecLatButtons [C_TXM_ELECTRODE_E1], C_TXM_ELECTRODE_E1);

    connect(qvecLatButtons [C_TXM_ELECTRODE_E2], SIGNAL (clicked ()), &(this->qsmapLatButtons), SLOT(map ()));
    this->qsmapLatButtons.setMapping(qvecLatButtons [C_TXM_ELECTRODE_E2], C_TXM_ELECTRODE_E2);

    connect(qvecLatButtons [C_TXM_ELECTRODE_E3], SIGNAL (clicked ()), &(this->qsmapLatButtons), SLOT(map ()));
    this->qsmapLatButtons.setMapping(qvecLatButtons [C_TXM_ELECTRODE_E3], C_TXM_ELECTRODE_E3);

    connect (&(this->qsmapLatButtons), SIGNAL(mapped(int)), this, SLOT(on_pbLat_clicked(int)));


    connect(qvecLonButtons [C_TXM_ELECTRODE_CENTER], SIGNAL (clicked ()), &(this->qsmapLonButtons), SLOT(map ()));
    this->qsmapLonButtons.setMapping(qvecLonButtons [C_TXM_ELECTRODE_CENTER], C_TXM_ELECTRODE_CENTER);

    connect(qvecLonButtons [C_TXM_ELECTRODE_E1], SIGNAL (clicked ()), &(this->qsmapLonButtons), SLOT(map ()));
    this->qsmapLonButtons.setMapping(qvecLonButtons [C_TXM_ELECTRODE_E1], C_TXM_ELECTRODE_E1);

    connect(qvecLonButtons [C_TXM_ELECTRODE_E2], SIGNAL (clicked ()), &(this->qsmapLonButtons), SLOT(map ()));
    this->qsmapLonButtons.setMapping(qvecLonButtons [C_TXM_ELECTRODE_E2], C_TXM_ELECTRODE_E2);

    connect(qvecLonButtons [C_TXM_ELECTRODE_E3], SIGNAL (clicked ()), &(this->qsmapLonButtons), SLOT(map ()));
    this->qsmapLonButtons.setMapping(qvecLonButtons [C_TXM_ELECTRODE_E3], C_TXM_ELECTRODE_E3);

    // checkqt5
    connect (&(this->qsmapLonButtons), SIGNAL(mapped(int)), this, SLOT(on_pbLon_clicked(int)));


    connect(qvecElevButtons [C_TXM_ELECTRODE_CENTER], SIGNAL (clicked ()), &(this->qsmapElevButtons), SLOT(map ()));
    this->qsmapElevButtons.setMapping(qvecElevButtons [C_TXM_ELECTRODE_CENTER], C_TXM_ELECTRODE_CENTER);

    connect(qvecElevButtons [C_TXM_ELECTRODE_E1], SIGNAL (clicked ()), &(this->qsmapElevButtons), SLOT(map ()));
    this->qsmapElevButtons.setMapping(qvecElevButtons [C_TXM_ELECTRODE_E1], C_TXM_ELECTRODE_E1);

    connect(qvecElevButtons [C_TXM_ELECTRODE_E2], SIGNAL (clicked ()), &(this->qsmapElevButtons), SLOT(map ()));
    this->qsmapElevButtons.setMapping(qvecElevButtons [C_TXM_ELECTRODE_E2], C_TXM_ELECTRODE_E2);

    connect(qvecElevButtons [C_TXM_ELECTRODE_E3], SIGNAL (clicked ()), &(this->qsmapElevButtons), SLOT(map ()));
    this->qsmapElevButtons.setMapping(qvecElevButtons [C_TXM_ELECTRODE_E3], C_TXM_ELECTRODE_E3);

    connect (&(this->qsmapElevButtons), SIGNAL(mapped(int)), this, SLOT(on_pbElevation_clicked(int)));
}


TxmElectrodePos::~TxmElectrodePos()
{
    delete ui;
}


void TxmElectrodePos::handleJobDataLoaded()
{
    // update input elements with data of selected job
    QObject::connect(mData->getSelectionModel(),
                     SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                     this ,
                     SLOT(jobSelectionChanged(const QItemSelection&, const QItemSelection&)),
                     Qt::UniqueConnection);

    QObject::connect(mData, SIGNAL(jobDataChanged()), this, SLOT(handleJobDataChanged()), Qt::UniqueConnection);
}


void TxmElectrodePos::handleJobDataChanged()
{
    // update input elements with data of selected job
    updateGuiElements();
}


void TxmElectrodePos::jobSelectionChanged( const QItemSelection &, const QItemSelection&)
{
    // update input elements with data of selected job
    updateGuiElements();
}


void TxmElectrodePos::updateGuiElements(void)
{
    // declaration of variables
    QString qstrLat;
    QString qstrLon;
    QString qstrElev;

    if (this->mData != NULL)
    {
        if (this->mData->jobCount() > 0)
        {
            this->mData->getTXMElectrodePosStr (C_TXM_ELECTRODE_CENTER, qstrLat, qstrLon, qstrElev);
            ui->pbLatCenter->setText      (qstrLat);
            ui->pbLonCenter->setText      (qstrLon);
            ui->pbElevationCenter->setText(qstrElev);

            this->mData->getTXMElectrodePosStr (C_TXM_ELECTRODE_E1, qstrLat, qstrLon, qstrElev);
            ui->pbLatE1->setText      (qstrLat);
            ui->pbLonE1->setText      (qstrLon);
            ui->pbElevationE1->setText(qstrElev);

            this->mData->getTXMElectrodePosStr (C_TXM_ELECTRODE_E2, qstrLat, qstrLon, qstrElev);
            ui->pbLatE2->setText      (qstrLat);
            ui->pbLonE2->setText      (qstrLon);
            ui->pbElevationE2->setText(qstrElev);

            this->mData->getTXMElectrodePosStr (C_TXM_ELECTRODE_E3, qstrLat, qstrLon, qstrElev);
            ui->pbLatE3->setText      (qstrLat);
            ui->pbLonE3->setText      (qstrLon);
            ui->pbElevationE3->setText(qstrElev);
        }
        else
        {
            ui->pbLatCenter->setText      ("N/A");
            ui->pbLonCenter->setText      ("N/A");
            ui->pbElevationCenter->setText("N/A");

            ui->pbLatE1->setText      ("N/A");
            ui->pbLonE1->setText      ("N/A");
            ui->pbElevationE1->setText("N/A");

            ui->pbLatE2->setText      ("N/A");
            ui->pbLonE2->setText      ("N/A");
            ui->pbElevationE2->setText("N/A");

            ui->pbLatE3->setText      ("N/A");
            ui->pbLonE3->setText      ("N/A");
            ui->pbElevationE3->setText("N/A");
        }
    }
    else
    {
        ui->pbLatCenter->setText      ("N/A");
        ui->pbLonCenter->setText      ("N/A");
        ui->pbElevationCenter->setText("N/A");

        ui->pbLatE1->setText      ("N/A");
        ui->pbLonE1->setText      ("N/A");
        ui->pbElevationE1->setText("N/A");

        ui->pbLatE2->setText      ("N/A");
        ui->pbLonE2->setText      ("N/A");
        ui->pbElevationE2->setText("N/A");

        ui->pbLatE3->setText      ("N/A");
        ui->pbLonE3->setText      ("N/A");
        ui->pbElevationE3->setText("N/A");
    }
}


void TxmElectrodePos::on_pbLon_clicked (int iIndex)
{
    // declaration of variables
    long    lLon;
    long    lLat;
    long    lElev;
    QString qstrTmp;

    if ((iIndex  < 0) && (iIndex >= ((int)DefaultValues::TXMNumElectrodePos)))
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: invalid electrode position index:" << iIndex;

        qstrTmp  = "[";
        qstrTmp += __PRETTY_FUNCTION__;
        qstrTmp += "]: invalid electrode position index:";
        qstrTmp += QString::number (iIndex);
        QMessageBox::critical(this, "Internal Error", qstrTmp, QMessageBox::Ok);
    }
    else
    {
        if (iIndex == 0)
        {
            qstrTmp = "Longitude - Center [GPS]";
        }
        else
        {
            qstrTmp = "Longitude - E" + QString::number (iIndex) + " [GPS]";
        }

        GpsKeypadWindow clKeyPad (tr (qstrTmp.toLatin1()), this);
        if (clKeyPad.exec() == true)
        {
            // read actual values
            this->mData->getTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // read new value from keypad
            lLon = clKeyPad.gpsValue();

            // write back updated values
            this->mData->setTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // now convert to Google Maps string and show on button
            geocoordinates* pclGeoCoordinates = new geocoordinates ();
            pclGeoCoordinates->set_lon_lat_msec(lLon, lLat, lElev);
            this->qvecLonButtons [iIndex]->setText(pclGeoCoordinates->get_lon_str_google_maps());
            delete (pclGeoCoordinates);
        }
    }
}


void TxmElectrodePos::on_pbLat_clicked (int iIndex)
{
    // declaration of variables
    long    lLon;
    long    lLat;
    long    lElev;
    QString qstrTmp;

    if ((iIndex  < 0) && (iIndex >= ((int)DefaultValues::TXMNumElectrodePos)))
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: invalid electrode position index:" << iIndex;

        qstrTmp  = "[";
        qstrTmp += __PRETTY_FUNCTION__;
        qstrTmp += "]: invalid electrode position index:";
        qstrTmp += QString::number (iIndex);
        QMessageBox::critical(this, "Internal Error", qstrTmp, QMessageBox::Ok);
    }
    else
    {
        if (iIndex == 0)
        {
            qstrTmp = "Latitude - Center [GPS]";
        }
        else
        {
            qstrTmp = "Latitude - E" + QString::number (iIndex) + " [GPS]";
        }

        GpsKeypadWindow clKeyPad (tr (qstrTmp.toLatin1()), this);
        if (clKeyPad.exec() == true)
        {
            // read actual values
            this->mData->getTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // read new value from keypad
            lLat = clKeyPad.gpsValue();

            // write back updated values
            this->mData->setTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // now convert to Google Maps string and show on button
            geocoordinates* pclGeoCoordinates = new geocoordinates ();
            pclGeoCoordinates->set_lon_lat_msec(lLon, lLat, lElev);
            this->qvecLatButtons [iIndex]->setText(pclGeoCoordinates->get_lat_str_google_maps());
            delete (pclGeoCoordinates);
        }
    }
}


void TxmElectrodePos::on_pbElevation_clicked (int iIndex)
{
    // declaration of variables
    long    lLon;
    long    lLat;
    long    lElev;
    QString qstrTmp;

    if ((iIndex  < 0) && (iIndex >= ((int)DefaultValues::TXMNumElectrodePos)))
    {
        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: invalid electrode position index:" << iIndex;

        qstrTmp  = "[";
        qstrTmp += __PRETTY_FUNCTION__;
        qstrTmp += "]: invalid electrode position index:";
        qstrTmp += QString::number (iIndex);
        QMessageBox::critical(this, "Internal Error", qstrTmp, QMessageBox::Ok);
    }
    else
    {
        if (iIndex == 0)
        {
            qstrTmp = "Elevation - Center [meter]";
        }
        else
        {
            qstrTmp = "Elevation - E" + QString::number (iIndex) + " [meter]";
        }

        KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (false, true, tr (qstrTmp.toLatin1()), this);
        if (pclKeyPad->exec() == true)
        {
            // read actual values
            this->mData->getTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // read new value from keypad
            lElev = (long) (pclKeyPad->text().toDouble() * 100);

            // write back updated values
            this->mData->setTXMElectrodePos(iIndex, lLat, lLon, lElev);

            // now convert to Google Maps string and show on button
            geocoordinates* pclGeoCoordinates = new geocoordinates ();
            pclGeoCoordinates->set_lon_lat_msec(lLon, lLat, lElev);
            this->qvecElevButtons [iIndex]->setText(pclGeoCoordinates->get_elevation_str(qstrTmp));
            delete (pclGeoCoordinates);
        }
    }
}
