#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "ui_about.h"
#include "ui_datetimeselector.h"
#include "simple_job_popup.h"
#include "default_values.h"
#include "keypad.h"
#include "tools_types.h"
#include "Strings.h"
#include "comparejoblists.h"
#include "fastbuttonselectpad.h"
#include "freekeypadwindow.h"
#include "timer.h"
#include "dialdialog.h"

#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QErrorMessage>
#include <QSettings>
#include <QCloseEvent>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QUrl>

#include <stdexcept>
#include <iostream>



////////////////////////////////////////////////////////////////////////////////
// 
// Gui related stuff..

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ui_AboutDialog(new Ui::AboutDialog),
    ui_DateTimeSelector(new Ui::DateTimeSelector)
{
    // declaration of variables
    QString qstrTmp;
    QString qstrTemp;

    qstrTemp = SVN_REVISION_STRING;
    qstrTemp = qstrTemp.right   (qstrTemp.size () - qstrTemp.indexOf (' '));
    qstrTemp = qstrTemp.left    (qstrTemp.indexOf ('$'));
    qstrTemp = qstrTemp.trimmed ();

    BuildManifest::VersionString += qstrTemp.toStdString();

    // setup GUI - connect all slots and signals
    ui->setupUi(this);

    // initialise the "settings" class
    pclSettings = new QSettings ("Metronix", BuildManifest::ApplicationName.c_str());

    // all this needs a bit cleanup...
    wireupDataModel(); // set the data model on all the tabs...

    mAboutDialog = new QDialog( this );
    ui_AboutDialog -> setupUi( mAboutDialog );
    ui_AboutDialog -> versionLabel -> setText(QApplication::translate("AboutDialog", BuildManifest::VersionString.c_str()  , 0));
    ui_AboutDialog -> dateLabel    -> setText(QApplication::translate("AboutDialog", BuildManifest::BuildDate.c_str()      , 0));

    // set window title
    qstrTmp  = BuildManifest::ApplicationName.c_str();
    qstrTmp += " - ";
    qstrTmp += BuildManifest::VersionString.c_str();
    this->setWindowTitle(qstrTmp);

    setupViews();
    setupActions();
    setupSignals();

    ui -> tabWidget   -> setEnabled( false );
    ui -> cbCSAMTMode -> setEnabled( false );

    pclCompareJoblists = NULL;

    enableCSAMTStuff( false );
    mDateTimeSelector = new DateTimeSelector( this );
    ui->shiftStartEdit->setDisplayFormat(DefaultValues::DateTimeFormat);

    // load joblist templates
    this->loadJobListTemplates ();

    bCtrlKeyPressed = false;

    bHasUnsavedChanges = false;

    addTooTips ();

    readSettings();

    goToJobTab();
}

MainWindow::~MainWindow()
{
    // delete the factories!
    delete ui;
    delete ui_AboutDialog;
    delete ui_DateTimeSelector;
}

void MainWindow::readSettings()
{
    QSettings settings;
    restoreGeometry( settings.value("MainWindow/geometry").toByteArray());
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
}

void MainWindow::closeEvent( QCloseEvent * event )
{
    // declaration of variables
    int iButton;
    QString qstrTmp;

    // check, if we have unsaved changes. if so, ask, if they should be saved now
    if (this->bHasUnsavedChanges == true)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        iButton = QMessageBox::question(this, qstrTmp, tr (C_STRING_GUI_SAVE_CHANGES), QMessageBox::Yes, QMessageBox::No);
        if (iButton == QMessageBox::Yes)
        {
            this->saveFiles(false);
        }
    }

    writeSettings();
    event -> accept();
}


void MainWindow::setAndLoadJobListFile( const QString& jobfileName ) 
{
    loadJobListFile( jobfileName );
}


void MainWindow::setupActions()
{
    QObject::connect (ui->actionOpen     , SIGNAL( triggered() ), this, SLOT( openFile()         ) , Qt::UniqueConnection);
    //QObject::connect (ui->actionSave     , SIGNAL( triggered() ), this, SLOT( saveFiles()          , Qt::UniqueConnection);
    QObject::connect (ui->actionQuit     , SIGNAL( triggered() ), this, SLOT( close()            ) , Qt::UniqueConnection);
    QObject::connect (ui->actionAbout    , SIGNAL( triggered() ), this, SLOT( showAbout()        ) , Qt::UniqueConnection);
    // QObject::connect (ui->actionSettings , SIGNAL( triggered() ), this, SLOT( updateSettings()   ) , Qt::UniqueConnection);
    // QObject::connect (ui->actionNew      , SIGNAL( triggered() ), this, SLOT( createNewJoblist() ) , Qt::UniqueConnection);
}

