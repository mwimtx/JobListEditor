#include "importautogainoffsetdialog.h"
#include "ui_importautogainoffsetdialog.h"
#include "Strings.h"
#include "domelementwalker.h"
#include "xml_parse_helper.h"
#include "xml_dom_helper.h"
#include "fastbuttonselectpad.h"
#include "codemapper.h"
#include "freekeypadwindow.h"

#include <QErrorMessage>
#include <QFile>
#include <QMessageBox>
#include <QDomDocument>
#include <QDebug>
#include <QGroupBox>
#include <QLabel>

ImportAutoGainOffsetDialog::ImportAutoGainOffsetDialog(DataModel* mDataRef, const QString& qstrPathToXMLRef, QWidget *parent) :
    mData           (mDataRef),
    qstrPathToXML   (qstrPathToXMLRef),
    QDialog         (parent),
    ui              (new Ui::ImportAutoGainOffsetDialog)
{
    // declaration of variables
    QString qstrTmp;

    // connect all GUI elements
    ui->setupUi(this);

    // set Heading
    // set window title
    qstrTmp  = BuildManifest::ApplicationName.c_str();
    qstrTmp += " - ";
    qstrTmp += BuildManifest::VersionString.c_str();
    qstrTmp += " - ";
    qstrTmp += "Import AutoGain / AutoOffset";
    this->setWindowTitle(qstrTmp);

    // show job data in list - do this manually here as at this point the
    // joblist is at any case loaded
    handleJobDataLoaded ();

    // manually connect slots
    // comments tab
    QObject::connect(mData , SIGNAL(jobDataLoaded  ()), this, SLOT (handleJobDataLoaded  ()), Qt::UniqueConnection);
    QObject::connect(mData , SIGNAL(jobDataChanged ()), this, SLOT (handleJobDataChanged ()), Qt::UniqueConnection);

    // initialise mapping for RF Filters
    this->clRFFilterMapper.addMapping ("LF_RF_1", "3000");
    this->clRFFilterMapper.addMapping ("LF_RF_2", "1000");
    this->clRFFilterMapper.addMapping ("LF_RF_3",  "500");
    this->clRFFilterMapper.addMapping ("LF_RF_4",  "200");

    // parse XML file
    this->parseADU07ChannelConfigXml(this->qstrPathToXML);

    // resize this window to fit parents size
    if (parent != NULL)
    {
        //this->setGeometry (parent->x(), parent->y(), parent->width(), parent->height());
        this->resize(parent->width() + 10, parent->height() + 110);
    }

    // hide scroll buttons and joblist table view as they are not needed actually
    ui->qfrScrollButtons->setVisible(false);
    ui->jobListTableView->setVisible(false);

}


ImportAutoGainOffsetDialog::~ImportAutoGainOffsetDialog()
{
    delete ui;
}


void ImportAutoGainOffsetDialog::on_atBeginButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }
    ui->jobListTableView->selectRow(0);
}


void ImportAutoGainOffsetDialog::on_atEndButton_clicked ()
{
    if (not (ui->jobListTableView->model()->rowCount() > 0))
    {
        return;
    }

    int newRow = ui->jobListTableView->model()->rowCount() - 1;
    ui->jobListTableView->selectRow(newRow);
}


void ImportAutoGainOffsetDialog::on_oneBackButton_clicked ()
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


void ImportAutoGainOffsetDialog::on_oneForewardButton_clicked ()
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


void ImportAutoGainOffsetDialog::handleJobDataLoaded()
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
            //ui->jobListTableView->setColumnHidden   (5, true);
            ui->jobListTableView->resizeRowsToContents   ();
            ui->jobListTableView->setAlternatingRowColors(true);

            QObject::connect(mData -> getSelectionModel(),
                             SIGNAL(selectionChanged (const QItemSelection&, const QItemSelection&)),
                             this ,
                             SLOT(jobSelectionChanged(const QItemSelection&, const QItemSelection&)),
                             Qt::UniqueConnection);

            QObject::connect(mData, SIGNAL(jobDataChanged()), this, SLOT(handleJobDataChanged()), Qt::UniqueConnection);

            ui->jobListTableView->resizeRowsToContents();
            ui->jobListTableView->setAlternatingRowColors(true);

            ui->jobListTableView->setColumnWidth(1, ui->jobListTableView->columnWidth(1) +20);
            ui->jobListTableView->setColumnWidth(2, ui->jobListTableView->columnWidth(2) +20);

            #if QT_VERSION >= 0x050000
                ui->jobListTableView->horizontalHeader ()->setSectionResizeMode (QHeaderView::Stretch);
            #else
                ui->jobListTableView->horizontalHeader ()->setResizeMode        (QHeaderView::Stretch);
            #endif

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


void ImportAutoGainOffsetDialog::handleJobDataChanged()
{
}


void ImportAutoGainOffsetDialog::jobSelectionChanged( const QItemSelection &, const QItemSelection&)
{
}


