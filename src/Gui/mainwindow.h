#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>
#include <QMimeData>

#include "datamodel.h"
#include "datetimeselector.h"
#include "comparejoblists.h"


#define SVN_REVISION_STRING  "$Revision: 145 $"

// constants and defines

/**
 * defines the sub directory the ADU-07e with 5*LF and 5*HF
 */
#define C_TEMPLATE_DIR_ADU07_5LF_5HF    "./jle/System/Templates/Template_ADU-07e_5LF_5HF/ADUConf"
#define C_TEMPLATE_NAME_ADU07_5LF_5HF   " ADU-07e "


/**
 * defines the sub directory the MF mode Joblist template is
 * located.
 */
#define C_TEMPLATE_DIR_MF       "./jle/System/Templates/Template_MF/ADUConf"
#define C_TEMPLATE_NAME_MF      " ADU-07e \n10 * MF"


/**
 * defines the sub directory the TXM mode Joblist template is
 * located.
 */
#define C_TEMPLATE_DIR_TXM      "./jle/System/Templates/Template_TXM/ADUConf"
#define C_TEMPLATE_NAME_TXM     " TXM-22 "

/**
 * defines the sub directory the ADU-08e mode Joblist template is
 * located.
 */
#define C_TEMPLATE_DIR_ADU08e_5BB      "./jle/System/Templates/Template_ADU-08e_5BB/ADUConf"
#define C_TEMPLATE_NAME_ADU08e_5BB     " ADU-08e "


/**
 * defines the sub directory the ADU-10e mode Joblist template is
 * located.
 */
#define C_TEMPLATE_DIR_ADU10e      "./jle/System/Templates/Template_ADU-10e/ADUConf"
#define C_TEMPLATE_NAME_ADU10e     " ADU-10e "


namespace Ui {
    class MainWindow;
    class AboutDialog;
    class DateTimeSelector;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
        
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setAndLoadJobListFile( const QString& jobfileName );

    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
public slots:

    ////////////////////////////////////////////////////////////////////////////////
    //
    // actions

    void openFile();
    void saveFiles(const bool bShowFileDialog);
    void showAbout();
    // void updateSettings();
    void createNewJoblist();

    ////////////////////////////////////////////////////////////////////////////////
    // 
    // this is the job tab --> move this into a widget of its own.
    void selectFirstJob();
    void selectNextJob();
    void selectLastJob();
    void selectPreviousJob();

    void handleRbAdjustPauseOff ( bool );
    void handleRbAdjustPauseAuto( bool );
    void handleRbAdjustPause1Min( bool );
    void handleRbAdjustPause2Min( bool );
    void handleRbAdjustPause3Min( bool );
    void handleRbAdjustPause5Min( bool );

    void execDateTimeDialog();

    void handleCbCSAMTMode( bool checked );
    void handlePbCSAMTTX_f();
    void handlePbCSAMTTX_t();
    void handlePbCSAMT_FSamples();

    // wtf... i think, i should think about the names...
    void updateNonQtItemModelControlledGui();

    void handleInsertJob();
    void handleAppendJob();
    void handleModifyJob();
    void handleDeleteJob();

    /**
     * \brief handle click on "Compare Joblists" menu entry.
     *
     * This function is used to handle the action that is executed if the user
     * clicks on the "Compare Joblists" menu entry.
     * It will open the "CompareJoblists" dialog.
     *
     * \date 2014-01-29
     * \author MWI
     */
    void on_actionCompare_Joblist_triggered (void);

    /**
     * \brief handle click on "Export Joblist" menu entry.
     *
     * This function is used to handle the action that is executed if the user
     * clicks on the "Export Joblists" menu entry. It will export the actually used
     * joblist to another directory.
     * This is done by copying the source joblist to the destination path and aferwards
     * saving the Joblist into that directory.
     *
     * \date 2014-01-30
     * \author MWI
     */
    void on_actionExport_Joblist_triggered (void);

    /**
     * \brief handle click on "New" menu entry.
     *
     * This function is used to handle any clicks on the "New" menu entry inside the "File"
     * menu. It will open a dialogue to let the user choose the base template for the new
     * Joblist. Up to now the following templates are available:<br>
     * <br>
     * 1.) ADU-07e system with LF/HF Board<br>
     * 2.) TXM-22 system<br>
     * <br>
     * After choosing the Joblist template, the according template will be exported to a
     * new path as base for the new joblist.
     *
     * \date 2014-07-17
     * \author MWI
     */
    void on_actionNewJoblist_triggered (void);