void MainWindow::setupSignals() 
{

    // job tab
    QObject::connect( &mData , SIGNAL( jobDataChanged() ) , this , SLOT( updateNonQtItemModelControlledGui() ) , Qt::UniqueConnection ); // FIXME: rename this slot; it is only related to the mainwindow.
    // sensors tab 
    QObject::connect( &mData , SIGNAL( sensorConfigLoaded() ) , ui -> sensorsWidget  , SLOT( handleSensorConfigLoaded() )  , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( sensorConfigChanged() ), ui -> sensorsWidget , SLOT( handleSensorConfigChanged() ) , Qt::UniqueConnection );
    // job config tab 
    QObject::connect( &mData , SIGNAL( jobDataLoaded()  )      , ui -> jobConfigWidget  , SLOT( handleJobDataLoaded() )       , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( jobDataChanged() )      , ui -> jobConfigWidget  , SLOT( handleJobDataChanged() )      , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( sensorConfigLoaded() )  , ui -> jobConfigWidget  , SLOT( handleSensorConfigLoaded() )  , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( sensorConfigChanged() ) , ui -> jobConfigWidget  , SLOT( handleSensorConfigChanged() ) , Qt::UniqueConnection );
    // txm parameters tab
    QObject::connect( &mData , SIGNAL( jobDataLoaded()  )      , ui ->txmParameterWidget, SLOT( handleJobDataLoaded() )       , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( jobDataChanged() )      , ui ->txmParameterWidget, SLOT( handleJobDataChanged() )      , Qt::UniqueConnection );
    // txm electrode pos tab
    QObject::connect( &mData , SIGNAL( jobDataLoaded()  )      , ui ->txmElectrodePosWidget, SLOT( handleJobDataLoaded() )       , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( jobDataChanged() )      , ui ->txmElectrodePosWidget, SLOT( handleJobDataChanged() )      , Qt::UniqueConnection );
    // comments tab
    QObject::connect( &mData , SIGNAL( jobDataLoaded()  )      , ui ->commentsWidget, SLOT( handleJobDataLoaded() )       , Qt::UniqueConnection );
    QObject::connect( &mData , SIGNAL( jobDataChanged() )      , ui ->commentsWidget, SLOT( handleJobDataChanged() )      , Qt::UniqueConnection );

    ////////////////////////////////////////////////////////////////////////////////
    //
    // main window , switching tabs...

    QObject::connect( ui -> sensorsWidget        , SIGNAL( goToJobTab() ) , this, SLOT( goToJobTab() ) , Qt::UniqueConnection );
    QObject::connect( ui -> jobConfigWidget      , SIGNAL( goToJobTab() ) , this, SLOT( goToJobTab() ) , Qt::UniqueConnection );
    QObject::connect( ui -> commentsWidget       , SIGNAL( goToJobTab() ) , this, SLOT( goToJobTab() ) , Qt::UniqueConnection );
    QObject::connect( ui -> sensorPositionWidget , SIGNAL( goToJobTab() ) , this, SLOT( goToJobTab() ) , Qt::UniqueConnection );

    QObject::connect(ui->jobListTableView ,         SIGNAL(doubleClicked (const QModelIndex&)), this, SLOT(on_jobListTableView_doubleClicked(const QModelIndex&)) , Qt::UniqueConnection );

    ////////////////////////////////////////////////////////////////////////////////
    //
    // jobs tab ( we should put this into a widget of its own, i think.

    QObject::connect (ui->pbChangeDateTime , SIGNAL( clicked() ), this , SLOT( execDateTimeDialog() ) , Qt::UniqueConnection );
    //QObject::connect (ui->shiftStartEdit , SIGNAL( dateTimeChanged ( const QDateTime & ) ), &mData, SLOT( shiftStartChanged ( const QDateTime & ) )  , Qt::UniqueConnection );

    QObject::connect (ui->atBeginButton     ,SIGNAL(clicked()), this, SLOT( selectFirstJob() )     , Qt::UniqueConnection );
    QObject::connect (ui->oneForewardButton ,SIGNAL(clicked()), this, SLOT( selectNextJob() )      , Qt::UniqueConnection);
    QObject::connect (ui->atEndButton       ,SIGNAL(clicked()), this, SLOT( selectLastJob() )      , Qt::UniqueConnection);
    QObject::connect (ui->oneBackButton     ,SIGNAL(clicked()), this, SLOT( selectPreviousJob() )  , Qt::UniqueConnection );

    QObject::connect (ui->pbMoveJob1Up      ,SIGNAL(clicked()), this, SLOT(handleMoveJob1Up() )    , Qt::UniqueConnection );
    QObject::connect (ui->pbMoveJob1Down    ,SIGNAL(clicked()), this, SLOT(handleMoveJob1Down())   , Qt::UniqueConnection );

    QObject::connect (ui->rbAdjustPauseOff  , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPauseOff ( bool ) ) , Qt::UniqueConnection );
    QObject::connect (ui->rbAdjustPauseAuto , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPauseAuto( bool ) ) , Qt::UniqueConnection );
    QObject::connect (ui->rbAdjustPause1Min , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPause1Min( bool ) ) , Qt::UniqueConnection );
    QObject::connect (ui->rbAdjustPause2Min , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPause2Min( bool ) ) , Qt::UniqueConnection );
    QObject::connect (ui->rbAdjustPause3Min , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPause3Min( bool ) ) , Qt::UniqueConnection );
    QObject::connect (ui->rbAdjustPause5Min , SIGNAL( toggled( bool ) ), this , SLOT( handleRbAdjustPause5Min( bool ) ) , Qt::UniqueConnection );

    QObject::connect (ui->adjustToFullMinuteCheckbox  , SIGNAL( toggled( bool ) ), &mData, SLOT( setAdjustToMinute( bool ) ) , Qt::UniqueConnection );

    QObject::connect(&mData , SIGNAL(jobDataChanged()), this, SLOT (handleJobDataChanged()), Qt::UniqueConnection);

    // CSAMT stuff...
    QObject::connect ( ui -> cbCSAMTMode , SIGNAL( toggled( bool ) ), this , SLOT( handleCbCSAMTMode( bool ) )  , Qt::UniqueConnection );

    QObject::connect ( ui -> pbCSAMTTX_f      , SIGNAL(clicked()), this , SLOT( handlePbCSAMTTX_f() )      , Qt::UniqueConnection );
    QObject::connect ( ui -> pbCSAMTTX_t      , SIGNAL(clicked()), this , SLOT( handlePbCSAMTTX_t() )      , Qt::UniqueConnection );
    QObject::connect ( ui -> pbCSAMT_FSamples , SIGNAL(clicked()), this , SLOT( handlePbCSAMT_FSamples() ) , Qt::UniqueConnection );

    // create/delete job stuff
    QObject::connect ( ui -> appendJobButton, SIGNAL(clicked()), this, SLOT( handleAppendJob() )  , Qt::UniqueConnection );
    QObject::connect ( ui -> insertJobButton, SIGNAL(clicked()), this, SLOT( handleInsertJob() )  , Qt::UniqueConnection ); 
    QObject::connect ( ui -> deleteJobButton, SIGNAL(clicked()), this, SLOT( handleDeleteJob() )  , Qt::UniqueConnection );
       
}

void MainWindow::setupViews()
{
    #if QT_VERSION >= 0x050000
        ui -> jobListTableView -> horizontalHeader() -> setSectionResizeMode(QHeaderView::Stretch);
    #else
       // ui -> jobListTableView -> horizontalHeader() -> setResizeMode(QHeaderView::Stretch);
        ui->jobListTableView->resizeColumnsToContents();
    #endif

    ui -> jobListTableView -> setSelectionBehavior(QAbstractItemView::SelectRows);
    ui -> jobListTableView -> setSelectionMode(QTableView::SingleSelection);
}

void MainWindow::goToJobTab()
{
    ui -> tabWidget -> setCurrentIndex( 0 );
}

void MainWindow::goToSensorsTab()
{
    ui -> tabWidget -> setCurrentIndex( 1 );
}

void MainWindow::goToJobConfigTab()
{
    ui -> tabWidget -> setCurrentIndex( 2 );
}

void MainWindow::wireupDataModel()
{    
    // joblist uses a qt-model, not wired up here
    // gnmpf.. this is error-prone... anyone a better idea?
    ui->sensorsWidget->setDataModel         (&mData);
    ui->jobConfigWidget->setDataModel       (&mData);
    ui->commentsWidget->setDataModel        (&mData);
    ui->sensorPositionWidget->setDataModel  (&mData);
    ui->txmParameterWidget->setDataModel    (&mData);
    ui->txmElectrodePosWidget->setDataModel (&mData);
}


bool MainWindow::chooseJoblistMode(const bool bShowSelectionDialog)
{
    // declaration of variables
    bool bRetValue = true;
    QStringList qstrlValidJoblistModes;

    if ((bShowSelectionDialog == true) && (this->mData.isTXM() == false))
    {
        qstrlValidJoblistModes.clear();
        qstrlValidJoblistModes << C_JOBLIST_MODE_MT_GUI_STRING << C_JOBLIST_MODE_CSMT_GUI_STRING;

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidJoblistModes, C_STRING_GUI_CHOOSE_JOBLIST_MODE, this, false);
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            if (pclKeyPad->qstrRetValue.indexOf(C_JOBLIST_MODE_CSMT_GUI_STRING) != -1)
            {
                this->mData.setJoblistMode(C_JOBLIST_MODE_CSMT);
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_JOBLIST_MODE_MT_GUI_STRING) != -1)
            {
                this->mData.setJoblistMode(C_JOBLIST_MODE_MT);
            }
        }
        else
        {
            bRetValue = false;
        }

        delete (pclKeyPad);
    }

    // if the joblist is a TXM joblist, mode is always CS
    if (this->mData.isTXM() == true)
    {
        this->mData.setJoblistMode(C_JOBLIST_MODE_CSMT);
    }

    // check, if we are in Controlled Source mode, if not, hide all
    // CS mode fields.
    if (this->mData.getJoblistMode() != C_JOBLIST_MODE_CSMT)
    {
        ui->tabWidget->setTabEnabled  (3, false);
        ui->tabWidget->setTabEnabled  (4, false);
        ui->gbCSAMT->setVisible       (false);
        ui->jobListTableView->setColumnHidden (3, true);
        this->mData.setMeasurementMode(ControlData::NormalMeasurementMode, true);
    }
    else
    {
        ui->tabWidget->setTabEnabled  (3, true);
        ui->tabWidget->setTabEnabled  (4, true);
        ui->gbCSAMT->setVisible       (true);
        ui->jobListTableView->setColumnHidden (3, false);
    }

    // check, if we are working on a TXM joblist. In this case hide away the
    // sensor types and sensor spacing tabs
    if (this->mData.isTXM() == true)
    {
        ui->tabWidget->setTabEnabled  (5, false);
        ui->tabWidget->setTabEnabled  (6, false);
    }
    else
    {
        ui->tabWidget->setTabEnabled  (5, true);
        ui->tabWidget->setTabEnabled  (6, true);
    }


    return (bRetValue);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    // declaration of variables
    QString qstrTmp;
    QString qstrFileName;

    QList<QUrl> urls = event->mimeData()->urls();
    if (urls.isEmpty())
    {
        return;
    }

    qstrFileName = urls[0].toLocalFile();

    // show filename in window title
    qstrTmp  = BuildManifest::ApplicationName.c_str();
    qstrTmp += " - ";
    qstrTmp += BuildManifest::VersionString.c_str();
    qstrTmp += " - " + qstrFileName;
    this->setWindowTitle(qstrTmp);

    // load joblist
    loadJobListFile(qstrFileName);

    // check, if the mode of this Joblist is still undefined (no node in
    // ADUConf XML file.
    // if so, let the user choose the mode for the Joblist now.
    if (this->mData.getJoblistMode() == C_JOBLIST_MODE_INV)
    {
        this->chooseJoblistMode (true);
    }
    else
    {
        this->chooseJoblistMode (false);
    }

    // store new path in settings object
    pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
    pclSettings->setValue(C_SETTINGS_LAST_JOBLIST_DIR, qstrFileName);
    pclSettings->endGroup();

    // show warning window that automatic update of start-/stop-times is inactive
    checkAutoAdjustPauseActive ();

    bHasUnsavedChanges = false;
}