bool ImportAutoGainOffsetDialog::parseADU07ChannelConfigXml(const QString& qstrSrcFile)
{
    // declaration of variables
    bool             bRetValue = true;
    QString          qstrXmlFile;
    QString          qstrTmp1;
    QString          qstrTmp2;
    QFile            clXmlFile;
    QByteArray       clTmpArray;
    QDomDocument     clDomXml;
    QDomNamedNodeMap clAttrList;

    // try to open XML file
    if (QFile::exists(qstrSrcFile) == false)
    {
        qstrTmp1  = BuildManifest::ApplicationName.c_str();
        qstrTmp1 += " - ";
        qstrTmp1 += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp1, tr (C_STRING_GUI_INPORT_AUTOGAIN_OPEN_FAILED).arg(qstrSrcFile), QMessageBox::Ok);

        bRetValue = false;
    }
    else
    {
        clXmlFile.setFileName(qstrSrcFile);
        if (clXmlFile.open(QFile::ReadOnly) == false)
        {
            qstrTmp1  = BuildManifest::ApplicationName.c_str();
            qstrTmp1 += " - ";
            qstrTmp1 += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp1, tr (C_STRING_GUI_INPORT_AUTOGAIN_OPEN_FAILED).arg(qstrSrcFile), QMessageBox::Ok);

            bRetValue = false;
        }
        else
        {
            clTmpArray  = clXmlFile.readAll();
            qstrXmlFile = clTmpArray;

            // start to parse the XML file
            clDomXml.setContent(qstrXmlFile);

            Tools::DomElementWalker* pclWalker = new Tools::DomElementWalker (clDomXml);
            {
                QDomElement clTmpNode = pclWalker->get();

                // calibration date time
                Tools::parseOptionalChildInto (clTmpNode, "ci_date", qstrTmp1);
                Tools::parseOptionalChildInto (clTmpNode, "ci_time", qstrTmp2);
                qstrTmp1 = qstrTmp1 + " " + qstrTmp2;
                this->sADU07ChannelConfig.clCalDateTime.fromString(qstrTmp1, DefaultValues::DateTimeFormat);

                // longitude
                Tools::parseOptionalChildInto (clTmpNode, "Longitude", qstrTmp1);
                this->sADU07ChannelConfig.uiLongitude = qstrTmp1.toUInt();

                // latitude
                Tools::parseOptionalChildInto (clTmpNode, "Latitude", qstrTmp1);
                this->sADU07ChannelConfig.uiLatitude = qstrTmp1.toUInt();


                pclWalker->goToFirstChild("AdbBoards");
                pclWalker->goToFirstChild("channel");
                Tools::DomElementSiblingIterator iterDomNodes (*pclWalker);
                while (iterDomNodes.isValid())
                {
                    sADU07ChannelConfigEntry_t sEntry;

                    // read attributes if channel node
                    clAttrList = iterDomNodes.get().attributes();
                    sEntry.uiChannelNumber = clAttrList.namedItem("id").nodeValue  ().toUInt();
                    sEntry.qstrBoardType   = clAttrList.namedItem("type").nodeValue();

                    // parse Settings subnodes
                    Tools::DomElementWalker* pclSubWalker = new Tools::DomElementWalker (iterDomNodes.get());
                    pclSubWalker->goToFirstChild("Settings");
                    Tools::DomElementSiblingIterator iterDomSubNodes (*pclSubWalker);
                    while (iterDomSubNodes.isValid())
                    {

                        // read attributes if Settings node
                        clAttrList = iterDomSubNodes.get().attributes();
                        sEntry.qstrFilterType = clAttrList.namedItem("filter_type").nodeValue ();
                        sEntry.uiInput        = clAttrList.namedItem("input").nodeValue().toUInt();
                        sEntry.qstrMode       = clAttrList.namedItem("input_circuitry").nodeValue();

                        // radio filter type
                        Tools::parseOptionalChildInto (iterDomSubNodes.get(), "filter_type", sEntry.qstrRFFilter);

                        // gains
                        Tools::parseOptionalChildInto (iterDomSubNodes.get(), "gain_stage1", qstrTmp1);
                        sEntry.uiGainStage1 = qstrTmp1.toUInt();
                        Tools::parseOptionalChildInto (iterDomSubNodes.get(), "gain_stage2", qstrTmp1);
                        sEntry.uiGainStage2 = qstrTmp1.toUInt();

                        // DC offset correction
                        Tools::parseOptionalChildInto (iterDomSubNodes.get(), "dac_val", qstrTmp1);
                        sEntry.iDCOffsetCorr = qstrTmp1.toInt();

                        this->sADU07ChannelConfig.qvecChannelValues.push_back(sEntry);

                        iterDomSubNodes.next();
                    }
                    delete (pclSubWalker);

                    iterDomNodes.next();
                }

                delete (pclWalker);
            }
        }
    }

    if (bRetValue == true)
    {
        this->setupGUI(this->sADU07ChannelConfig);
    }

    return (bRetValue);
}


