/**
 * \file Gui/Strings.h
 *
 * \brief This file contains all strings, that are used inside the project and GUI interface.
 *
 * -
 *
 * \author MWI
 * \date 2013-10-22
 */

#ifndef STRINGS_H
#define STRINGS_H

// constants and defines

/**
 * These strings are used inside the GUI interface.
 */
#define C_STRING_GUI_OPEN_JOBLIST_DIALOG            "Open Joblist"
#define C_STRING_GUI_EXPORT_JOBLIST_DIALOG          "Export Joblist"
#define C_STRING_GUI_OPEN_JOBLIST_CMP_DIALOG        "Open Joblist To Compare"
#define C_STRING_GUI_TXM_ENTER_DIP_TX_TIME          "Enter Dipole TX Time [s]"
#define C_STRING_GUI_TXM_ENTER_NUM_STACKS           "Enter Number Of Stacks [-]"
#define C_STRING_GUI_TXM_CHANGE_ALL                 "Do you really want to change the value(s) in all jobs of the joblist?"
#define C_STRING_GUI_TXM_CHANGE_ALL_DIP_POL         "Change TXM Dipole Polarisation"
#define C_STRING_GUI_TXM_CHANGE_ALL_WAVEFORM        "Change TXM Waveform Name"
#define C_STRING_GUI_TXM_CHANGE_ALL_MAX_AMPL        "Change TXM Maximum Amplitude"
#define C_STRING_GUI_COMMENTS_CHANGE_ALL            "Change Comments"
#define C_STRING_GUI_COMMENTS_PROC_CHANGE_ALL_CSAMT "Change Processing Instructions (CSAMT)"
#define C_STRING_GUI_COMMENTS_PROC_CHANGE_ALL_MT    "Change Processing Instructions (MT)"
#define C_STRING_GUI_MAINWINDOW_JOBLIST_EMPTY       "The joblist is empty. Please open/add some jobs."
#define C_STRING_GUI_CHANNEL_CONF_AUTO              "AutoGain/Offset"
#define C_STRING_GUI_CHANNEL_CONF_ACTIVATE_AUTO     "Activating AutoGain/AutoOffset is only usefull if the \"AutoGain/AutoOffset\" selftest jobs have been executed for this site on the ADU system before.\n\nUsing old AutoGain/AutoOffset values may result in bad data.\n With UNIFIED FAST SELFTEST it is O.K."

#define C_STRING_GUI_SENSOR_TYPE_ADAPT_NONE         "Always use the Sensor Configuration of the Joblist (mode = NONE)"
#define C_STRING_GUI_SENSOR_TYPE_ADAPT_E_SERIES     "Overwrite the Sensor Configuration for all detected intelligent sensors, e.g. MFS06e/07e (mode = E_SERIES)"
#define C_STRING_GUI_SENSOR_TYPE_ADAPT_ALL          "Overwrite the complete Sensor Configuration with the values of the last job on ADU system (mode = ALL)"

#define C_STRING_GUI_MAINWINDOW_NO_JOB_SELECTED     "No job selected - doing nothing.\n\nAt least 1 job must be selected!"
#define C_STRING_GUI_MAINWINDOW_NO_AUTO_ADJUST      "Start-/Stop-times are actually not updated automatically.\n\nAs the order of the jobs was changed you\neither should activate \"Auto\" for \"Adjust Pause\"\nor you will need to update the start-/stop-times manually.\n\nActivate \"Auto\" for \"Adjust Pause\"?"
#define C_STRING_GUI_INVALID_INPUT_VALUE            "The value you entered is not a valid number."

#define C_STRING_GUI_INPORT_AUTOGAIN_OPEN_FAILED    "Failed to open \"ADU07ChannelConfig\" XML file at\n\n$1\n"
#define C_STRING_GUI_INPORT_AUTOGAIN_SAME_RF        "Set same Radio Filter for all channels of this board type and Filter configuration?\n"

#define C_STRING_GUI_UNABLE_TO_OPEN_TEMPLATE        "Unable to open Joblist template:<br><br>%1<br><br>Please check your JobListEditor installation.\n"

#define C_STRING_GUI_FAILED_TO_CREATE_NEW_JOBLIST   "Failed to create new Joblist!"
#define C_STRING_GUI_CHOOSE_JOBLIST_MODE            "Choose Joblist Mode"

#define C_STRING_GUI_JOBLIST_SAVED                  "Joblist was saved to disk."
#define C_STRING_GUI_SAVE_CHANGES                   "Save Changes?"
#define C_STRING_GUI_NO_JOBLIST_OPEN                "No Joblist opened."
#define C_STRING_GUI_CHOOSE_JOBLIST_TYPE            "Choose Joblist Type (Target System)"