void MainWindow::openFile()
{
    // declaration of variables
    QString qstrTmp;
    QString qstrFileName;
    QString qstrFilter;
    QStringList qstrlValidJoblistModes;

    // read last open path from settings class
    pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
    qstrTmp = pclSettings->value(C_SETTINGS_LAST_JOBLIST_DIR).toString();
    pclSettings->endGroup();

    qstrFilter   = "ADUConf File (ADUConf.xml)";
    qstrFileName = QFileDialog::getOpenFileName(this, C_STRING_GUI_OPEN_JOBLIST_DIALOG, qstrTmp, tr("ADUConf File (ADUConf.xml)"), &qstrFilter);
    if (qstrFileName.isEmpty())
    {
        return;
    }

    // show filename in window title
    qstrTmp  = BuildManifest::ApplicationName.c_str();
    qstrTmp += " - ";
    qstrTmp += BuildManifest::VersionString.c_str();
    qstrTmp += " - " + qstrFileName;
    this->setWindowTitle(qstrTmp);

    // load joblist
    loadJobListFile(qstrFileName);

    // check, if the mode of this Joblist is still undefined (no node in
    // ADUConf XML file.
    // if so, let the user choose the mode for the Joblist now.
    if (this->mData.getJoblistMode() == C_JOBLIST_MODE_INV)
    {
        this->chooseJoblistMode (true);
    }
    else
    {
        this->chooseJoblistMode (false);
    }

    // store new path in settings object
    pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
    pclSettings->setValue(C_SETTINGS_LAST_JOBLIST_DIR, qstrFileName);
    pclSettings->endGroup();

    // show warning window that automatic update of start-/stop-times is inactive
    checkAutoAdjustPauseActive ();

    bHasUnsavedChanges = false;
}


void MainWindow::saveFiles(const bool bShowFileDialog)
{
    // declaration of variables
    QString qstrTmp;
    QString qstrFileName;

    if (this->mData.getIsReady() == true)
    {

        const QFileInfo jobListFileInfo (mData.getJobListFileInfo());

        if (bShowFileDialog == true)
        {
            QFileDialog fd;

            fd.selectFile    (jobListFileInfo.absoluteFilePath());
            fd.setDirectory  (jobListFileInfo.absolutePath());
            fd.setAcceptMode (QFileDialog::AcceptSave);

            if  (not (fd.exec() == QDialog::Accepted))
            {
                return;
            }

            QStringList fileList = fd.selectedFiles();
            if (fileList.size() != 1)
            {
                qDebug() << __PRETTY_FUNCTION__  << " WTF: fileList.size() != 1 ";
            }

            qstrFileName = fileList[0];
        }
        else
        {
            qstrFileName = mData.getJobListFileInfo().absoluteFilePath();
        }

        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox* pclMsgBox = new QMessageBox (QMessageBox::Information, qstrTmp, tr (C_STRING_GUI_JOBLIST_SAVED));
        pclMsgBox->removeButton(pclMsgBox->defaultButton());
        pclMsgBox->show();
        pclMsgBox->repaint();
        pclMsgBox->setFocus();

        // double save here as bugfix for sensor config to be saved
        mData.setJobListFileInfo (QFileInfo (qstrFileName));
        mData.saveFiles();

        mData.setJobListFileInfo (QFileInfo (qstrFileName));
        mData.saveFiles();

        Timer::msleep(1500);
        pclMsgBox->hide();
        delete (pclMsgBox);
    }
    else
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_NO_JOBLIST_OPEN), QMessageBox::Ok);
    }

    this->bHasUnsavedChanges = false;
}


void MainWindow::showAbout()
{
    mAboutDialog -> exec();
}

// void MainWindow::updateSettings()
// {
//     SettingsDialog settingsDialog( mData );
//     settingsDialog.setup();
//     settingsDialog.exec();
// }

void MainWindow::createNewJoblist()
{
    // SettingsDialog settingsDialog( mData );
    // settingsDialog.setup();
    // if ( settingsDialog.exec() == QDialog::Accepted )
    //     qDebug() << __PRETTY_FUNCTION__  << " create new joblist!";
}


void MainWindow::selectFirstJob()
{
    if ( not ( ui -> jobListTableView -> model() -> rowCount() > 0 ) )
        return;
    ui -> jobListTableView -> selectRow( 0 );
}

void MainWindow::selectNextJob()
{
    QItemSelectionModel* selectionModel = ui -> jobListTableView -> selectionModel();
    QModelIndexList selectionList =  selectionModel -> selectedRows();
    if ( selectionList.size() > 1 )
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    if ( selectionList.size() == 0 )
        return;
    int newRow = selectionList[0].row() + 1;
    if ( not ( newRow < ui -> jobListTableView -> model() -> rowCount() ) )
        return;

    ui -> jobListTableView -> selectRow( newRow );

}

void MainWindow::selectLastJob()
{
    if ( not ( ui -> jobListTableView -> model() -> rowCount() > 0 ) )
        return;

    int newRow = ui -> jobListTableView -> model() -> rowCount() - 1;
    ui -> jobListTableView -> selectRow( newRow );
}

void MainWindow::selectPreviousJob()
{
    QItemSelectionModel* selectionModel = ui -> jobListTableView -> selectionModel();    
    QModelIndexList selectionList =  selectionModel -> selectedRows();
    if ( selectionList.size() > 1 )
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";
    if ( selectionList.size() == 0 )
        return;
    int newRow = selectionList[0].row() - 1;
    if ( newRow < 0 )
        return;

    ui -> jobListTableView -> selectRow( newRow );
}