bool ImportAutoGainOffsetDialog::setupGUI(sADU07ChannelConfig_t& sChConfig)
{
    // declaration of variables
    bool            bRetValue = true;
    unsigned int    uiCounter;
    unsigned int    uiChannelCounter;
    unsigned int    uiEntryCounter;
    QString         qstrTmp;

    // clear vector of entries to be shown
    this->qstrlEntriesToBeShown.clear();
    this->qmapEntriesToBeShown.clear ();
    this->qvecEntriesToBeShown.clear ();

    for (uiCounter = 0; (int)uiCounter < sChConfig.qvecChannelValues.size(); uiCounter++)
    {
        // as first time, only add values for input 0
        sChConfig.qvecChannelValues [uiCounter].uiSignalMapperIndex = uiCounter;

        sChConfig.qvecChannelValues [uiCounter].pbGainStage1   = new QPushButton ();
        sChConfig.qvecChannelValues [uiCounter].pbGainStage2   = new QPushButton ();
        sChConfig.qvecChannelValues [uiCounter].pbDCOffsetCorr = new QPushButton ();
        sChConfig.qvecChannelValues [uiCounter].pbRFFilter     = new QPushButton ();

        sChConfig.qvecChannelValues [uiCounter].pbGainStage1->setText  (QString::number (sChConfig.qvecChannelValues [uiCounter].uiGainStage1));
        sChConfig.qvecChannelValues [uiCounter].pbGainStage2->setText  (QString::number (sChConfig.qvecChannelValues [uiCounter].uiGainStage2));
        sChConfig.qvecChannelValues [uiCounter].pbDCOffsetCorr->setText(QString::number (sChConfig.qvecChannelValues [uiCounter].iDCOffsetCorr));
        sChConfig.qvecChannelValues [uiCounter].pbRFFilter->setText    (this->clRFFilterMapper.toVisual(sChConfig.qvecChannelValues [uiCounter].qstrRFFilter));

        // connect buttons to signal mapper
        connect(sChConfig.qvecChannelValues [uiCounter].pbGainStage1, SIGNAL (clicked ()), &(this->qsmapGainStage1), SLOT(map ()));
        this->qsmapGainStage1.setMapping(sChConfig.qvecChannelValues [uiCounter].pbGainStage1, sChConfig.qvecChannelValues [uiCounter].uiSignalMapperIndex);

        connect(sChConfig.qvecChannelValues [uiCounter].pbGainStage2, SIGNAL (clicked ()), &(this->qsmapGainStage2), SLOT(map ()));
        this->qsmapGainStage2.setMapping(sChConfig.qvecChannelValues [uiCounter].pbGainStage2, sChConfig.qvecChannelValues [uiCounter].uiSignalMapperIndex);

        connect(sChConfig.qvecChannelValues [uiCounter].pbDCOffsetCorr, SIGNAL (clicked ()), &(this->qsmapDCOffsetCorr), SLOT(map ()));
        this->qsmapDCOffsetCorr.setMapping(sChConfig.qvecChannelValues [uiCounter].pbDCOffsetCorr, sChConfig.qvecChannelValues [uiCounter].uiSignalMapperIndex);

        connect(sChConfig.qvecChannelValues [uiCounter].pbRFFilter, SIGNAL (clicked ()), &(this->qsmapRFFilter), SLOT(map ()));
        this->qsmapRFFilter.setMapping(sChConfig.qvecChannelValues [uiCounter].pbRFFilter, sChConfig.qvecChannelValues [uiCounter].uiSignalMapperIndex);

        // build up GUI
        sChConfig.qvecChannelValues [uiCounter].pbGroupBox = new QGroupBox ();
        QGridLayout* pclLayout = new QGridLayout ();
        sChConfig.qvecChannelValues [uiCounter].pbGroupBox->setLayout(pclLayout);

        qstrTmp  = sChConfig.qvecChannelValues [uiCounter].qstrBoardType;
        qstrTmp += " - ";
        if (sChConfig.qvecChannelValues [uiCounter].uiInput == 0)
        {
            qstrTmp += "Std. Inputs (Ex .. Hz) - ";
        }
        else
        {
            qstrTmp += "Multi Purpose (IN2) - ";
        }
        qstrTmp += "Filter: \"";
        qstrTmp += sChConfig.qvecChannelValues [uiCounter].qstrFilterType;
        qstrTmp += "\"";
        sChConfig.qvecChannelValues [uiCounter].pbGroupBox->setTitle(qstrTmp);

        // add buttons
        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("GainStage1");
            pclLayout->addWidget(pclLab,                                               0, 0);
            pclLayout->addWidget(sChConfig.qvecChannelValues [uiCounter].pbGainStage1, 0, 1);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("GainStage2");
            pclLayout->addWidget(pclLab,                                               0, 3);
            pclLayout->addWidget(sChConfig.qvecChannelValues [uiCounter].pbGainStage2, 0, 4);
        }

        pclLayout->setColumnMinimumWidth(2, 50);

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("DC Offset Corr.");
            pclLayout->addWidget(pclLab,                                                 1, 0);
            pclLayout->addWidget(sChConfig.qvecChannelValues [uiCounter].pbDCOffsetCorr, 1, 1);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("RF Filter");
            pclLayout->addWidget(pclLab,                                             1, 3);
            pclLayout->addWidget(sChConfig.qvecChannelValues [uiCounter].pbRFFilter, 1, 4);
        }
    }

    // now place the single group boxes on the scroll area

    // show only entries for the LF  and HF boards on input 0
    for (uiCounter = 0; (int)uiCounter < sChConfig.qvecChannelValues.size(); uiCounter++)
    {
        if ((sChConfig.qvecChannelValues [uiCounter].qstrBoardType.contains ("ADU-07-LF") == true)  &&
            (sChConfig.qvecChannelValues [uiCounter].qstrFilterType.contains("ADU07")     == false) &&
            (sChConfig.qvecChannelValues [uiCounter].uiInput                              == 0))
        {
            if (qmapEntriesToBeShown.find(0) == qmapEntriesToBeShown.end())
            {
                qstrlEntriesToBeShown.append(sChConfig.qvecChannelValues [uiCounter].pbGroupBox->title());

                sAutoGainOffsetConfig_t sEntryGroup;
                sEntryGroup.bApplied = false;
                sEntryGroup.bSkipped = false;
                sEntryGroup.pbApply  = new QPushButton ();
                sEntryGroup.pbSkip   = new QPushButton ();
                sEntryGroup.pbGoBack = new QPushButton ();
                sEntryGroup.pbApply->setText (" Apply");
                sEntryGroup.pbSkip->setText  (" Skip");
                sEntryGroup.pbGoBack->setText(" Back");

                connect(sEntryGroup.pbApply, SIGNAL (clicked ()), &(this->qsmapApply), SLOT(map ()));
                this->qsmapApply.setMapping(sEntryGroup.pbApply, 0);

                connect(sEntryGroup.pbSkip, SIGNAL (clicked ()), &(this->qsmapSkip), SLOT(map ()));
                this->qsmapSkip.setMapping(sEntryGroup.pbSkip, 0);

                connect(sEntryGroup.pbGoBack, SIGNAL (clicked ()), &(this->qsmapGoBack), SLOT(map ()));
                this->qsmapGoBack.setMapping(sEntryGroup.pbGoBack, 0);

                qvecEntriesToBeShown.append (sEntryGroup);
            }
            if (qvecEntriesToBeShown.size())

            qmapEntriesToBeShown.insert(0, uiCounter);
        }

        if ((sChConfig.qvecChannelValues [uiCounter].qstrBoardType.contains ("ADU-07-LF") == true) &&
            (sChConfig.qvecChannelValues [uiCounter].qstrFilterType.contains("ADU07")     == true) &&
            (sChConfig.qvecChannelValues [uiCounter].uiInput                              == 0))
        {
            if (qmapEntriesToBeShown.find(1) == qmapEntriesToBeShown.end())
            {
                qstrlEntriesToBeShown.append(sChConfig.qvecChannelValues [uiCounter].pbGroupBox->title());

                sAutoGainOffsetConfig_t sEntryGroup;
                sEntryGroup.bApplied = false;
                sEntryGroup.bSkipped = false;
                sEntryGroup.pbApply  = new QPushButton ();
                sEntryGroup.pbSkip   = new QPushButton ();
                sEntryGroup.pbGoBack = new QPushButton ();
                sEntryGroup.pbApply->setText (" Apply");
                sEntryGroup.pbSkip->setText  (" Skip");
                sEntryGroup.pbGoBack->setText(" Back");

                connect(sEntryGroup.pbApply, SIGNAL (clicked ()), &(this->qsmapApply), SLOT(map ()));
                this->qsmapApply.setMapping(sEntryGroup.pbApply, 1);

                connect(sEntryGroup.pbSkip, SIGNAL (clicked ()), &(this->qsmapSkip), SLOT(map ()));
                this->qsmapSkip.setMapping(sEntryGroup.pbSkip, 1);

                connect(sEntryGroup.pbGoBack, SIGNAL (clicked ()), &(this->qsmapGoBack), SLOT(map ()));
                this->qsmapGoBack.setMapping(sEntryGroup.pbGoBack, 1);

                qvecEntriesToBeShown.append (sEntryGroup);
            }
            if (qvecEntriesToBeShown.size())

            qmapEntriesToBeShown.insert(1, uiCounter);
        }

        if ((sChConfig.qvecChannelValues [uiCounter].qstrBoardType.contains ("ADU-07-HF") == true)  &&
            (sChConfig.qvecChannelValues [uiCounter].qstrFilterType.contains("ADU07")     == false) &&
            (sChConfig.qvecChannelValues [uiCounter].uiInput                              == 0))
        {
            if (qmapEntriesToBeShown.find(2) == qmapEntriesToBeShown.end())
            {
                qstrlEntriesToBeShown.append(sChConfig.qvecChannelValues [uiCounter].pbGroupBox->title());

                sAutoGainOffsetConfig_t sEntryGroup;
                sEntryGroup.bApplied = false;
                sEntryGroup.bSkipped = false;
                sEntryGroup.pbApply  = new QPushButton ();
                sEntryGroup.pbSkip   = new QPushButton ();
                sEntryGroup.pbGoBack = new QPushButton ();
                sEntryGroup.pbApply->setText (" Apply");
                sEntryGroup.pbSkip->setText  (" Skip");
                sEntryGroup.pbGoBack->setText(" Back");

                connect(sEntryGroup.pbApply, SIGNAL (clicked ()), &(this->qsmapApply), SLOT(map ()));
                this->qsmapApply.setMapping(sEntryGroup.pbApply, 2);

                connect(sEntryGroup.pbSkip, SIGNAL (clicked ()), &(this->qsmapSkip), SLOT(map ()));
                this->qsmapSkip.setMapping(sEntryGroup.pbSkip, 2);

                connect(sEntryGroup.pbGoBack, SIGNAL (clicked ()), &(this->qsmapGoBack), SLOT(map ()));
                this->qsmapGoBack.setMapping(sEntryGroup.pbGoBack, 2);

                qvecEntriesToBeShown.append (sEntryGroup);
            }
            if (qvecEntriesToBeShown.size())

            qmapEntriesToBeShown.insert(2, uiCounter);
        }

        if ((sChConfig.qvecChannelValues [uiCounter].qstrBoardType.contains ("ADU-07-HF") == true) &&
            (sChConfig.qvecChannelValues [uiCounter].qstrFilterType.contains("ADU07")     == true) &&
            (sChConfig.qvecChannelValues [uiCounter].uiInput                              == 0))
        {
            if (qmapEntriesToBeShown.find(3) == qmapEntriesToBeShown.end())
            {
                qstrlEntriesToBeShown.append(sChConfig.qvecChannelValues [uiCounter].pbGroupBox->title());

                sAutoGainOffsetConfig_t sEntryGroup;
                sEntryGroup.bApplied = false;
                sEntryGroup.bSkipped = false;
                sEntryGroup.pbApply  = new QPushButton ();
                sEntryGroup.pbSkip   = new QPushButton ();
                sEntryGroup.pbGoBack = new QPushButton ();
                sEntryGroup.pbApply->setText (" Apply");
                sEntryGroup.pbSkip->setText  (" Skip");
                sEntryGroup.pbGoBack->setText(" Back");

                connect(sEntryGroup.pbApply, SIGNAL (clicked ()), &(this->qsmapApply), SLOT(map ()));
                this->qsmapApply.setMapping(sEntryGroup.pbApply, 3);

                connect(sEntryGroup.pbSkip, SIGNAL (clicked ()), &(this->qsmapSkip), SLOT(map ()));
                this->qsmapSkip.setMapping(sEntryGroup.pbSkip, 3);

                connect(sEntryGroup.pbGoBack, SIGNAL (clicked ()), &(this->qsmapGoBack), SLOT(map ()));
                this->qsmapGoBack.setMapping(sEntryGroup.pbGoBack, 3);

                qvecEntriesToBeShown.append (sEntryGroup);
            }
            if (qvecEntriesToBeShown.size())

            qmapEntriesToBeShown.insert(3, uiCounter);
        }
    }

    // now add the items that shall be displayed to the GUI
    for (uiCounter = 0; (int)uiCounter < qstrlEntriesToBeShown.size(); uiCounter++)
    {
        qvecEntriesToBeShown [uiCounter].pclGroupBox = new QGroupBox ();
        qvecEntriesToBeShown [uiCounter].pclGroupBox->setTitle(qstrlEntriesToBeShown [uiCounter]);

        QVBoxLayout* pclLayout1 = new QVBoxLayout ();
        QGridLayout* pclLayout  = new QGridLayout ();
        pclLayout1->addLayout(pclLayout);
        qvecEntriesToBeShown [uiCounter].pclGroupBox->setLayout(pclLayout1);

        // add heading
        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("Channel");
            pclLayout->addWidget(pclLab, 0, 0);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("Radio Filter");
            pclLayout->addWidget(pclLab, 0, 1);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("GainStage1");
            pclLayout->addWidget(pclLab, 0, 2);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("DC Offset Corr.");
            pclLayout->addWidget(pclLab, 0, 3);
        }

        {
            QLabel* pclLab = new QLabel ();
            pclLab->setText("GainStage2");
            pclLayout->addWidget(pclLab, 0, 4);
        }

        // now add buttons for all channels
        QList<unsigned int> clEntries = qmapEntriesToBeShown.values(uiCounter);
        for (uiChannelCounter = 0; uiChannelCounter < 10; uiChannelCounter++)
        {
            // check, if we have such a channel inside the entries that belong to this config
            int iEntry = -1;
            for (uiEntryCounter = 0; (int)uiEntryCounter < clEntries.size(); uiEntryCounter++)
            {
                if (sADU07ChannelConfig.qvecChannelValues [clEntries [uiEntryCounter]].uiChannelNumber == uiChannelCounter)
                {
                    iEntry = sADU07ChannelConfig.qvecChannelValues [clEntries [uiEntryCounter]].uiSignalMapperIndex;
                }
            }

            if (iEntry >= 0)
            {
                QPushButton* pclTmpButton = new QPushButton;
                pclTmpButton->setEnabled(true);
                pclTmpButton->setText (QString::number (uiChannelCounter));
                pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 0);

                pclLayout->addWidget  (sADU07ChannelConfig.qvecChannelValues [iEntry].pbRFFilter,     uiChannelCounter + 1, 1);
                pclLayout->addWidget  (sADU07ChannelConfig.qvecChannelValues [iEntry].pbGainStage1,   uiChannelCounter + 1, 2);
                pclLayout->addWidget  (sADU07ChannelConfig.qvecChannelValues [iEntry].pbDCOffsetCorr, uiChannelCounter + 1, 3);
                pclLayout->addWidget  (sADU07ChannelConfig.qvecChannelValues [iEntry].pbGainStage2,   uiChannelCounter + 1, 4);
            }
            else
            {
                {
                    QPushButton* pclTmpButton = new QPushButton;
                    pclTmpButton->setEnabled(false);
                    pclTmpButton->setText (QString::number (uiChannelCounter));
                    pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 0);
                }

                {
                    QPushButton* pclTmpButton = new QPushButton;
                    pclTmpButton->setEnabled(false);
                    pclTmpButton->setText ("");
                    pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 1);
                }

                {
                    QPushButton* pclTmpButton = new QPushButton;
                    pclTmpButton->setEnabled(false);
                    pclTmpButton->setText ("");
                    pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 2);
                }

                {
                    QPushButton* pclTmpButton = new QPushButton;
                    pclTmpButton->setEnabled(false);
                    pclTmpButton->setText ("");
                    pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 3);
                }

                {
                    QPushButton* pclTmpButton = new QPushButton;
                    pclTmpButton->setEnabled(false);
                    pclTmpButton->setText ("");
                    pclLayout->addWidget  (pclTmpButton, uiChannelCounter + 1, 4);
                }
            }
        }

        {
            QIcon clIcon;
            clIcon.addFile(QString::fromUtf8(":/icons/Resources/pic/Button_ShiftLeftLeft.png"), QSize(), QIcon::Normal, QIcon::Off);
            qvecEntriesToBeShown [uiCounter].pbGoBack->setIcon          (clIcon);
            qvecEntriesToBeShown [uiCounter].pbGoBack->setIconSize      (QSize(40, 40));
            qvecEntriesToBeShown [uiCounter].pbGoBack->setMinimumHeight (56);
        }

        {
            QIcon clIcon;
            clIcon.addFile(QString::fromUtf8(":/icons/Resources/pic/Button_OK.png"), QSize(), QIcon::Normal, QIcon::Off);
            qvecEntriesToBeShown [uiCounter].pbApply->setIcon          (clIcon);
            qvecEntriesToBeShown [uiCounter].pbApply->setIconSize      (QSize(45, 45));
            qvecEntriesToBeShown [uiCounter].pbApply->setMinimumHeight (56);
        }

        {
            QIcon clIcon;
            clIcon.addFile(QString::fromUtf8(":/icons/Resources/pic/Button_ShiftRightRight.png"), QSize(), QIcon::Normal, QIcon::Off);
            qvecEntriesToBeShown [uiCounter].pbSkip->setIcon          (clIcon);
            qvecEntriesToBeShown [uiCounter].pbSkip->setIconSize      (QSize(40, 40));
            qvecEntriesToBeShown [uiCounter].pbSkip->setMinimumHeight (56);
        }

        QHBoxLayout* pclButtonLayout = new QHBoxLayout();
        pclLayout1->addLayout(pclButtonLayout);
        pclButtonLayout->addWidget(qvecEntriesToBeShown [uiCounter].pbGoBack);
        pclButtonLayout->addWidget(qvecEntriesToBeShown [uiCounter].pbApply);
        pclButtonLayout->addWidget(qvecEntriesToBeShown [uiCounter].pbSkip);

        ui->clEntryLayout->addWidget(qvecEntriesToBeShown [uiCounter].pclGroupBox);
        qvecEntriesToBeShown [uiCounter].pclGroupBox->setVisible(false);
    }

    // start with display of first config group
    qvecEntriesToBeShown [0].pclGroupBox->setVisible(true);

    // connect signal mapper to private slots
    connect (&(this->qsmapGainStage1),   SIGNAL(mapped(int)), this, SLOT(on_pbGainStage1_clicked   (int)));
    connect (&(this->qsmapGainStage2),   SIGNAL(mapped(int)), this, SLOT(on_pbGainStage2_clicked   (int)));
    connect (&(this->qsmapDCOffsetCorr), SIGNAL(mapped(int)), this, SLOT(on_pbDCOffsetCorr_clicked (int)));
    connect (&(this->qsmapRFFilter),     SIGNAL(mapped(int)), this, SLOT(on_pbRFFilter_clicked     (int)));
    connect (&(this->qsmapApply),        SIGNAL(mapped(int)), this, SLOT(on_pbApply_clicked        (int)));
    connect (&(this->qsmapSkip),         SIGNAL(mapped(int)), this, SLOT(on_pbSkip_clicked         (int)));
    connect (&(this->qsmapGoBack),       SIGNAL(mapped(int)), this, SLOT(on_pbGoBack_clicked       (int)));

    return (bRetValue);
}