    /**
     * \brief handle click on "Choose Joblist Mode" menu entry.
     *
     * This function handles clicks on the Choose Joblist Mode menu entry.
     * It will show a small dialogue to let the user switch between JobList
     * modes (MT, CSMT, ...).
     * Depending on the selected mode the TXM input tabs will be shown / hidden.
     *
     * \date 2014-07-17
     * \author MWI
     */
    void on_actionChooseJoblistMode_triggered (void);

    /**
     * \brief handle click on "Save" menu entry.
     *
     * Saves Joblist at actual path.
     *
     * \date 2014-07-17
     * \author MWI
     */
    void on_actionSave_triggered (void);

    /**
     * \brief handle click on "Save As" menu entry.
     *
     * Saves Joblist on path entered by user on Dialog.
     *
     * \date 2014-07-17
     * \author MWI
     */
    void on_actionSaveAs_triggered (void);

    /**
     * \brief This function is used to move the selected job 1 up.
     *
     * This function will move the selected job one position up. If it is already
     * the first one, or no job is selected, nothing will be done.
     * Additonally the "Time Adjust" mode will be set to "1 Minute", if beeing set to
     * "Off" as the joblist start-/stop-times need to be reconfigured.
     *
     * \date 2014-01-22
     * \author MWI
     */
    void handleMoveJob1Up   (void);

    /**
     * \brief This function is used to move the selected job 1 down.
     *
     * see handleMoveJob1Up (...)
     *
     * \date 2014-01-22
     * \author MWI
     */
    void handleMoveJob1Down (void);

    ////////////////////////////////////////////////////////////////////////////////
    //
    // mainwindow stuff.

    void goToJobTab();
    void goToSensorsTab();
    void goToJobConfigTab();

private slots:

    void jobSelectionChanged( const QItemSelection & , const QItemSelection & ); 

    void on_cbStorageMode_currentIndexChanged(int index);

    void on_jobListTableView_doubleClicked ( const QModelIndex & index );

    void handleJobDataChanged (void);

    /**
     * \brief This function handles clicks on the "Choose UTC To Localtime" menu entry.
     *
     * Opens an instance of DialDialog to let the user enter a new value for the
     * UTC to Localtime Offset. Afterwards the start-times of all jobs must be updated
     * according to new UTC to Localtime offset, so that inside the JobListEditor all
     * times are shown in Localtime, but inside the Joblist itself all Times are
     * UTC. The times shown in JobListEditor shall stay absolute.
     *
     * \date 2014-01-22
     * \author MWI
     */
    void on_actionUTCLocalTime_triggered (void);

    /**
     * \brief This function handles clicks on the "Open From Joblist Template" menu entry.
     *
     * This will scan for all directories inside the active working directory that contain
     * joblist files (hence, that contain a "./ADUConf/ADUConf.xml" subdirectory / file.
     * All these directories will be displayed inisde a fast button select dialog.
     * If the user clicks on one of this buttons, the according joblist will be exported
     * and set as active joblist.
     * Note that the "System" directory is not displayed as this one contains the templates,
     * that are used to create new jobs.
     *
     * \date 2014-08-04
     * \author MWI
     */
    void on_actionOpen_From_Joblist_Template_triggered  (void);


signals:
    
    // void fileIsLoaded  ( bool );
    // void jobIsSelected ( bool );

protected:

    virtual void closeEvent(QCloseEvent *);

    /**
     * \brief This function handles all keypress / release events on the JobListEditor.
     *
     * Up to now only the following key events are handles:<br>
     * <br>
     * CTRL+S: save joblist<br>
     * Delete: delete selected job, if Joblist Tableview is in focus<br>
     *
     * @param[in] QKeyEvent* pclEvent = Event class instance
     *
     * \date 2014-01-22
     * \author MWI
     */
    void keyReleaseEvent (QKeyEvent* pclEvent);

    /**
     * \brief This function handles all keypress / release events on the JobListEditor.
     *
     * --> see keyReleaseEvent (...)
     *
     * @param[in] QKeyEvent* pclEvent = Event class instance
     *
     * \date 2014-01-22
     * \author MWI
     */
    void keyPressEvent   (QKeyEvent* pclEvent);


private:

    void setupActions();
    void setupSignals() ;
    void setupViews();
    void wireupDataModel();

    void enableCSAMTStuff( bool );
    void updateCSAMTStuff();