void MainWindow::jobSelectionChanged( const QItemSelection &  , const QItemSelection & ) 
{
    bool enabled = ui -> jobListTableView -> selectionModel() -> hasSelection() ;
    ui -> deleteJobButton -> setEnabled( enabled );
    ui -> insertJobButton -> setEnabled( enabled );

    ui -> cbCSAMTMode -> setEnabled( enabled );
    bool isInCSAMTMode = mData.getMeasurementMode() == ControlData::CSAMTMeasurementMode;
    ui -> cbCSAMTMode    -> setChecked( isInCSAMTMode );

    enableCSAMTStuff( isInCSAMTMode );

    updateStorageModeSettings ();
    
    if (isInCSAMTMode)
    {
        updateCSAMTStuff();
    }

    return;
}

void MainWindow::handleRbAdjustPauseOff( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::AdjustIsOff );
}


void MainWindow::handleRbAdjustPauseAuto( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::AdjustAuto );
}
 
void MainWindow::handleRbAdjustPause1Min( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::Adjust1Min );
}

void MainWindow::handleRbAdjustPause2Min( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::Adjust2Min );
}

void MainWindow::handleRbAdjustPause3Min( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::Adjust3Min );
}

void MainWindow::handleRbAdjustPause5Min( bool checked ) 
{
    if ( checked ) mData.setAdjustPause( ControlData::Adjust5Min );
}

void MainWindow::handleCbCSAMTMode( bool checked )
{
    mData.setMeasurementMode( checked ? ControlData::CSAMTMeasurementMode : ControlData::NormalMeasurementMode );
}

