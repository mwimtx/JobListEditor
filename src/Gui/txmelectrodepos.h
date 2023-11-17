#ifndef TXMELECTRODEPOS_H
#define TXMELECTRODEPOS_H

#include <QWidget>
#include <QVector>
#include <QPushButton>
#include <QSignalMapper>

#include <datamodel.h>

namespace Ui {
    class TxmElectrodePos;
}

// class definition
class TxmElectrodePos;

struct TxmElectrodePosFOB;

struct TxmElectrodePosUi  : public QWidget
{

    friend struct TxmElectrodePosFOB;

    Q_OBJECT

    public:

        TxmElectrodePosUi (TxmElectrodePos* pclTxmElectrodePosWidget, const Types::Index&);

        TxmElectrodePosUi& setupGui();

        void setupSignals();

        TxmElectrodePos* mTxmElectrodePosWidget;

    private slots:

    private:
        unsigned int mStartRow;
        unsigned int mStartColumn;
};




/**
 * \brief This is the "TxmElectrodePos" class.
 *
 * This is the "TxmElectrodePos" class that implements the "TXM Electrode Positions"
 * widget that is shown on the "TXM Electrode Positions" tab of the JLE.
 * It implements input elements to enter the electrode positions of the Transmitter
 * electrodes that will be written to all the XML job files.
 *
 * \date 2013-11.22
 * \author MWI
 */
class TxmElectrodePos : public QWidget
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
         * \date 2013-11-23
         */
        explicit TxmElectrodePos(QWidget *parent = 0);

        ~TxmElectrodePos();

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
         * \date 2013-11-23
         */
        inline void setDataModel(DataModel* pclDataModel)
        {
            mData = pclDataModel;
        }


    private:

        /**
         * This is the link to the graphical user interface of this widget.
         */
        Ui::TxmElectrodePos *ui;

        /**
         * This pointer holds the link to the data model class that contains all
         * the joblist information.
         */
        DataModel* mData;

        /**
         * This vector contains the pointers to the single buttons for setting
         * longitude, latitude and elevation.
         */
        QVector<QPushButton*> qvecLatButtons;   ///< buttons for latitude
        QVector<QPushButton*> qvecLonButtons;   ///< buttons for longitude
        QVector<QPushButton*> qvecElevButtons;  ///< buttons for elevation

        /**
         * This signal mapper handles the signals for the single buttons.
         */
        QSignalMapper qsmapLatButtons;
        QSignalMapper qsmapLonButtons;
        QSignalMapper qsmapElevButtons;

        /**
         * \brief This function will update the GUI with the current values of the jobs.
         *
         * This function will read all TXM parameters from the data model class
         * and update the GUI elements with this information. It is called each time
         * the data model contents changes.
         *
         * \author MWI
         * \date 2013-11-23
         */
        void updateGuiElements(void);


    private slots:

        void on_pbLon_clicked       (int);
        void on_pbLat_clicked       (int);
        void on_pbElevation_clicked (int);


    public slots:

        void handleJobDataLoaded();

        void handleJobDataChanged();

        void jobSelectionChanged (const QItemSelection&, const QItemSelection&);


};

#endif // TXMELECTRODEPOS_H