    void updateStorageModeSettings (void);

    void loadJobListFile(const QString&);


    /**
     * \brief This function is used to load the template Joblists.
     *
     * It will create the LF/HF  and MF board Joblist templates from the
     * contents of the template subdirectory.<br>
     * <br>
     * 1.) working dir\Templates\Template_LF_HF<br>
     *     -->contains template Joblist for LF/HF board ADUs<br>
     * <br>
     * 2.) working dir\Templates\Template_MF<br>
     *     -->contains template Joblist for MF board ADUs<br>
     * <br>
     * The Template Joblists contain Jobs for all frequencies, including DigitalFilter
     * Jobs and are used as master jobs, if new jobs shall be appended / inserted into
     * the joblist.
     *
     * @param[out] const bool = true: all templates could be loaded / false: at least 1 template could not be loaded
     *
     * \date 2014-07-16
     * \author MWI
     */
    bool loadJobListTemplates (void);
    bool loadJobListTemplates (const QString qstrTargetPath, DataModel& clData);


    /**
     * \brief This function is used to export a joblist to a new directory.
     *
     * This function will export the joblist, that is received as parameter into a new
     * directory. Afterwards the working dir will directly be switched to the new
     * directory.
     *
     * @param[in] DataModel& clJoblist = reference to DataModel class instances that contains Joblist to be exported
     * @param[in] QString& qstrNewJoblistDir = reference to string the new joblist directory will be returned in
     * @param[out] const bool = true: all templates could be loaded / false: at least 1 template could not be loaded
     *
     * \date 2014-07-17
     * \author MWI
     */
    bool exportJoblist (DataModel& clJoblist, QString& qstrNewJoblistDir);


    /**
     * \brief Lets the user select the Joblist mode.
     *
     * This function will show a small dialogue to let the user choose the mode
     * of this joblist (MT / CSMT).
     *
     * @param[in] const bool bShowSelectionDialog = true: show dialog / false: just update Mainwindow GUI
     * @param[out] const bool = true: OK / false: NOK
     *
     * \date 2014-07-17
     * \author MWI
     */
    bool chooseJoblistMode (const bool bShowSelectionDialog);


    Types::Index selectedJobRow() const;


    /**
     * \brief This function is used to create a new jobs to be appended / inserted to the Joblist.
     *
     * Depending on the actually used Hardware Configuration, either the
     *
     * @param[out] const bool = true: all templates could be loaded / false: at least 1 template could not be loaded
     *
     * \date 2014-07-16
     * \author MWI
     */
    Job::Spt createNewJob();

    void readSettings();
    void writeSettings();


    /**
     * \brief This function will add ToolTips to the single input elements.
     *
     *-
     *
     */
    void addTooTips (void);

    /**
     * \brief This function checks, if the automatic pause option is active.
     *
     * It will check, if the "Adjust Pause" function is active. If not, it will
     * ask the user, if it should be activated.
     *
     */
    void checkAutoAdjustPauseActive (void);


    /**
     * pointer to Compare Joblists window.
     */
    CompareJobLists* pclCompareJoblists;

    /**
     * Data Model contains the Joblist to be edited.
     */
    DataModel mData;

    /**
     * Data Model contains the Template Joblist for LF / HF board ADUs.
     */
    DataModel mJoblistTempl_LF_HF;

    /**
     * Data Model contains the Template Joblist for MF board ADUs.
     */
    DataModel mJoblistTempl_MF;

    /**
     * Data Model contains the Template Joblist for TXM board ADUs.
     */
    DataModel mJoblistTempl_TXM;

    /**
     * Data Model contains the Template Joblist for ADU-08e board ADUs.
     */
    DataModel mJoblistTempl_ADU08eBB;

    /**
     * Data Model contains the Template Joblist for ADU-10e board ADUs.
     */
    DataModel mJoblistTempl_ADU10e;

    /**
     * This flag is used to detect, if the CTRL key is pressed.
     */
    bool bCtrlKeyPressed;

    /**
     * This flag is true, if the Joblist contains unsaved changes.
     */
    bool bHasUnsavedChanges;


    Ui::MainWindow       *ui;
    QDialog              *mAboutDialog;
    Ui::AboutDialog      *ui_AboutDialog;
    DateTimeSelector     *mDateTimeSelector;
    Ui::DateTimeSelector *ui_DateTimeSelector;

};

#endif // MAINWINDOW_H