void MainWindow::handlePbCSAMTTX_f()
{
    // declaration of variables
    int     iValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Number of Stacks", this);
    if (pclKeyPad->exec() == true)
    {
        iValue = pclKeyPad->text().toInt(&bOk);
        if (bOk == true)
        {
            mData.setTXMNumStacks (QString::number (iValue));

            if (ui->cbCSAMTMode->isChecked() == true)
            {
                checkAutoAdjustPauseActive();
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
}

void MainWindow::handlePbCSAMTTX_t()
{
    // declaration of variables
    int     iValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Dipole TX Time [s]", this);
    if (pclKeyPad->exec() == true)
    {
        iValue = pclKeyPad->text().toInt(&bOk);
        if (bOk == true)
        {
            mData.setTXMDipTXTime (QString::number (iValue));

            if (ui->cbCSAMTMode->isChecked() == true)
            {
                checkAutoAdjustPauseActive();
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
}

void MainWindow::handlePbCSAMT_FSamples()
{
    // declaration of variables
    int     iValue;
    bool    bOk;
    QString qstrTmp;

    KeypadFreeTxmClass* pclKeyPad = new KeypadFreeTxmClass (true, false, "Number Of Dipoles", this);
    if (pclKeyPad->exec() == true)
    {
        iValue = pclKeyPad->text().toInt(&bOk);
        if (bOk == true)
        {
            mData.setTXMNumDipoles (QString::number (iValue));

            if (ui->cbCSAMTMode->isChecked() == true)
            {
                checkAutoAdjustPauseActive();
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
}


void MainWindow::execDateTimeDialog()
{
    // delcaration of variables
    unsigned int uiCounter;
    QString      qstrTmp;

    if (mData.jobCount() <= 0)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_JOBLIST_EMPTY), QMessageBox::Ok);
    }
    else
    {
        qstrTmp = mData.jobListStartDateTime().toString("yyyy-MM-dd hh:mm:ss");
        mDateTimeSelector->setDateTime  (mData.jobListStartDateTime());
        mDateTimeSelector->setStartMode (mData.getTimeFormat       ());

        mDateTimeSelector->setCyclicJobParameters (mData.getJobAt(0)->getCyclicEvent(),
                                                   mData.getJobAt(0)->getCyclicDuration(),
                                                   mData.getJobAt(0)->getCyclicUnit());

        mDateTimeSelector->setCleanJobTable(mData.getCleanJobTables());

        if (mDateTimeSelector->exec() == QDialog::Accepted)
        {
            QDateTime newTime = mDateTimeSelector->getDateTime();

            mData.shiftStartChanged(newTime);

            ui->shiftStartEdit->clearMaximumDateTime();
            ui->shiftStartEdit->clearMinimumDateTime();
            ui->shiftStartEdit->setDateTime(newTime);

            // set CleanJobTable flag
            mData.setCleanJobTables(mDateTimeSelector->getCleanJobTables());

            // set Time Format in job list
            mData.setTimeFormat(mDateTimeSelector->getStartMode());

            // set cyclic job parameters in job list. this is done in ALL
            // jobs
            for (uiCounter = 0; uiCounter < mData.jobCount(); uiCounter++)
            {
                mData.getJobAt(uiCounter)->setCyclicEvent    (mDateTimeSelector->getCyclicEvent    ());
                mData.getJobAt(uiCounter)->setCyclicDuration (mDateTimeSelector->getCyclicDuration ());
                mData.getJobAt(uiCounter)->setCyclicUnit     (mDateTimeSelector->getCyclicUnit     ());
            }
        }
    }
}

void MainWindow::enableCSAMTStuff( bool enable )
{
    ui -> pbCSAMTTX_f      -> setEnabled( enable );
    ui -> pbCSAMTTX_t      -> setEnabled( enable );
    ui -> pbCSAMT_FSamples -> setEnabled( enable );

    ui -> lbCSAMTTX_f      -> setEnabled( enable );
    ui -> lbCSAMTTX_t      -> setEnabled( enable );
    ui -> lbCSAMT_FSamples -> setEnabled( enable );
    
}

void MainWindow::updateCSAMTStuff()
{
    ui -> pbCSAMTTX_f      -> setText( mData.getTXMNumStacks () );
    ui -> pbCSAMTTX_t      -> setText( mData.getTXMDipTXTime () );
    ui -> pbCSAMT_FSamples -> setText( mData.getTXMNumDipoles() );
}


void MainWindow::updateStorageModeSettings (void)
{
    // storage mode settings
    if (mData.getStorageMode().compare(Types::qstrlStorageModeStrings [Types::StorageModes::WriteToInternal]) == 0)
    {
        ui->cbStorageMode->setCurrentIndex(Types::StorageModes::WriteToInternal);
        mData.setTargetDirectory(Types::qstrlTargetDirStrings [Types::StorageModes::WriteToInternal]);
    }
    else
    {
        ui->cbStorageMode->setCurrentIndex(Types::StorageModes::WriteToUSB);
        mData.setTargetDirectory(Types::qstrlTargetDirStrings [Types::StorageModes::WriteToUSB]);
    }
}


void MainWindow::updateNonQtItemModelControlledGui()
{
    QDateTime shiftStart = mData.jobListStartDateTime();
    ui -> shiftStartEdit   -> setDateTime( shiftStart );    

    bool isInCSAMTMode = mData.getMeasurementMode() == ControlData::CSAMTMeasurementMode;

    updateStorageModeSettings ();

    enableCSAMTStuff( isInCSAMTMode );

    if ( not isInCSAMTMode )
        return;

    updateCSAMTStuff();

}

Job::Spt MainWindow::createNewJob()
{
    // declaration of variables
    DataModel* pclTemplateData = NULL;

    if (this->mData.isTXM() == true)
    {
        pclTemplateData = &this->mJoblistTempl_TXM;
    }
    else
    {
        if (this->mData.isMF() == true)
        {
            pclTemplateData = &this->mJoblistTempl_MF;
        }
        else
        {
            if (this->mData.isADU08eBB() == true)
            {
                pclTemplateData = &this->mJoblistTempl_ADU08eBB;
            }
            else
            {
                if (this->mData.isADU09u10e() == true)
                {
                    pclTemplateData = &this->mJoblistTempl_ADU10e;
                }
                else
                {
                    if (this->mData.isADU11e5CHSTD() == true)
                    {
                        pclTemplateData = &this->mJoblistTempl_ADU11e_5CH_STD;
                    }
                    else
                    {
                        pclTemplateData = &this->mJoblistTempl_LF_HF;
                    }
                }
            }
        }
    }

    //SimpleJobPopup jobPopup (pclTemplateData->allowedSampleFreqs());
    SimpleJobPopup jobPopup (pclTemplateData->getAllowedSampleFreqsFromTemplate ());
    jobPopup.setFrequency          (DefaultValues::Frequency);
    jobPopup.setDurationAsInterval (DefaultValues::JobDuration);
    jobPopup.updateDisplay         ();

    if (not(jobPopup.exec() == QDialog::Accepted))
    {
        return Job::Spt();
    }

    Job::Spt newJob;
    newJob = pclTemplateData->cloneJobWithFreqNear (jobPopup.getFrequency());

    if (newJob.isNull())
    {
        newJob = Job::createFromTemplate();
    }
    
    (*newJob).setDuration   (jobPopup.getDuration ());
    (*newJob).setFrequency  (jobPopup.getFrequency());
    (*newJob).clearFileName (); // we will create one when we save...
    (*newJob).commitChanges ();
    
    pclTemplateData = NULL;

    return newJob;
}

void MainWindow::handleInsertJob()
{
    // declaration of variables

    Job::Spt newJob = createNewJob();

    if ( newJob.isNull() )
        return;

    Types::Index selectedRow = selectedJobRow();

    if ( selectedRow.isValid() )
        mData.addJobBeforeJobAtIndex( selectedRow, newJob );

    checkAutoAdjustPauseActive ();
}
    
void MainWindow::handleAppendJob()
{
    // declaration of variables

    Job::Spt newJob = createNewJob();

    if ( newJob.isNull() )
        return;

 //    Types::Index selectedRow = selectedJobRow();

//     selectedRow.isValid() ?
//         mData.addJobAfterJobAtIndex( selectedRow      , newJob ) :
//         mData.addJobAfterJobAtIndex( mData.jobCount() , newJob );

    mData.addJobAfterJobAtIndex( mData.jobCount() , newJob );
    selectLastJob();

    checkAutoAdjustPauseActive ();
}


void MainWindow::checkAutoAdjustPauseActive (void)
{
    // declaration of variables
    QString qstrTmp;
    int     iTmp;

    if (ui->rbAdjustPauseOff->isChecked() == true)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        iTmp = QMessageBox::question(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_NO_AUTO_ADJUST), QMessageBox::Yes, QMessageBox::No);
        if (iTmp == QMessageBox::Yes)
        {
            ui->rbAdjustPauseOff->setChecked  (false);
            ui->rbAdjustPause1Min->setChecked (true);
            ui->rbAdjustPause2Min->setChecked (false);
            ui->rbAdjustPause3Min->setChecked (false);
            ui->rbAdjustPause5Min->setChecked (false);
            ui->rbAdjustPauseAuto->setChecked (false);
            ui->adjustToFullMinuteCheckbox->setChecked(true);
        }
    }

    this->updateCSAMTStuff();
    this->updateNonQtItemModelControlledGui();
}


void MainWindow::handleModifyJob()
{
    goToJobConfigTab();
}

void MainWindow::handleDeleteJob()
{
    Types::Index selectedRow = selectedJobRow();

    if ( not selectedRow.isValid() )
        return;

    mData.deleteJobAtIndex( selectedRow );
}

Types::Index  MainWindow::selectedJobRow() const
{

    QItemSelectionModel* selectionModel = ui -> jobListTableView -> selectionModel();

    if ( not selectionModel -> hasSelection() )
        return Types::Index();

    QModelIndexList selectionList =  selectionModel -> selectedRows();
    if ( selectionList.size() > 1 )
        qDebug() << __PRETTY_FUNCTION__ << " WARNING more than one row selected here, will only use the first one!";

    return Types::Index( selectionList[0].row() );

}




////////////////////////////////////////////////////////////////////////////////
//
// internal stuff

void MainWindow::loadJobListFile( const QString& fileName )
{
    try
    {
        QFileInfo fileInfo( fileName );
        QDir mainDir = fileInfo.absoluteDir();

        if (not mainDir.cdUp())
        {
            throw std::runtime_error("cant cd to root of ADU-07 dirhier.");
        }

        // joblist
        mData.setJobDirectory( mainDir.canonicalPath() );
        mData.loadAndParseJobListFile( fileName );

        ui -> shiftStartEdit -> setMinimumDateTime( mData.jobListStartDateTime() );
        QAbstractItemModel* model = mData.getJobListModel();
        ui->jobListTableView->setModel          (model);
        ui->jobListTableView->setSelectionModel (mData.getSelectionModel ());
        ui->jobListTableView->resizeRowsToContents();
        ui->jobListTableView->setAlternatingRowColors(true);

        ui->jobListTableView->setColumnWidth(1, ui->jobListTableView->columnWidth(1) +20);
        ui->jobListTableView->setColumnWidth(2, ui->jobListTableView->columnWidth(2) +20);

        ui -> tabWidget -> setEnabled( true );

        // joblist control buttons
        bool enabled = ui -> jobListTableView -> selectionModel() -> hasSelection() ;
        ui -> deleteJobButton -> setEnabled( enabled );
        ui -> insertJobButton -> setEnabled( enabled );

        // hardware config and hardware database
        // FIXME: make this invisible form the outside of the datamodel!
        mData.loadAndParseHwData( fileInfo.canonicalPath() );

        QObject::connect ( mData.getSelectionModel(),
                           SIGNAL( selectionChanged ( const QItemSelection & , const QItemSelection &  ) ),
                           this ,
                           SLOT( jobSelectionChanged( const QItemSelection & , const QItemSelection & ) ) );

        // load the single jobs
        if (mData.jobCount() > 0)
        {
            Job::Spt job = mData.getJobAt( 0 );

            QString jobFileName = job->getJobFileNameAsReadFromJoblist();

            // sadly, qt does not have tools for path manipulation independent from the underlying os.
            // so we use a poor man implementation for now; the next iteration should sensible like boost filesystem
            // dont blame me for this.
            QStringList pathComponents = jobFileName.split( "/" );
            if (pathComponents.size() < 2)
            {
                mData.setJobSubdir("");
            }

            pathComponents.pop_back(); // remove filename
            QString subdir = pathComponents.last();

            if (not subdir.startsWith("/" ))
            {
                subdir.prepend("/");
            }

            mData.setJobSubdir(subdir);
        }
    } catch ( const std::runtime_error& e )
    {
        qDebug() << "Unable to load file : " << fileName << ", reason : ";
        qDebug() << e.what ();
        QErrorMessage em;
        em.showMessage( QString( e.what() ) );
        em.exec();
        mData.reset();
    }
}


bool MainWindow::loadJobListTemplates(const QString qstrTargetPath, DataModel& clData)
{
    // declaration of variables
    bool        bRetValue = true;
    Job::Spt    pclJob;
    QString     qstrJobFileName;
    QString     qstrADUConf = qstrTargetPath + "/ADUConf.xml";

    // try to load LF/HF template joblist
    try
    {
        QFileInfo clFileInfo(qstrTargetPath);
        QDir      clMainDir = clFileInfo.absoluteDir();

        // joblist
        clData.setJobDirectory         (clMainDir.canonicalPath());
        clData.loadAndParseJobListFile (qstrADUConf);

        // hardware config and hardware database
        clData.loadAndParseHwData (qstrTargetPath);

        // load the single jobs
        if (clData.jobCount() > 0)
        {
            pclJob          = clData.getJobAt (0);
            qstrJobFileName = pclJob->getJobFileNameAsReadFromJoblist();

            // sadly, qt does not have tools for path manipulation independent from the underlying os.
            // so we use a poor man implementation for now; the next iteration should sensible like boost filesystem
            // dont blame me for this.
            QStringList pathComponents = qstrJobFileName.split( "/" );
            if (pathComponents.size() < 2)
            {
                clData.setJobSubdir("");
            }

            pathComponents.pop_back(); // remove filename
            QString subdir = pathComponents.last();

            if (not subdir.startsWith("/" ))
            {
                subdir.prepend("/");
            }

            clData.setJobSubdir(subdir);
        }
    } catch (const std::runtime_error& e)
    {
        clData.reset();
        bRetValue = false;
    }

    return (bRetValue);
}


bool MainWindow::loadJobListTemplates(void)
{
    // declaration of variables
    bool    bRetValue = true;
    QString qstrTmp;

    // try to load ADU-07e 5*LF + 5*HF template joblist
    //qstrTmp = QDir::currentPath () + C_TEMPLATE_DIR_LF_HF;
    qstrTmp = C_TEMPLATE_DIR_ADU07_5LF_5HF;
    if (this->loadJobListTemplates(qstrTmp, this->mJoblistTempl_LF_HF) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(qstrTmp);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }

    /*
    // try to load MF template joblist
    if (this->loadJobListTemplates(C_TEMPLATE_DIR_MF, this->mJoblistTempl_MF) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(C_TEMPLATE_DIR_MF);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }

    // try to load TXM template joblist
    if (this->loadJobListTemplates(C_TEMPLATE_DIR_TXM, this->mJoblistTempl_TXM) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(C_TEMPLATE_DIR_TXM);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }
    */

    // try to load ADU-08e template joblist
    if (this->loadJobListTemplates(C_TEMPLATE_DIR_ADU08e_5BB, this->mJoblistTempl_ADU08eBB) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(C_TEMPLATE_DIR_ADU08e_5BB);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }

    // try to load ADU-10e template joblist
    if (this->loadJobListTemplates(C_TEMPLATE_DIR_ADU10e, this->mJoblistTempl_ADU10e) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(C_TEMPLATE_DIR_ADU10e);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }

    // try to load ADU-11e template joblist - 5-CH standard inputs
    if (this->loadJobListTemplates(C_TEMPLATE_DIR_ADU11e_5CH_STD, this->mJoblistTempl_ADU11e_5CH_STD) == false)
    {
        qstrTmp = tr(C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE).arg(C_TEMPLATE_DIR_ADU11e_5CH_STD);
        qDebug() << "[" << __PRETTY_FUNCTION__ << "] " << qstrTmp;
        QErrorMessage em;
        em.showMessage(qstrTmp);
        em.resize(600, 300);
        em.exec();
        bRetValue = false;
    }

    return (bRetValue);
}



void MainWindow::on_cbStorageMode_currentIndexChanged(int index __attribute__((unused)))
{
    mData.setStorageMode     (Types::qstrlStorageModeStrings [ui->cbStorageMode->currentIndex()]);
    mData.setTargetDirectory (Types::qstrlTargetDirStrings   [ui->cbStorageMode->currentIndex()]);
}


void MainWindow::handleMoveJob1Up (void)
{
    // declaration of variables
    QItemSelectionModel* pclSelectionModel;
    QModelIndexList      clSelectionList;
    int                  iActIndex = -1;
    QString              qstrTmp;

    // get selected row in data model
    pclSelectionModel = ui->jobListTableView->selectionModel();
    clSelectionList   = pclSelectionModel->selectedRows     ();
    if(clSelectionList.size() >= 1)
    {
        // multible rows have been selected - only using first one
        iActIndex = clSelectionList[0].row ();

        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: mutliple rows have been selected - using first one";
    }

    if (clSelectionList.size() == 0)
    {
        // no row selected - doing nothing;
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_NO_JOB_SELECTED), QMessageBox::Ok);

        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: no row selected - doing nothing";
    }

    // check, if the job is already the first one. if so, do nothing
    if (iActIndex > 0)
    {
        // shift job
        (void) this->mData.swapJobPosition (iActIndex, (iActIndex - 1));
    }
    else
    {
        // do nothing, as this is the first job, or no job is selected
        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: job is the first one, or no job selected - doing nothing:" << iActIndex;
    }

    // check, if "auto offset" is active. if not, ask to activate, or manipulate start-times manually
    if (ui->rbAdjustPauseOff->isChecked() == true)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        iActIndex = QMessageBox::question(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_NO_AUTO_ADJUST), QMessageBox::Yes, QMessageBox::No);
        if (iActIndex == QMessageBox::Yes)
        {
            ui->rbAdjustPauseOff->setChecked  (false);
            ui->rbAdjustPause1Min->setChecked (true);
            ui->rbAdjustPause2Min->setChecked (false);
            ui->rbAdjustPause3Min->setChecked (false);
            ui->rbAdjustPause5Min->setChecked (false);
            ui->rbAdjustPauseAuto->setChecked (false);
            ui->adjustToFullMinuteCheckbox->setChecked(true);
        }
    }

    ui->jobListTableView->selectRow ((iActIndex - 1));
}