#define C_STRING_GUI_WARNING_TX_FREQ_TOO_BIG        "The selected TX frequency is bigger than \"Sample Frequency / 4\"\nPlease select a smaller base frequency, or raise the jobs sampling frequency.\n"

#define C_STRING_GUI_FAILED_OPEN_TEMPLATE           "Failed to open Joblist template."


/**
 * These strings define the table headings inside the GUI.
 */
#define C_STRING_GUI_TABLE_HEADING_SAMPLE_FREQ      "Sample Freq."
#define C_STRING_GUI_TABLE_HEADING_START_TIME       "Start Time"
#define C_STRING_GUI_TABLE_HEADING_Duration         "Duration"
#define C_STRING_GUI_TABLE_HEADING_TXM_BASE_FREQ    "TXM Freq."
#define C_STRING_GUI_TABLE_HEADING_DIGFIL_TYPE      "DigFil Type"
#define C_STRING_GUI_TABLE_HEADING_LOCKED           "Locked"


/**
 * These strings define the ToolTips for the Mainwindow.
 */
#define C_STRING_TT_MAINWINDOW_SELECT_PREV           "Select previous job."
#define C_STRING_TT_MAINWINDOW_SELECT_NEXT           "Select next job."
#define C_STRING_TT_MAINWINDOW_SELECT_FIRST          "Select first job."
#define C_STRING_TT_MAINWINDOW_SELECT_LAST           "Select last job."
#define C_STRING_TT_MAINWINDOW_MOVE_1_UP             "Move selected job 1 up."
#define C_STRING_TT_MAINWINDOW_MOVE_1_DOWN           "Move selected job 1 down."
#define C_STRING_TT_MAINWINDOW_APPEND                "Append a new job to the end of the joblist."
#define C_STRING_TT_MAINWINDOW_INSERT                "Insert a new job before the selected job."
#define C_STRING_TT_MAINWINDOW_DELETE                "Delete the selected job. It will be completely removed\nfrom the joblist."
#define C_STRING_TT_MAINWINDOW_CHANGE_SAMPLEFREQ     "Double Click to change sample frequency of job."
#define C_STRING_TT_MAINWINDOW_CHANGE_STARTTIME      "Double Click to change start-time of job."
#define C_STRING_TT_MAINWINDOW_CHANGE_DURATION       "Double Click to change duration of job."
#define C_STRING_TT_MAINWINDOW_ADJUST_PAUSE          "Enable \"Auto\" to let the JobListEditor automatically adjust all start-/stop-times according to\nthe jobs duration and configured pause length.\n\nThis is a MUST, if you add or delete jobs or move jobs to another position within the joblist."
#define C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_FULL_MIN "Activate to make the next job start on a full minute, e.g. last job ends on 10:32:16,\nnew job will start on 10:33:00."
#define C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_OFF      "All start-/stop-times will NOT automatically be adjusted in this mode.\nYou MUST update start-times manually, if you append, insert or delete\n any job, or move a job to another position inside the joblist."
#define C_STRING_TT_MAINWINDOW_ADJUST_PAUSE_AUTO     "All start-/stop-times will automatically be adjusted by the JobListEditor.\nThis is the save mode, if you append, insert or delete any jobs."
#define C_STRING_TT_MAINWINDOW_CSAMT_MODE            "In case you are preparing a joblist for CSAMT activate this option and enter:\n\n1.)the correct number of dipoles that are transmitted,\n2.)the correct duration of each transmitted dipole polarisation\n3.)the number of stacks\n\nUsing this information the duration of the complete job will be computed automatically.\nYou will not be able to change the duration manually, as long as CSAMT mode is active."

/**
 * These strings define the ToolTips for the Channel Config window.
 */