void ImportAutoGainOffsetDialog::on_pbGainStage1_clicked(int iIndex)
{
    // declaration of variables
    QStringList qstrlValidGains;

    if (this->mData != NULL)
    {
        qstrlValidGains = this->mData->allowedDiscreteValuesForGain1AtChannel(this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiChannelNumber);

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidGains, "Select GainStage-1 Gain");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiGainStage1 = pclKeyPad->qstrRetValue.toUInt ();
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].pbGainStage1->setText(QString::number (this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiGainStage1));
        }

        delete (pclKeyPad);
    }
}


void ImportAutoGainOffsetDialog::on_pbGainStage2_clicked(int iIndex)
{
    // declaration of variables
    QStringList qstrlValidGains;

    if (this->mData != NULL)
    {
        qstrlValidGains = this->mData->allowedDiscreteValuesForGain2AtChannel(this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiChannelNumber);

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidGains, "Select GainStage-2 Gain");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiGainStage2 = pclKeyPad->qstrRetValue.toUInt ();
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].pbGainStage2->setText(QString::number (this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiGainStage2));
        }

        delete (pclKeyPad);
    }
}


void ImportAutoGainOffsetDialog::on_pbDCOffsetCorr_clicked (int iIndex)
{
    // declaration of variables
    double  dDacMin;
    double  dDacMax;
    double  dValue;
    bool    bOk;
    QString qstrTmp;

    // get min / max values for DAC
    dDacMin = this->mData->getMinDacAtChannel (this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiChannelNumber).toDouble();
    dDacMax = this->mData->getMaxDacAtChannel (this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiChannelNumber).toDouble();

    // declaration of variables
    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, true, " DC Offset Correction [-2500 mV .. 2500 mV] ", this);
    if (pclKeyPad->exec() == true)
    {
        dValue = pclKeyPad->text().toDouble(&bOk);
        if ((bOk    == true)    &&
            (dValue >= dDacMin) &&
            (dValue <= dDacMax))
        {
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].iDCOffsetCorr = (int) dValue;
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].pbDCOffsetCorr->setText(QString::number (this->sADU07ChannelConfig.qvecChannelValues [iIndex].iDCOffsetCorr));
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


