#include "comparejoblists.h"
#include "QFileDialog"

#include "ui_comparejoblists.h"

CompareJobLists::CompareJobLists(DataModel* pclJobList1,
                                 QString&   qstrCmpJobListPath,
                                 QWidget*   parent) :
    pclSrcJobList   (pclJobList1),
    QDialog         (parent),
    ui              (new Ui::CompareJobLists)
{
    ui->setupUi(this);

    QFileInfo clFileInfo (qstrCmpJobListPath);
    QDir clMainDir = clFileInfo.absoluteDir();

    if (not clMainDir.cdUp())
    {
        throw std::runtime_error("cant cd to root of ADU-07 dir here.");
    }

    pclCmpJobList = new DataModel          ();
    pclCmpJobList->setJobDirectory         (clMainDir.canonicalPath());
    pclCmpJobList->loadAndParseJobListFile (qstrCmpJobListPath);
    pclCmpJobList->loadAndParseHwData      (clFileInfo.canonicalPath());

    qvecSrcJoblistButtons.clear();
    qvecSrcJobListAttr.clear   ();
    qvecCmpJoblistButtons.clear();
    qvecCmpJobListAttr.clear   ();

    dPixelPerSecond = 4.0;
    ui->slZoom->setValue(dPixelPerSecond * 100.0);

    this->drawChart();

    this->setWindowTitle("Compare Joblists");

    QPalette qplTmp = ui->labNoDiff->palette();
    qplTmp.setColor(ui->labNoDiff->backgroundRole(), Qt::green);
    ui->labNoDiff->setPalette(qplTmp);
    ui->labNoDiff->setAutoFillBackground(true);

    qplTmp.setColor(ui->labSampleFreqDiff->backgroundRole(), Qt::yellow);
    ui->labSampleFreqDiff->setPalette(qplTmp);
    ui->labSampleFreqDiff->setAutoFillBackground(true);

    qplTmp.setColor(ui->labTXParamDiff->backgroundRole(), Qt::blue);
    ui->labTXParamDiff->setPalette(qplTmp);
    ui->labTXParamDiff->setAutoFillBackground(true);

    qplTmp.setColor(ui->labTimeDiff->backgroundRole(), Qt::red);
    ui->labTimeDiff->setPalette(qplTmp);
    ui->labTimeDiff->setAutoFillBackground(true);

    ui->pbRelativeStart->setCheckable(true);
    ui->pbRelativeStart->setChecked  (false);
}


CompareJobLists::~CompareJobLists()
{
    delete ui;

    delete (this->pclCmpJobList);
    pclSrcJobList = NULL;

    qvecSrcJoblistButtons.clear();
    qvecSrcJobListAttr.clear   ();
    qvecCmpJoblistButtons.clear();
    qvecCmpJobListAttr.clear   ();
}