void MainWindow::handleMoveJob1Down (void)
{
    // declaration of variables
    QItemSelectionModel* pclSelectionModel;
    QModelIndexList      clSelectionList;
    int                  iActIndex = -1;
    QString              qstrTmp;

    // get selected row in data model
    pclSelectionModel = ui->jobListTableView->selectionModel();
    clSelectionList   = pclSelectionModel->selectedRows     ();
    if(clSelectionList.size() >= 1)
    {
        // multible rows have been selected - only using first one
        iActIndex = clSelectionList[0].row ();

        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: mutliple rows have been selected - using first one:" << iActIndex;
    }

    if (clSelectionList.size() == 0)
    {
        // no row selected - doing nothing;
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_NO_JOB_SELECTED), QMessageBox::Ok);

        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: no row selected - doing nothing";
    }

    // check, if the job is already the last one. if so, do nothing
    if ((iActIndex < (((int)(this->mData.jobCount())) - 1)) &&
        (iActIndex >= 0))
    {
        // shift job
        (void) this->mData.swapJobPosition (iActIndex, (iActIndex + 1));
    }
    else
    {
        // do nothing, as this is the last job, or no job is selected
        qDebug () << "[" << __PRETTY_FUNCTION__ << "]: job is the last one, or no job selected - doing nothing:" << iActIndex;
    }

    // check, if "auto offset" is active. if not, ask to activate, or manipulate start-times manually
    if (ui->rbAdjustPauseOff->isChecked() == true)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        iActIndex = QMessageBox::question(this, qstrTmp, tr (C_STRING_GUI_MAINWINDOW_NO_AUTO_ADJUST), QMessageBox::Yes, QMessageBox::No);
        if (iActIndex == QMessageBox::Yes)
        {
            ui->rbAdjustPauseOff->setChecked  (false);
            ui->rbAdjustPause1Min->setChecked (true);
            ui->rbAdjustPause2Min->setChecked (false);
            ui->rbAdjustPause3Min->setChecked (false);
            ui->rbAdjustPause5Min->setChecked (false);
            ui->rbAdjustPauseAuto->setChecked (false);
            ui->adjustToFullMinuteCheckbox->setChecked(true);
        }
    }

    ui->jobListTableView->selectRow ((iActIndex + 1));
}