void ImportAutoGainOffsetDialog::on_pbRFFilter_clicked (int iIndex)
{
    // declaration of variables
    QStringList     qstrlValidRFFilters;
    QStringList     qstrlValidRFFiltersGUI;
    QString         qstrTmp;
    unsigned int    uiCounter;
    int             iTmp;

    // get valid RF Filters
    qstrlValidRFFilters = mData->allowedRfFilterValuesAtChannel (this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiChannelNumber);

    // convert them into resistance values
    qstrlValidRFFiltersGUI.clear();
    for (uiCounter = 0; (int)uiCounter < qstrlValidRFFilters.size(); uiCounter++)
    {
        qstrlValidRFFiltersGUI.append(this->clRFFilterMapper.toVisual(qstrlValidRFFilters [uiCounter]));
    }

    if (this->mData != NULL)
    {
        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidRFFiltersGUI, "Select Radio Filter");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].qstrRFFilter = this->clRFFilterMapper.toCode(pclKeyPad->qstrRetValue);
            this->sADU07ChannelConfig.qvecChannelValues [iIndex].pbRFFilter->setText(this->clRFFilterMapper.toVisual(this->sADU07ChannelConfig.qvecChannelValues [iIndex].qstrRFFilter));

            // ask to set the same value for all entries of the same type
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            iTmp = QMessageBox::question(this, qstrTmp, tr (C_STRING_GUI_INPORT_AUTOGAIN_SAME_RF), QMessageBox::Yes, QMessageBox::No);
            if (iTmp == QMessageBox::Yes)
            {
                for (uiCounter = 0; (int)uiCounter < this->sADU07ChannelConfig.qvecChannelValues.size(); uiCounter++)
                {
                    if ((this->sADU07ChannelConfig.qvecChannelValues [uiCounter].qstrBoardType.contains  (this->sADU07ChannelConfig.qvecChannelValues [iIndex].qstrBoardType)  == true) &&
                        (this->sADU07ChannelConfig.qvecChannelValues [uiCounter].qstrFilterType == this->sADU07ChannelConfig.qvecChannelValues [iIndex].qstrFilterType)                 &&
                        (this->sADU07ChannelConfig.qvecChannelValues [uiCounter].uiInput        == this->sADU07ChannelConfig.qvecChannelValues [iIndex].uiInput)                        &&
                        ((int)uiCounter                                                         != iIndex))
                    {
                        this->sADU07ChannelConfig.qvecChannelValues [uiCounter].qstrRFFilter = this->sADU07ChannelConfig.qvecChannelValues [iIndex].qstrRFFilter;
                        this->sADU07ChannelConfig.qvecChannelValues [uiCounter].pbRFFilter->setText (this->clRFFilterMapper.toVisual(this->sADU07ChannelConfig.qvecChannelValues [uiCounter].qstrRFFilter));
                    }
                }
            }
        }

        delete (pclKeyPad);
    }
}


