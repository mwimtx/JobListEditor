/**
 * \brief This file contains the "TXMParameters" widget.
 *
 * This file contains the class definition of the "TXM Parameters" widget
 * that is shown on the "TXM Parameters" tab of the JLE.
 *
 * \author MWI
 * \date 2013-10-23
 */

// self include procetion
#ifndef TXMPARAMETERS_H
#define TXMPARAMETERS_H

#include <QWidget>
#include <QSignalMapper>

#include "datamodel.h"

// namespace
namespace Ui {
    class txmparameters;
}

// class definition
class txmparameters;

struct TxmParametersFOB;

struct TxmParametersUi  : public QWidget
{

    friend struct TxmParametersFOB;

    Q_OBJECT

    public:

        TxmParametersUi (txmparameters* pclTxmParametersWidget, const Types::Index&);

        TxmParametersUi& setupGui();

        void setupSignals();

        txmparameters* mTxmParametersWidget;

    private slots:

    private:
        unsigned int mStartRow;
        unsigned int mStartColumn;
};


/**
 * \brief This is the "txmparameters" class.
 *
 * This is the "txmparamaters" class that implements the "TXM Parameters"
 * widget that is shown on the "TXM Parameters" tab of the JLE.
 * It implements input elements to enter all the TXM specific parameters
 * of the job, as Waveform name and amplitude, Number of dipoles,
 * polarisatio angles, ... .
 *
 * \date 2013-10-23
 * \author MWI
 */
class txmparameters : public QWidget
{
    Q_OBJECT

    public:

        /**
         * \brief This is the constructor.
         *
         * -
         *
         * @param[in] QWidget *parent = 0 = pointer to parent widget
         *
         * \author MWI
         * \date 2013-10-23
         */
        explicit txmparameters(QWidget *parent = 0);
        ~txmparameters();

        /**
         * \brief This function is used to set the pointer to the data model class instance.
         *
         * This function is called by the mainwindow class to set the pointer to the
         * data model class instance that is used to show all the job information
         * inside the table view.
         *
         * @param[in] DataModel* pclDataModel = pointer to data model.
         *
         * \author MWI
         * \date 2013-10-23
         */
        inline void setDataModel(DataModel* pclDataModel)
        {
            mData = pclDataModel;
        }


    private:

        Ui::txmparameters *ui;

        QSignalMapper* pclTXMDipPolMapper;

        /**
         * This pointer holds the link to the data model class that contains all
         * the joblist information.
         */
        DataModel* mData;

        /**
         * \brief This function is used to setup the table view for the joblist tab.
         *
         * This function will initialise the job table view with the actual contents
         * of the data model.
         *
         * \author MWI
         * \date 2013-10-23
         */
        void setupViews(void);

        /**
         * \brief This function will update the GUI with the current values of rge jobs.
         *
         * This function will read all TXM parameters from the data model class
         * and update the GUI elements with this information. It is called each time
         * the data model contents changes.
         *
         * \author MWI
         * \date 2013-10-23
         */
        void updateChannelStatus(void);


        void updateTXMIterations (void);


        /**
         * \brief This function will add ToolTips to the single input elements.
         *
         *-
         *
         */
        void addToolTips (void);



    private slots:

        void on_cbTXMWaveformName_currentIndexChanged(int index);

        void on_sspinTXMMaxAmpl_valueChanged(double d);

        void on_cbTXMBaseFreq_currentIndexChanged(int index);

        void on_pbTXMDipTXTime_clicked ();

        void on_pbTXMNumStacks_clicked ();

        void on_cbTXMNumDipoles_currentIndexChanged(int index);

        void on_DipolePolChanged (const int&);

        void on_pbTXMDipoleChangeAll_clicked ();

        void on_pbTXMWaveformChangeAll_clicked ();

        void on_pbTXMAmplChangeAll_clicked ();

        void on_atBeginButton_clicked ();

        void on_atEndButton_clicked ();

        void on_oneBackButton_clicked ();

        void on_oneForewardButton_clicked ();


    public slots:

        void handleJobDataLoaded();

        void handleJobDataChanged();

        void jobSelectionChanged (const QItemSelection&, const QItemSelection&);


};

#endif // TXMPARAMETERS_H