bool CompareJobLists::drawChart(void)
{
    // declaration of variables
    bool            bRetValue = true;
    ssize_t         sCounter;
    ssize_t         sMaxJobs;
    ssize_t         sSameNumJobs;
    QString         qstrSrcToolTip;
    QString         qstrCmpToolTip;
    bool            bStartDiff;
    bool            bDurDiff;
    bool            bTXFreqDiff;
    bool            bTXDipTimeDiff;
    bool            bRXFreqDiff;

    // check, which joblist has the earlier start-time
    if (this->pclSrcJobList->getJobAt(0)->getStartDateTimeUnix() < this->pclCmpJobList->getJobAt(0)->getStartDateTimeUnix())
    {
        this->sStartTimeCmpJoblist = this->pclCmpJobList->getJobAt(0)->getStartDateTimeUnix() - this->pclSrcJobList->getJobAt(0)->getStartDateTimeUnix();
        this->sStartTimeSrcJoblist = 0;
    }
    else
    {
        this->sStartTimeSrcJoblist = this->pclSrcJobList->getJobAt(0)->getStartDateTimeUnix() - this->pclCmpJobList->getJobAt(0)->getStartDateTimeUnix();
        this->sStartTimeCmpJoblist = 0;
    }

    // parse the source joblist to create the button vector
    parseJoblist (&(*this->pclSrcJobList), ui->hlSrcJobList, this->qvecSrcJoblistButtons, this->qvecSrcJobListAttr, this->sStartTimeSrcJoblist);
    parseJoblist (&(*this->pclCmpJobList), ui->hlCmpJobList, this->qvecCmpJoblistButtons, this->qvecCmpJobListAttr, this->sStartTimeCmpJoblist);

    if (this->sStartTimeSrcJoblist != this->sStartTimeCmpJoblist)
    {
        QPalette qplTmp = ui->labLedStartTimeSynced->palette ();
        qplTmp.setColor(ui->labNoDiff->backgroundRole(), Qt::red);
        ui->labLedStartTimeSynced->setPalette(qplTmp);
        ui->labLedStartTimeSynced->setAutoFillBackground(true);
        ui->labLedStartTimeSynced->setText("NO");
    }
    else
    {
        QPalette qplTmp = ui->labLedStartTimeSynced->palette ();
        qplTmp.setColor(ui->labNoDiff->backgroundRole(), Qt::green);
        ui->labLedStartTimeSynced->setPalette(qplTmp);
        ui->labLedStartTimeSynced->setAutoFillBackground(true);
        ui->labLedStartTimeSynced->setText("YES");
    }

    // now compare the single jobs of the joblists and check for differences
    // determine, which one of the joblists is smaller one
    if (this->qvecSrcJobListAttr.size() < this->qvecCmpJobListAttr.size())
    {
        sSameNumJobs = this->qvecSrcJobListAttr.size();
        sMaxJobs     = this->qvecCmpJobListAttr.size();
    }
    else
    {
        sSameNumJobs = this->qvecCmpJobListAttr.size();
        sMaxJobs     = this->qvecSrcJobListAttr.size();
    }

    for (sCounter = 0; sCounter < sMaxJobs; sCounter++)
    {
        bStartDiff  = false;
        bDurDiff    = false;
        bTXFreqDiff = false;
        bTXDipTimeDiff  = false;
        bRXFreqDiff = false;
        qstrSrcToolTip.clear();
        qstrCmpToolTip.clear();

        if (sCounter < sSameNumJobs)
        {
            if (this->qvecSrcJobListAttr [sCounter].uiStartTime != this->qvecCmpJobListAttr [sCounter].uiStartTime)
            {
                qstrSrcToolTip = "Starttime differs between jobs!";
                bStartDiff  = true;
            }

            if (this->qvecSrcJobListAttr [sCounter].uiDuration != this->qvecCmpJobListAttr [sCounter].uiDuration)
            {
                qstrSrcToolTip += "\nDuration differs between jobs!";
                bDurDiff     = true;
            }

            if (this->qvecSrcJobListAttr [sCounter].dTXFreq != this->qvecCmpJobListAttr [sCounter].dTXFreq)
            {
                qstrSrcToolTip += "\nTransmitter (TX) Frequency differs between jobs!";
                bTXFreqDiff  = true;
            }

            if (this->qvecSrcJobListAttr [sCounter].dTXDipTime != this->qvecCmpJobListAttr [sCounter].dTXDipTime)
            {
                qstrSrcToolTip += "\nTransmitter (TX) Dipole Time differs between jobs!";
                bTXDipTimeDiff = true;
            }

            if (this->qvecSrcJobListAttr [sCounter].dRXFreq != this->qvecCmpJobListAttr [sCounter].dRXFreq)
            {
                qstrSrcToolTip += "\nSample (RX) Frequency differs between jobs!";
                bRXFreqDiff  = true;
            }

            {
                QPalette qplTmp = this->qvecSrcJoblistButtons [sCounter]->palette();
                qplTmp.setColor(QPalette::Button,     Qt::green);
                this->qvecSrcJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecSrcJoblistButtons [sCounter]->setAutoFillBackground(true);
                this->qvecCmpJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecCmpJoblistButtons [sCounter]->setAutoFillBackground(true);
            }

            // in first step colour the button in any case to show conflicts
            if (bRXFreqDiff == true)
            {
                QPalette qplTmp = this->qvecSrcJoblistButtons [sCounter]->palette();
                qplTmp.setColor(QPalette::Button,     Qt::yellow);
                this->qvecSrcJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecSrcJoblistButtons [sCounter]->setAutoFillBackground(true);
                this->qvecCmpJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecCmpJoblistButtons [sCounter]->setAutoFillBackground(true);
            }

            if ((bTXFreqDiff    == true) ||
                (bTXDipTimeDiff == true))
            {
                QPalette qplTmp = this->qvecSrcJoblistButtons [sCounter]->palette();
                qplTmp.setColor(QPalette::Button,     Qt::blue);
                this->qvecSrcJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecSrcJoblistButtons [sCounter]->setAutoFillBackground(true);
                this->qvecCmpJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecCmpJoblistButtons [sCounter]->setAutoFillBackground(true);
            }

            if ((bStartDiff == true) ||
                (bDurDiff   == true))
            {
                QPalette qplTmp = this->qvecSrcJoblistButtons [sCounter]->palette();
                qplTmp.setColor(QPalette::Button,     Qt::red);
                this->qvecSrcJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecSrcJoblistButtons [sCounter]->setAutoFillBackground(true);
                this->qvecCmpJoblistButtons [sCounter]->setPalette(qplTmp);
                this->qvecCmpJoblistButtons [sCounter]->setAutoFillBackground(true);
            }
        }

        // add tooltip
        qstrCmpToolTip = qstrSrcToolTip;

        if (sCounter < this->qvecSrcJobListAttr.size())
        {
            qstrSrcToolTip.prepend("\nSample Freq.: " + QString::number (this->qvecSrcJobListAttr [sCounter].dRXFreq) + "Hz\n");
            qstrSrcToolTip.prepend("\nTX Freq.: "     + QString::number (this->qvecSrcJobListAttr [sCounter].dTXFreq) + "Hz");
            qstrSrcToolTip.prepend("ID: "             + QString::number (sCounter + 1));

            QFont clFont;
            clFont = this->qvecSrcJoblistButtons [sCounter]->font();
            clFont.setFamily("Courier");
            this->qvecSrcJoblistButtons [sCounter]->setFont(clFont);
            this->qvecSrcJoblistButtons [sCounter]->setToolTip(qstrSrcToolTip);
        }

        if (sCounter < this->qvecCmpJobListAttr.size())
        {
            qstrCmpToolTip.prepend("\nSample Freq.: " + QString::number (this->qvecCmpJobListAttr [sCounter].dRXFreq) + "Hz\n");
            qstrCmpToolTip.prepend("\nTX Freq.: "     + QString::number (this->qvecCmpJobListAttr [sCounter].dTXFreq) + "Hz");
            qstrCmpToolTip.prepend("ID: "             + QString::number (sCounter + 1));

            QFont clFont;
            clFont = this->qvecCmpJoblistButtons [sCounter]->font();
            clFont.setFamily("Courier");
            this->qvecCmpJoblistButtons [sCounter]->setFont(clFont);
            this->qvecCmpJoblistButtons [sCounter]->setToolTip(qstrCmpToolTip);
        }
    }

    return (bRetValue);
}