#define C_STRING_TT_CH_CONFIG_ENABLE                "Enable or disable the channel in this job."
#define C_STRING_TT_CH_CONFIG_EXT_GAIN              "Configure the amplification (gain) of the optionally connected satellite box for electric\nfield connection. You only need to change this value from 1 if you connected a satellite box."
#define C_STRING_TT_CH_CONFIG_CHOPPER               "Activate / deactivate the chopper amplifier for connected coils. This should be done by the\nfollowing rule:\n\n1.) Sample frequency <= 512Hz -> Chopper On\nSample frequency > 512Hz -> Chopper Off\n\nIf you do not activate the Chopper for low frequency recordings, the low frequency data will\nbe of poor qualitiy.\n\nIf you activate the Chopper for high frequency recordings, you will see additional\nnoise caused by the chopper amplifier.\n\nChoose \"Auto\" to let the JobListEditor configure the correct value according to the sample frequency."
#define C_STRING_TT_CH_CONFIG_RF_FILTER             "For all jobs that are running on ADU07-ADB-LF (low frequency) boards at least one radio filter\nmust be selected. The filter type depends on the resistance of the electric field probes.\nBy default choose 200Ohm type."
#define C_STRING_TT_CH_CONFIG_GAIN1                 "Select a gain amplification for the first gain stage. This gain stage is located\nbefore the DC offset correction stage and filter."
#define C_STRING_TT_CH_CONFIG_DC_OFFSET_CORR        "Select a DC offset correction value for this channel. The DC offset correction stage\ncan apply a DC offset of +/- 2500mV. To compensate a DC offset of 400mV, select -400."
#define C_STRING_TT_CH_CONFIG_GAIN2                 "Select a gain amplification for the second gain stage. This gain stage is located\nbehind DC offset correction stage and filter in front of the ADC."
#define C_STRING_TT_CH_CONFIG_LP_HP                 "Select an optional low pass / high pass filter. These filters are realized as\nanalog filter stages."
#define C_STRING_TT_CH_CONFIG_AUTOGAIN              "Activate this functionality to replace all the gain and DC offset correction parameters with the ones that\nhave been detected automatically by the ADU system when the \"AutoGain AutoOffset\" jobs have been\nexecuted.\n\nNote, that this is ONLY usefull, if the \"AutoGain AutoOffset\" functionality has been started before on\nthe ADU!"

/**
 * These strings define the ToolTips for the Comments window.
 */
#define C_STRING_TT_COMMENTS_SITE_CONFIG            "Use these parameters to enter additional information about the job. All the entered values\nwill later on be stored inside the meas-doc XML file and ATS headers of all time series files."
#define C_STRING_TT_COMMENTS_PROCMT                 "Enter the name of the ProcMT processing instruction file that shall be used for processing\nthe recorded time series of this job. If this is done properly, hence a processing instruction\nfile exists inside the ProcMT Survey the data processing can easily be started by simple\n\"Double Clicking\" on the time series inside ProcMT GUI."
#define C_STRING_TT_COMMENTS_PROCMT_MT              "Let the JobListEditor automatically configure the ProcMT processing name for normal MT jobs."
#define C_STRING_TT_COMMENTS_PROCMT_CSAMT           "Let the JobListEditor automatically configure the ProcMT processing name for CSAMT jobs."

/**
 * These strings define the ToolTips for the TXM Parameters window.
 */
#define C_STRING_TT_TXM_PARAM_WAVEFORM              "Define the parameters for the output signal waveform of the TXM transmitter.\n\n1.)Name: Defines the type of the out signal waveform. You may choose between:\n        DefaultRect: rectangular signal\n        DefaultSine: sinewave signal\n        DefaultTriangle: triangle signal\n        DefaultRamp: ramp signal\n\n2.)Max. Amplitude: defines the maximum amplitude of the output signal waveform\n   (peak value).\n\n3.)Base Frequency: defines the signal frequency of the output signal waveform."
#define C_STRING_TT_TXM_PARAM_SEQUENCE              "Define the parameters for the transmitted Sequence. A Sequence always consists of several the following 3 parameters\n\n1.)Number of Dipoles:\n    The Number of Dipoles defines how many different directions (dipoles) shall be transmitted. The direction of\n   the output dipole is defined by its Polarisation Angle.\n\n2.)Dipole TX time: Defines the duration each one of the dipoles shall be transmitted, before the next one\n   is transmitted\n\n3.)Number of Stacks: Defines the number of repetations of the transmission of all the dipoles\n\nUsing these values the jobs duration can be computed automatically, if \"CSAMT\" is activated on the main tab."

/**
 * These strings define the ToolTips for the Sensor Config window.
 */
#define C_STRING_TT_SENSOR_CONF_INPUT               "Select the input (connector) for this channel:\n\n0 = The normal connectors (north, south, east, west, Hx, Hy and Hz will be used.\n    Choose this option for all normal MT soundings with EFP and MFS coils.\n\n1 = The multi purpose connector IN2 is used.\n    Choose this option, if you connected e.g. a FGS or SHFT sensor, or external satellite box."
#define C_STRING_TT_SENSOR_CONF_NAME                "Select name of the connected sensor. This value is needed for the later data processing in ProcMT or Mapros.\nThe channel name defines the channels role inside the data processing. For a full MT site at least one channel\nmust be configured to one of the following names: Ex, Ey, Hx, Hy and Hz (optionaly)."
#define C_STRING_TT_SENSOR_CONF_TYPE                "Select the type of the connected sensor. The selection of the sensors calibration function in the later ProcMT or Mapros\ndata processing depends of the correct selection of this value.\n\nNote, that for MFS06e or MFS07e coils this value can be detected automatically by the ADU system."





#endif // STRINGS_H