void ImportAutoGainOffsetDialog::on_jobListTableView_doubleClicked (const QModelIndex& index)
{
    // declaration of variables

    if (index.column() == 5)
    {
        ui->jobListTableView->model()->setData(index, 1, Qt::EditRole);
    }
}


void ImportAutoGainOffsetDialog::on_pbApply_clicked (int iIndex)
{
    // declaration of variables
    unsigned int uiCounter;
    unsigned int uiAutoGainCounter;
    unsigned int uiChannelCounter;
    unsigned int uiChannelID;
    unsigned int uiChannelNum;
    unsigned int uiInput;
    QString      qstrBoardType;
    QString      qstrFilterType;
    QString      qstrSensorType;
    QString      qstrChannelType;
    QString      qstrTmp;
    double       dTmp;

    QStringList qstrlLFBoardRFFilters;
    QMap <unsigned int, QString> qmapLFBoardRFFilters;

    QStringList qstrlMFBoardRFFilters;
    QMap <unsigned int, QString> qmapMFBoardRFFilters;

    qstrlMFBoardRFFilters.clear ();


    for (uiCounter = 0; uiCounter < this->mData->jobCount(); uiCounter++)
    {
        if (this->mData->getJobAt(uiCounter)->getLocked() == false)
        {
            qDebug () << "[" << __PRETTY_FUNCTION__ << "]  ====> job number" << uiCounter << "<====";

            qDebug () << "[" << __PRETTY_FUNCTION__ << "] searching for smalles E-field radio filter ...";







            for (uiChannelCounter = 0; uiChannelCounter < this->mData->getJobAt(uiCounter)->getNumChannels(); uiChannelCounter++)
            {
                if (this->mData->getJobAt(uiCounter)->getIsChannelEnabled(uiChannelCounter) == true)
                {
                    AdbBoardInfo::Spt pclBoardInfo = this->mData->getHwConfig()->getAdbBoardInfoForChannel (this->mData->getJobAt(uiCounter)->getJobChannelConfigForChannel(uiChannelCounter)->getChannelId());

                    qstrBoardType  = pclBoardInfo->getName();
                    qstrFilterType = this->mData->getJobAt(uiCounter)->getLpHpFilterForChannel(uiChannelCounter);
                    if (qstrFilterType.contains("Off") == true)
                    {
                        qstrFilterType.clear();
                    }
                    uiChannelID     = this->mData->getJobAt(uiCounter)->getJobChannelConfigForChannel(uiChannelCounter)->getChannelId();
                    uiChannelNum    = uiChannelCounter;
                    uiInput         = this->mData->getSensorHWSelectedInputAtChannel    (uiChannelID).toUInt();
                    qstrSensorType  = this->mData->getSensorHWSensorTypeInputAtChannel  (uiChannelID);
                    qstrChannelType = this->mData->getSensorHWChannelTypeInputAtChannel (uiChannelID);

                    // now search for a fitting entry inside the AutoGain / AutoOffset vector
                    QList<unsigned int> clEntries = qmapEntriesToBeShown.values (iIndex);
                    for (uiAutoGainCounter = 0; (int)uiAutoGainCounter < clEntries.size(); uiAutoGainCounter++)
                    {
                        if ((this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrBoardType.contains (qstrBoardType) == true)           &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrFilterType                         == qstrFilterType) &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiInput                                == uiInput)        &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiChannelNumber                        == uiChannelID))
                        {

                            if (((qstrChannelType.contains ("E") == true) || (qstrChannelType.contains ("RE"))) &&
                                 (qstrBoardType.contains   ("LF")))
                            {
                                if (qstrlLFBoardRFFilters.contains (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrRFFilter) == false)
                                {
                                    qstrlLFBoardRFFilters.push_back (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrRFFilter);
                                }
                            }

                            if (((qstrChannelType.contains ("E") == true) || (qstrChannelType.contains ("RE"))) &&
                                 (qstrBoardType.contains   ("MF")))
                            {
                                if (qstrlMFBoardRFFilters.contains (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrRFFilter) == false)
                                {
                                    qstrlMFBoardRFFilters.push_back (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrRFFilter);
                                }
                            }
                        }
                    }
                }
            }

            qDebug () << "[" << __PRETTY_FUNCTION__ << "] LF board E-Field RF filters:" << qstrlLFBoardRFFilters;
            qDebug () << "[" << __PRETTY_FUNCTION__ << "] MF board E-Field RF filters:" << qstrlMFBoardRFFilters;

            qmapLFBoardRFFilters.clear ();
            qmapMFBoardRFFilters.clear ();

            for (uiChannelNum = 0; (int)uiChannelNum < qstrlLFBoardRFFilters.size(); uiChannelNum++)
            {
                qmapLFBoardRFFilters.insert (this->clRFFilterMapper.toVisual(qstrlLFBoardRFFilters [uiChannelNum]).toUInt (), qstrlLFBoardRFFilters [uiChannelNum]);
            }

            for (uiChannelNum = 0; (int)uiChannelNum < qstrlMFBoardRFFilters.size(); uiChannelNum++)
            {
                qmapMFBoardRFFilters.insert (this->clRFFilterMapper.toVisual(qstrlMFBoardRFFilters [uiChannelNum]).toUInt (), qstrlMFBoardRFFilters [uiChannelNum]);
            }

            qDebug () << "[" << __PRETTY_FUNCTION__ << "] LF board E-Field RF filters:" << qmapLFBoardRFFilters;
            qDebug () << "[" << __PRETTY_FUNCTION__ << "] MF board E-Field RF filters:" << qmapMFBoardRFFilters;

            for (uiChannelCounter = 0; uiChannelCounter < this->mData->getJobAt(uiCounter)->getNumChannels(); uiChannelCounter++)
            {
                if (this->mData->getJobAt(uiCounter)->getIsChannelEnabled(uiChannelCounter) == true)
                {
                    AdbBoardInfo::Spt pclBoardInfo = this->mData->getHwConfig()->getAdbBoardInfoForChannel (this->mData->getJobAt(uiCounter)->getJobChannelConfigForChannel(uiChannelCounter)->getChannelId());

                    qstrBoardType  = pclBoardInfo->getName();
                    qstrFilterType = this->mData->getJobAt(uiCounter)->getLpHpFilterForChannel(uiChannelCounter);
                    if (qstrFilterType.contains("Off") == true)
                    {
                        qstrFilterType.clear();
                    }
                    uiChannelID     = this->mData->getJobAt(uiCounter)->getJobChannelConfigForChannel(uiChannelCounter)->getChannelId();
                    uiChannelNum    = uiChannelCounter;
                    uiInput         = this->mData->getSensorHWSelectedInputAtChannel    (uiChannelID).toUInt();
                    qstrSensorType  = this->mData->getSensorHWSensorTypeInputAtChannel  (uiChannelID);
                    qstrChannelType = this->mData->getSensorHWChannelTypeInputAtChannel (uiChannelID);

                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] channel number:" << uiChannelCounter;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] channel ID....:" << uiChannelID;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] board type....:" << qstrBoardType;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] filter type...:" << qstrFilterType;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] sensor type...:" << qstrSensorType;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] sensor type...:" << qstrChannelType;
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] input.........:" << uiInput << "\n";

                    // now search for a fitting entry inside the AutoGain / AutoOffset vector
                    QList<unsigned int> clEntries = qmapEntriesToBeShown.values (iIndex);
                    for (uiAutoGainCounter = 0; (int)uiAutoGainCounter < clEntries.size(); uiAutoGainCounter++)
                    {
                        if ((this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrBoardType.contains (qstrBoardType) == true)           &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].qstrFilterType                         == qstrFilterType) &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiInput                                == uiInput)        &&
                            (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiChannelNumber                        == uiChannelID))
                        {
                            this->mData->getJobAt(uiCounter)->setGain1ForChannel    (uiChannelCounter, QString::number (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiGainStage1));
                            this->mData->getJobAt(uiCounter)->setGain2ForChannel    (uiChannelCounter, QString::number (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].uiGainStage2));
                            this->mData->getJobAt(uiCounter)->setDacForChannel      (uiChannelCounter, QString::number (this->sADU07ChannelConfig.qvecChannelValues [clEntries [uiAutoGainCounter]].iDCOffsetCorr));

                            // for LF board use smallest RF filter that was found for E-Fields
                            if (qstrBoardType.contains ("LF") == true)
                            {
                                this->mData->getJobAt(uiCounter)->setRfFilterForChannel (uiChannelCounter, qmapLFBoardRFFilters.begin ().value ());
                            }
                            else
                            {
                                // for MF board in LF mode use smallest RF filter that was found for E-Fields
                                if ((qstrBoardType.contains ("MF") == true) && (this->mData->getJobAt(uiCounter)->getFrequency (qstrTmp, dTmp) <= 4096))
                                {
                                    this->mData->getJobAt(uiCounter)->setRfFilterForChannel (uiChannelCounter, qmapMFBoardRFFilters.begin ().value ());
                                }
                                else
                                {
                                    this->mData->getJobAt(uiCounter)->setRfFilterForChannel (uiChannelCounter, "");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    this->qvecEntriesToBeShown [iIndex].bApplied = true;

    if ((iIndex + 1) >= this->qvecEntriesToBeShown.size())
    {
        this->close();
    }
    else
    {
        this->qvecEntriesToBeShown [iIndex    ].pclGroupBox->setVisible(false);
        this->qvecEntriesToBeShown [iIndex + 1].pclGroupBox->setVisible(true);
    }
}


void ImportAutoGainOffsetDialog::on_pbCancel_clicked ()
{
    this->close();
}


void ImportAutoGainOffsetDialog::on_pbSkip_clicked (int iIndex)
{

    this->qvecEntriesToBeShown [iIndex].bSkipped = true;

    if ((iIndex + 1) < this->qvecEntriesToBeShown.size())
    {
        this->qvecEntriesToBeShown [iIndex    ].pclGroupBox->setVisible(false);
        this->qvecEntriesToBeShown [iIndex + 1].pclGroupBox->setVisible(true);
    }
    else
    {
        this->close();
    }
}


void ImportAutoGainOffsetDialog::on_pbGoBack_clicked (int iIndex)
{
    if ((iIndex - 1) >= 0)
    {
        this->qvecEntriesToBeShown [iIndex    ].pclGroupBox->setVisible(false);
        this->qvecEntriesToBeShown [iIndex - 1].pclGroupBox->setVisible(true);
    }
}