bool CompareJobLists::parseJoblist(DataModel*                pclJobList,
                                          QHBoxLayout*              pclLayout,
                                          QVector<QPushButton*>&    qvecButtons,
                                          QVector<sJobAtttributes>& qvectAttributes,
                                          time_t&                   sStartTimeAbs)
{
    // declaration of variables
    bool bRetValue = true;
    size_t          sCounter;
    unsigned int    uiStartTime;
    unsigned int    uiStartFirstJob;
    unsigned int    uiDuration;
    unsigned int    uiLastStop = 0;
    unsigned int    uiSpacer;
    unsigned int    uiTmp;
    unsigned int    uiStartTimeOffset = 0;
    QString         qstrTmp;
    QString         qstrTmp2;
    sJobAtttributes sTmp;
    double          dTmp;

    // clear all elemets
    QLayoutItem* child;
    while ((child = pclLayout->takeAt(0)) != 0)
    {
        if (child->widget() != NULL)
        {
            delete (child->widget());
        }
        delete child;
    }
    qvecButtons.clear    ();
    qvectAttributes.clear();

    // parse the source joblist to create the button vector
    for (sCounter = 0; sCounter < pclJobList->jobCount(); sCounter++)
    {
        if (sCounter == 0)
        {
            uiStartFirstJob = pclJobList->getJobAt(sCounter)->getStartDateTimeUnix();

            // check, if absolute start-times, or relative start-times shall be shown
            if (ui->pbRelativeStart->isChecked() == false)
            {
                uiTmp = sStartTimeAbs * dPixelPerSecond;
                if (uiTmp < 5)
                {
                    uiTmp = 5;
                }
                pclLayout->addSpacing(uiTmp * dPixelPerSecond);
                uiStartTimeOffset = sStartTimeAbs;
            }

            sStartTimeAbs   = uiStartFirstJob;
        }

        uiStartTime = pclJobList->getJobAt(sCounter)->getStartDateTimeUnix();
        uiStartTime = uiStartTime - uiStartFirstJob;
        uiDuration  = pclJobList->getJobAt(sCounter)->getDuration().asSeconds();

        // if this is not the first job, add a spacer
        if (sCounter > 0)
        {
            uiSpacer = uiStartTime - uiLastStop;
            uiTmp    = uiSpacer * dPixelPerSecond;
            if (uiTmp < 5)
            {
                uiTmp = 5;
            }
            pclLayout->addSpacing(uiTmp * dPixelPerSecond);
        }
        uiLastStop  = uiStartTime + uiDuration;

        // now create button with according size and add at according position to frame
        QPushButton* pclNewButton = new QPushButton ();
        qvecButtons.append(pclNewButton);
        uiTmp = uiDuration * dPixelPerSecond;
        if (uiTmp < 5)
        {
            uiTmp = 5;
        }
        qvecButtons.back()->setMinimumWidth (uiTmp);
        qvecButtons.back()->setMinimumHeight(100);
        qvecButtons.back()->setMaximumWidth (uiTmp);
        qvecButtons.back()->setMaximumHeight(100);

        qstrTmp  = "ID: "     + QString::number (sCounter + 1);
        qstrTmp += "\nf_TX: " + pclJobList->getJobAt(sCounter)->getTXMBaseFreq() + "Hz";
        qstrTmp += "\nf_RX: " + QString::number (pclJobList->getJobAt(sCounter)->getFrequency(qstrTmp2, dTmp)) + "Hz";
        qvecButtons.back()->setText (qstrTmp);

        pclLayout->addWidget (qvecButtons.back());

        // store main attributes of this job inside vector
        if (ui->pbRelativeStart->isChecked() == false)
        {
            sTmp.uiStartTime = uiStartTime + uiStartTimeOffset;
        }
        else
        {
            sTmp.uiStartTime = uiStartTime;
        }
        sTmp.uiDuration  = uiDuration;
        sTmp.dTXFreq     = pclJobList->getJobAt(sCounter)->getTXMBaseFreq().toDouble();
        sTmp.dRXFreq     = pclJobList->getJobAt(sCounter)->getFrequency    (qstrTmp2, dTmp);
        sTmp.dTXDipTime  = pclJobList->getJobAt(sCounter)->getTXMDipTXTime ().toDouble();
        qvectAttributes.append(sTmp);
    }
    pclLayout->addStretch();

    pclJobList = NULL;

    return (bRetValue);
}


void CompareJobLists::on_slZoom_valueChanged (int iNewValue)
{
    this->dPixelPerSecond = ((double) iNewValue) / 100.0;
    this->drawChart();
}


void CompareJobLists::on_pbOK_clicked (void)
{
    this->close();
}


void CompareJobLists::on_pbRelativeStart_clicked (void)
{
    this->drawChart();
}