void MainWindow::on_actionCompare_Joblist_triggered (void)
{
    // declaration of variables
    QString   qstrCmpJobListPath;
    QString   qstrFilter;
    QString   qstrTmp;

    // read last open path from settings class
    pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
    qstrTmp = pclSettings->value(C_SETTINGS_LAST_JOBLIST_DIR).toString();
    pclSettings->endGroup();

    qstrFilter         = "ADUConf File (ADUConf.xml)";
    qstrCmpJobListPath = QFileDialog::getOpenFileName(this, C_STRING_GUI_OPEN_JOBLIST_CMP_DIALOG, qstrTmp, tr("ADUConf File (ADUConf.xml)"), &qstrFilter);
    if (qstrCmpJobListPath.isEmpty() == false)
    {
        if (pclCompareJoblists != NULL)
        {
            delete (pclCompareJoblists);
        }
        pclCompareJoblists = new CompareJobLists (&(this->mData), qstrCmpJobListPath);

        pclCompareJoblists->setModal(true);
        pclCompareJoblists->show    ();
    }
}


bool MainWindow::exportJoblist(DataModel& clJoblist, QString& qstrNewJoblistDir)
{
    // declaration of variables
    QString     qstrDestDir;
    QString     qstrSrcDir;
    QString     qstrTmp;
    QFile       clFile;
    bool        bRetValue = true;

    // read last open path from settings class
    pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
    qstrDestDir = pclSettings->value(C_SETTINGS_LAST_JOBLIST_DIR).toString();
    pclSettings->endGroup();

    // select directory to export joblist to
    qstrDestDir       = QFileDialog::getExistingDirectory(this, C_STRING_GUI_EXPORT_JOBLIST_DIALOG, qstrDestDir);
    if (qstrDestDir.size() == 0)
    {
        bRetValue = false;
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_FAILED_TO_CREATE_NEW_JOBLIST), QMessageBox::Ok);
    }
    else
    {
        qstrNewJoblistDir = qstrDestDir;

        // source directory is the one of the actual active joblist
        qstrSrcDir = clJoblist.getJobDirectory();

        // copy the contents of the ADUConf directory to the destination dir
        try
        {
            if (qstrDestDir.size() != 0)
            {
                QDir clExportDir (qstrDestDir);
                clExportDir.mkdir("ADUConf");

                qstrTmp = clJoblist.getJobSubdir();
                qstrTmp.remove('/');
                clExportDir.mkdir(qstrTmp);

                clFile.setFileName (qstrSrcDir + "/ADUConf/HwDatabase.xml");
                clFile.copy(qstrDestDir + "/ADUConf/HwDatabase.xml");
                clFile.close();

                clFile.setFileName (qstrSrcDir + "/ADUConf/HwConfig.xml");
                clFile.copy(qstrDestDir + "/ADUConf/HwConfig.xml");
                clFile.close();

                clFile.setFileName (qstrSrcDir + "/ADUConf/ADUConf.xml");
                clFile.copy(qstrDestDir + "/ADUConf/ADUConf.xml");
                clFile.close();

                // now save the joblist at the new destination
                clJoblist.setJobListFileInfo (QFileInfo (qstrDestDir + "/ADUConf/ADUConf.xml"));
                clJoblist.setJobDirectory    (qstrDestDir);
                clJoblist.saveFiles();

                // set window title
                qstrTmp  = BuildManifest::ApplicationName.c_str();
                qstrTmp += " - ";
                qstrTmp += BuildManifest::VersionString.c_str();
                qstrTmp += " - ";
                qstrTmp += qstrDestDir + "/ADUConf/ADUConf.xml";
                this->setWindowTitle(qstrTmp);

                // store new path in settings object
                pclSettings->beginGroup(C_SETTINGS_GROUP_MAINWINDOW);
                pclSettings->setValue(C_SETTINGS_LAST_JOBLIST_DIR, qstrDestDir + "/ADUConf/ADUConf.xml");
                pclSettings->endGroup();
            }
        }
        catch (std::runtime_error e)
        {
            bRetValue = false;
        }

        if (bRetValue == true)
        {
            // save Joblist before exporting
            if (this->mData.getIsReady() == true)
            {
                this->saveFiles (false);
            }
        }
    }

    return (bRetValue);
}


void MainWindow::on_actionExport_Joblist_triggered (void)
{
    // declaration of variables
    QString qstrTmp;

    // read last open path from settings class
    (void) this->exportJoblist(this->mData, qstrTmp);
}


void MainWindow::on_jobListTableView_doubleClicked (const QModelIndex& index)
{
    // declaration of variables
    QStringList qstrlValidSampleFreqs;
    QString     qstrDigFilType;
    double      dDigFilDecRate = 1.0;
    double      dTmp;
    ssize_t     sCount;

    if (index.column() == 0)
    {
        qstrlValidSampleFreqs = this->mData.allowedSampleFreqs(selectedJobRow());

        // check, if this is a digital filter job. if so, the single sample frequencies must be
        // divided down by the filter factor
        this->mData.frequnecyOfJob(selectedJobRow(), qstrDigFilType, dDigFilDecRate);
        if (qstrDigFilType.indexOf("mtx") != -1)
        {
            for (sCount = 0; sCount < qstrlValidSampleFreqs.size(); sCount++)
            {
                dTmp  = qstrlValidSampleFreqs [sCount].toDouble ();
                dTmp /= dDigFilDecRate;
                qstrlValidSampleFreqs [sCount] = QString::number ((unsigned int) dTmp);
            }
        }

        FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidSampleFreqs, "Select Frequency [Hz]");
        pclKeyPad->exec();

        if (pclKeyPad->bValueSelected == true)
        {
            ui->jobListTableView->model()->setData(index, pclKeyPad->qstrRetValue.toUInt(), Qt::EditRole);
        }

        delete (pclKeyPad);
    }
    else if (index.column() == 5)
    {
        ui->jobListTableView->model()->setData(index, 1, Qt::EditRole);
    }
}


void MainWindow::keyReleaseEvent (QKeyEvent* pclEvent)
{
    // declaration of variables
    bool bKeyEventHandled = false;

    // delete selected job
    switch (pclEvent->key())
    {
        case Qt::Key_Delete:
            {
                if (ui->jobListTableView->hasFocus() == true)
                {
                    handleDeleteJob ();
                }
                bKeyEventHandled = true;
                break;
            }

        case Qt::Key_Control:
            {
                this->bCtrlKeyPressed = false;
                bKeyEventHandled      = true;
                break;
            }

        case Qt::Key_S:
            {
                if (this->bCtrlKeyPressed == true)
                {
                    this->saveFiles(false);
                    this->bCtrlKeyPressed = false;
                }
                bKeyEventHandled = true;
                break;
            }
    }

    if (bKeyEventHandled == false)
    {
        QWidget::keyReleaseEvent(pclEvent);
    }
}


void MainWindow::keyPressEvent (QKeyEvent* pclEvent)
{
    // declaration of variables
    bool bKeyEventHandled = false;

    // delete selected job
    if (pclEvent->key() == Qt::Key_Control)
    {
        this->bCtrlKeyPressed = true;
        bKeyEventHandled      = true;
    }

    if (bKeyEventHandled == false)
    {
        QWidget::keyReleaseEvent(pclEvent);
    }
}


