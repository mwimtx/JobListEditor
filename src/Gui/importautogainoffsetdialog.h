#ifndef IMPORTAUTOGAINOFFSETDIALOG_H
#define IMPORTAUTOGAINOFFSETDIALOG_H

#include "datamodel.h"
#include "codemapper.h"

#include <QDialog>
#include <QSignalMapper>
#include <QGroupBox>


namespace Ui {
    class ImportAutoGainOffsetDialog;
}


/**
 * This struct us used to store the information of the ADU07ChannelConfig
 * XML file for 1 single board.
 */
typedef struct
{
    // identifiers
    QString      qstrBoardType;         ///< ADB board type
    QString      qstrFilterType;        ///< used filter (LF_LP_4Hz, HF_HP_1Hz, ...) - NOT RADIO FILTER
    QString      qstrMode;              ///< mode (only used for MF boards)
    unsigned int uiChannelNumber;       ///< channel number
    unsigned int uiInput;               ///< used input - 0: standard inputs 1: multi purpose

    // suggested values
    QString      qstrRFFilter;          ///< suggested radio filter type
    unsigned int uiGainStage1;          ///< suggested gain for GainStage1
    unsigned int uiGainStage2;          ///< suggested gain for GainStage2
    int          iDCOffsetCorr;         ///< suggested DC offset correction value in mV

    // GUI Elements
    unsigned int uiSignalMapperIndex;   ///<index for GUI Signal Mapper

    QPushButton* pbGainStage1;          ///< push button to change GainStage1 value
    QPushButton* pbGainStage2;          ///< push button to change GainStage2 value
    QPushButton* pbRFFilter;            ///< push button to change Radio Filter value
    QPushButton* pbDCOffsetCorr;        ///< push button to change DC offset correction value

    QGroupBox*   pbGroupBox;            ///< pointer to group box that finally contains all buttons, ...
} sADU07ChannelConfigEntry_t;


/**
 * This struct us used to store the information for one complete set of
 * channels for a AutoGain / AutoOffset configuration.
 */
typedef struct
{
    bool         bApplied;              ///< values have been applied to joblist
    bool         bSkipped;              ///< values have been skipped - should not be applied to joblist
    QPushButton* pbApply;               ///< apply values to joblist
    QPushButton* pbSkip;                ///< skip this entry - do not apply to joblist
    QPushButton* pbGoBack;              ///< go back to last entry
    QGroupBox*   pclGroupBox;           ///< box to gorup the buttons for this entry

} sAutoGainOffsetConfig_t;


/**
 * This struct us used to store the information of the ADU07ChannelConfig
 * XML file.
 */
typedef struct
{
    // identifiers
    QDateTime    clCalDateTime;         ///< date / time the AutoGain / AutoOffset jobs have been executed
    unsigned int uiLongitude;           ///< GPS position longitude of the site the AutoGain / AutoOffset jobs have been executed
    unsigned int uiLatitude;            ///< GPS position latitude of the site the AutoGain / AutoOffset jobs have been executed

    // channel values
    QVector <sADU07ChannelConfigEntry_t> qvecChannelValues; ///< values of the single channels
} sADU07ChannelConfig_t;



/**
 * \brief This is the "ImportAutoGainOffset" class.
 *
 * This class implements the "Import AutoGain / AutoOffset dialog". This is used
 * to import the "ADU07ChannelConfig.xml" file, that contains the AutoGain / AutoOffset
 * values, that have been detected by ADU.
 * After import of the file the user may edit the AutoGain / AutoOffset values, before they
 * finally are applied to the single Joblist jobs.
 * Additionally the user may set the "Locked" attribute for those jobs, that shall not
 * be updated with AutoGain / AutoOffset values.
 *
 * \date 2014-03-10
 * \author MWI
 */
class ImportAutoGainOffsetDialog : public QDialog
{
    Q_OBJECT

    public:

        /**
         * \brief This is the constructor.
         *
         * This is the class constructor that gets the pointer to the joblist
         * as parameter. It will set-up the Joblist view, as well as start
         * parsing the ADU07ChannelConfig XML file to provide the information
         * inside the input elements for edit.
         *
         * @param[in] DataModel* mDataRef = pointer to DataModel that
         * @param[in] const QString& qstrPathToXMLRef = path to ADU07ChannelConfig XML file
         * @param[in] QWidget* parent = 0 = pointer to parent widget
         *
         * \author MWI
         * \date 2013-11-23
         */
        explicit ImportAutoGainOffsetDialog (DataModel* mDataRef, const QString& qstrPathToXMLRef, QWidget* parent = 0);

        /**
         * \brief This is the destructor.
         *
         * This is the class destructor it will clean up al allocated resources.
         *
         * \author MWI
         * \date 2013-11-23
         */
        ~ImportAutoGainOffsetDialog();


    private slots:

        /**
         * \brief This function is called, if the "Scroll To Begin" button is clicked.
         *
         * It will set the selected job to the first one inside the list.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_atBeginButton_clicked ();

        /**
         * \brief This function is called, if the "Scroll To End" button is clicked.
         *
         * It will set the selected job to the last one inside the list.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_atEndButton_clicked ();

        /**
         * \brief This function is called, if the "Select Previous Job" button is clicked.
         *
         * It will set the selected job to previous one inside the list.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_oneBackButton_clicked ();

        /**
         * \brief This function is called, if the "Select Next Job" button is clicked.
         *
         * It will set the selected job to next one inside the list.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_oneForewardButton_clicked ();

        /**
         * \brief This function handles double clicks on the Joblist Table.
         *
         * It will ignore all clicks, execpt on the "Locked" field. Double Clicking
         * on the "Locked" field will toggle the "Locked" flag.
         *
         * @param [in] const QModelIndex& index = index of clicked cell inside Joblist Table
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_jobListTableView_doubleClicked (const QModelIndex & index);

        /**
         * \brief This function handles clicks on the "Apply AutoGain / AutoOffset" values.
         *
         * It will go through the list of jobs and update all the gain, DC offset and RF filter
         * settings of those jobs, that are not "Locked" and fit to the actual configuration.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_pbApply_clicked ( int);

        /**
         * \brief This function handles clicks on the "Cancel" button.
         *
         * closes the dialog
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_pbCancel_clicked ();

        /**
         * \brief This function handles clicks on the "Skip" button.
         *
         * This will make the window fo to the next configuration to be displayed, or
         * close the window, if the actual one is already the last one.
         * No changes will be done to the joblist for the actual configuration.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_pbSkip_clicked (int iIndex);

        /**
         * \brief This function handles clicks on the "Back" button.
         *
         * This will make the last configuration to be displayed again.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void on_pbGoBack_clicked (int iIndex);


    public slots:

        /**
         * \brief handle signal "JobDataLoaded" from DataModel class.
         *
         * not wired up yet, only used in constructor call.
         *
         * \author MWI
         * \date 2014-03-10
         */
        void handleJobDataLoaded();

        /**
         * \brief handle signal "JobDataChanged" from DataModel class.
         *
         * not used yet
         *
         * \author MWI
         * \date 2014-03-10
         */
        void handleJobDataChanged();

        /**
         * \brief handle signal "JobSelectionChanged" from DataModel class.
         *
         * not used yet
         *
         * \author MWI
         * \date 2014-03-10
         */
        void jobSelectionChanged (const QItemSelection&, const QItemSelection&);

        /**
         * \brief Handle click on GainStage1 buttons.
         *
         * This function is called whenever the "GainStage1" button is clicked
         * for one of the ADB configurations.
         * The according ADB board and configuration is defined by the index, that
         * is handed over as parameters.
         *
         * @param[in] int = index of ADB board and configuration
         *
         * \author MWI
         * \date 2014-03-11
         */
        void on_pbGainStage1_clicked (int);

        /**
         * \brief Handle click on GainStage2 buttons.
         *
         * see on_pbGainStage1_clicked (...)
         *
         * @param[in] int = index of ADB board and configuration
         *
         * \author MWI
         * \date 2014-03-11
         */
        void on_pbGainStage2_clicked (int);

        /**
         * \brief Handle click on DC Offset Correction buttons.
         *
         * see on_pbGainStage1_clicked (...)
         *
         * @param[in] int = index of ADB board and configuration
         *
         * \author MWI
         * \date 2014-03-11
         */
        void on_pbDCOffsetCorr_clicked (int);

        /**
         * \brief Handle click on Radio Filter buttons.
         *
         * see on_pbGainStage1_clicked (...)
         *
         * @param[in] int = index of ADB board and configuration
         *
         * \author MWI
         * \date 2014-03-11
         */
        void on_pbRFFilter_clicked (int);


    private:

        /**
         * This pointer contains the DataModel with all the Joblist informations.
         */
        DataModel* mData;

        /**
         * Path to ADU07ChannelConfig XML file.
         */
        QString qstrPathToXML;

        /**
         * This structure contains the parsed values of the ADU07ChannelConfig XML file.
         */
        sADU07ChannelConfig_t sADU07ChannelConfig;

        /**
         * This signal mapper handles the signals for the single buttons.
         */
        QSignalMapper qsmapGainStage1;      ///< buttons to edit GainStage1
        QSignalMapper qsmapGainStage2;      ///< buttons to edit GainStage2
        QSignalMapper qsmapDCOffsetCorr;    ///< buttons to edit DC offset correction
        QSignalMapper qsmapRFFilter;        ///< buttons to edit radio filter
        QSignalMapper qsmapApply;           ///< buttons to apply values to joblist
        QSignalMapper qsmapSkip;            ///< buttons to skip this entry - do not apply to joblist
        QSignalMapper qsmapGoBack;          ///< buttons to go to last entry

        /**
         * This mapper is used to convert the RF Filter names (LF_RF_X) to the
         * according resistance values.
         */
        Tools::CodeMapper clRFFilterMapper;

        /**
         * Link to GUI (widget)
         */
        Ui::ImportAutoGainOffsetDialog *ui;

        /**
         * This vectore contains the index values of those AutoGain / AutoOffset values
         * that shall be shown on the GUI, hence, must be edited by the user.
         */
        QStringList                             qstrlEntriesToBeShown;  ///< contains the headings, e.g. "ADU-07-LF - Std. Inputs ..."
        QVector<sAutoGainOffsetConfig_t>        qvecEntriesToBeShown;   ///< contains the GUI elements that belong to the configuration
        QMultiMap<unsigned int, unsigned int>   qmapEntriesToBeShown;   ///< contains the links to the entries inside the AutoGain / AutoOffset vector that belong to this configuration

        /**
         * \brief This function will parse the ADU07ChannelConfig file.
         *
         * This function will try to open the ADU07ChannelConfig file at qstrPathToXML and parse
         * the contents to read the suggested values into the sADU07ChannelConfig structure.
         *
         * @param[in] const QString& qstrSrcFile = path to surce XML file
         * @param[out] const bool = true: OK / false: failed to find / parse file
         *
         * \author MWI
         * \date 2014-03-10
         */
        bool parseADU07ChannelConfigXml (const QString& qstrSrcFile);

        /**
         * \brief This function will add GUI elements for all parsed AutoGain / AutoOffset values.
         *
         * This function will use the values of the sADU07ChannelConfig structure to create
         * GUI elements for all the gains, radio filter and DC offset correction values.
         *
         * @param[in] sADU07ChannelConfig_t& sChConfig = structure with AutoGain / AutoOffset values
         * @param[out] const bool = true: OK / false: failed to create GUI elements
         *
         * \author MWI
         * \date 2014-03-10
         */
        bool setupGUI (sADU07ChannelConfig_t& sChConfig);
};

#endif // IMPORTAUTOGAINOFFSETDIALOG_H