void MainWindow::addTooTips (void)
{
    ui->pbMoveJob1Up->setToolTip        (tr (C_STRING_TT_MAINWINDOW_MOVE_1_UP));
    ui->pbMoveJob1Down->setToolTip      (tr (C_STRING_TT_MAINWINDOW_MOVE_1_DOWN));
    ui->atBeginButton->setToolTip       (tr (C_STRING_TT_MAINWINDOW_SELECT_FIRST));
    ui->atEndButton->setToolTip         (tr (C_STRING_TT_MAINWINDOW_SELECT_LAST));
    ui->oneBackButton->setToolTip       (tr (C_STRING_TT_MAINWINDOW_SELECT_PREV));
    ui->oneForewardButton->setToolTip   (tr (C_STRING_TT_MAINWINDOW_SELECT_NEXT));

    ui->appendJobButton->setToolTip     (tr (C_STRING_TT_MAINWINDOW_APPEND));
    ui->deleteJobButton->setToolTip     (tr (C_STRING_TT_MAINWINDOW_DELETE));
    ui->insertJobButton->setToolTip     (tr (C_STRING_TT_MAINWINDOW_INSERT));

    ui->gpAdjustPause->setToolTip       (tr (C_STRING_TT_MAINWINDOW_ADJUST_PAUSE));
    ui->adjustToFullMinuteCheckbox->setToolTip(tr(C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_FULL_MIN));
    ui->rbAdjustPauseOff->setToolTip    (tr(C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_OFF));
    ui->rbAdjustPauseAuto->setToolTip   (tr(C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_AUTO));

    ui->gbCSAMT->setToolTip             (tr(C_STRING_TT_MAINWINDOW_CSAMT_MODE));
}


void MainWindow::on_actionNewJoblist_triggered (void)
{
    // declaration of variables
    QStringList  qstrlValidGains;
    QString      qstrNewJoblistDir;
    QString      qstrTmp;
    bool         bRetValue = true;

    qstrlValidGains.clear();
    //qstrlValidGains << C_TEMPLATE_NAME_LF_HF << C_TEMPLATE_NAME_MF << C_TEMPLATE_NAME_TXM << C_TEMPLATE_NAME_ADU08e;
    qstrlValidGains << C_TEMPLATE_NAME_ADU07_5LF_5HF << C_TEMPLATE_NAME_ADU08e_5BB << C_TEMPLATE_NAME_ADU10e << C_TEMPLATE_NAME_ADU11e_5CH_STD;

    FastButtonSelectPad* pclKeyPad = new FastButtonSelectPad (qstrlValidGains, C_STRING_GUI_CHOOSE_JOBLIST_TYPE, this, false);
    pclKeyPad->exec();

    if (pclKeyPad->bValueSelected == true)
    {
        try
        {
            if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_ADU07_5LF_5HF) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_LF_HF, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create LF/HF joblist";
                    bRetValue = false;
                }
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_MF) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_MF, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create MF joblist";
                    bRetValue = false;
                }
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_TXM) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_TXM, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create TXM joblist";
                    bRetValue = false;
                }
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_ADU08e_5BB) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_ADU08eBB, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create ADU-08e joblist";
                    bRetValue = false;
                }
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_ADU10e) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_ADU10e, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create ADU-10e joblist";
                    bRetValue = false;
                }
            }
            else if (pclKeyPad->qstrRetValue.indexOf(C_TEMPLATE_NAME_ADU11e_5CH_STD) != -1)
            {
                if (this->exportJoblist(this->mJoblistTempl_ADU11e_5CH_STD, qstrNewJoblistDir) == false)
                {
                    qDebug () << "[" << __PRETTY_FUNCTION__ << "] failed to create ADU-11e 5-CH STD joblist";
                    bRetValue = false;
                }
            }
            else
            {
                bRetValue = false;
            }
        }
        catch (...)
        {
            bRetValue = false;
        }

        if (bRetValue == true)
        {
            // load exported joblist
            qstrTmp = qstrNewJoblistDir + "/ADUConf/ADUConf.xml";
            loadJobListFile (qstrTmp);

            // delete all jobs from joblist to start with empty joblist
            while (this->mData.jobCount() > 0)
            {
                this->mData.deleteJobAtIndex(0);
            }

            // remove all XML job file from joblist directory
            qstrTmp = qstrNewJoblistDir + this->mData.getJobSubdir();
            QDir clJoblistDir (qstrTmp);
            clJoblistDir.setFilter (QDir::NoDotAndDotDot | QDir::Files);
            foreach (QString qstrDirItem, clJoblistDir.entryList())
            {
                clJoblistDir.remove (qstrDirItem);
            }
        }
    }

    delete (pclKeyPad);

    if (bRetValue == true)
    {
        this->chooseJoblistMode(true);
    }

    if (bRetValue == false)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_FAILED_TO_CREATE_NEW_JOBLIST), QMessageBox::Ok);
    }
}


void MainWindow::on_actionChooseJoblistMode_triggered (void)
{
    QString qstrTmp;

    try
    {
        this->chooseJoblistMode        (true);
        this->updateCSAMTStuff         ();
        this->updateStorageModeSettings();
    } catch (...)
    {
        qstrTmp  = BuildManifest::ApplicationName.c_str();
        qstrTmp += " - ";
        qstrTmp += BuildManifest::VersionString.c_str();
        QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_NO_JOBLIST_OPEN), QMessageBox::Ok);
    }
}


void MainWindow::on_actionSave_triggered (void)
{
    this->saveFiles(false);
}


void MainWindow::on_actionSaveAs_triggered (void)
{
    this->saveFiles(true);
}


void MainWindow::handleJobDataChanged (void)
{
    this->bHasUnsavedChanges = true;
}


void MainWindow::on_actionUTCLocalTime_triggered (void)
{
    // declaration of variables
    DialDialog* pclDial = NULL;
    QString     qstrTmp;

    pclDial = new DialDialog (12.0, -12.0, 0.5, this->mData.getUTCLocalTimeOffset(), "Choose UTC to LocalTime Offset [hours]", " hours", this);
    pclDial->setModal(true);
    pclDial->exec    ();

    // update joblist
    if (pclDial->bRetValueValid == true)
    {
        if (this->mData.getIsReady() == true)
        {
            this->mData.setUTCLocalTimeOffset(pclDial->dValue);
        }
        else
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_NO_JOBLIST_OPEN), QMessageBox::Ok);
        }
    }

    delete (pclDial);

}


void MainWindow::on_actionOpen_From_Joblist_Template_triggered  (void)
{
    // declaration of variables
    QDir*       pclDir = NULL;
    QStringList qstrlSubDirs;
    QStringList qstrJoblistDirs;
    int         iCounter;
    QString     qstrTmp;

    pclDir       = new QDir ("./jle");
    qstrlSubDirs = pclDir->entryList (QDir::Dirs);

    // parse through all subdirs and check, if they contain "ADUConf/ADUConf.xml" file and
    // are NOT called "System"
    qstrJoblistDirs.clear();
    for (iCounter = 0; iCounter < qstrlSubDirs.size(); iCounter++)
    {
        qstrTmp  = "./";
        qstrTmp += qstrlSubDirs [iCounter];
        qstrTmp += "/ADUConf/ADUConf.xml";
        if ((pclDir->exists(qstrTmp) == true) && (qstrlSubDirs [iCounter].contains ("System") == false))
        {
            qstrJoblistDirs.append(qstrlSubDirs [iCounter]);
        }
    }

    FastButtonSelectPad* pclSelect = new FastButtonSelectPad (qstrJoblistDirs, "Select Joblist Template", this);
    pclSelect->exec();

    if (pclSelect->bValueSelected == true)
    {
        qstrTmp = pclSelect->qstrRetValue;

        // load template joblist
        qstrTmp  = "./jle/" + qstrTmp + "/";
        qstrTmp += "/ADUConf/ADUConf.xml";
        this->loadJobListFile (qstrTmp);

        // and directly export the joblist into a new directory
        if (this->exportJoblist(this->mData, qstrTmp) == false)
        {
            qstrTmp  = BuildManifest::ApplicationName.c_str();
            qstrTmp += " - ";
            qstrTmp += BuildManifest::VersionString.c_str();
            QMessageBox::warning(this, qstrTmp, tr (C_STRING_GUI_FAILED_OPEN_TEMPLATE), QMessageBox::Ok);
        }
    }
    delete (pclSelect);
    delete (pclDir);
}






