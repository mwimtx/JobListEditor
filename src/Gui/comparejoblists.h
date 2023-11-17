#ifndef COMPAREJOBLISTS_H
#define COMPAREJOBLISTS_H

#include <QDialog>
#include <QHBoxLayout>
#include <datamodel.h>

namespace Ui {
    class CompareJobLists;
}


/**
 * This struct is used to store the main attributes of a job to be
 * able to check for confilciting jobs between the 2 joblists.
 */
typedef struct
{
    unsigned int uiStartTime;   ///< computed start-time - relative to start-time of joblist
    unsigned int uiDuration;    ///< duration of job in seconds
    double       dTXFreq;       ///< TX frequency
    double       dTXDipTime;    ///< TX dipole time
    double       dRXFreq;       ///< RX frequency
} sJobAtttributes;


/**
 * \class CompareJobLists
 *
 * \brief This class implements the "Compare Joblists" Dialog.
 *
 * The "CompareJoblists" dialog shall be used to make it possible for the
 * user to easily compare the schedule of 2 joblists. This is necessary e.g.
 * if you run 2 synchronous joblists on ADU-07e and TXM-22 in CSAMT mode.
 * The window will show the jobs of both joblists as "GANT chart", showing the
 * jobs sample frequency and TXM frequency.
 *
 * \date 2014-01-29
 * \author MWI
 */
class CompareJobLists : public QDialog
{
    Q_OBJECT

    public:

        /**
         * \brief This is the class constructor.
         *
         * This is the constructor. It will setup the GUI elements and
         * additonally trigger the drawing of the joblists chart.
         *
         * @param[in] DataModel* pclJobList1 = pointer to joblist from Editor
         * @param[in] QString& qstrCmpJobListPath = path to joblist to be compared to
         * @param[in] QWidget *parent = 0 = pointer to parent widget
         */
        explicit CompareJobLists(DataModel* pclJobList1, QString& qstrCmpJobListPath, QWidget *parent = 0);

        /**
         * \brief This is the class destructor.
         *
         * -
         */
        ~CompareJobLists();

    private slots:

        /**
         * \brief This slot is called whenever the Zoom slider has changed.
         *
         * This slot will redraw the chart, if the "Zoo" slider was moved. This is done
         * by simply changing the value of the "dPixPerSecond" attribute and redrawing
         * the plot.
         *
         * @param[in] int iNewValue = new slide value (div by 10 to get new pixel per second value)
         */
        void on_slZoom_valueChanged (int iNewValue);

        /**
         * \brief This slot is called whenever the OK button is clicked.
         *
         * It will close this window
         */
        void on_pbOK_clicked (void);

        /**
         * \brief This slot is called whenever the "Relative SartTimes" button is clicked.
         *
         * It will display the joblists either relative to the start-time of the first job, or
         * absolutely.
         */
        void on_pbRelativeStart_clicked (void);

    private:

        /**
         * \brief This function will draw the joblist char.
         *
         * This function will evaluate the single start-times and durations of the
         * single jobs and draw them as charts.
         *
         * @param[out] bool
         */
         bool drawChart (void);

         /**
          * \brief This function will parse the joblist.
          *
          * This function will parse throught the joblist and create all buttons and place
          * them on the GUI chart, as well as fill the vectors with job attributes.
          *
          * @param[in] DataModel* pclJobList = pointer to joblist to be parsed
          * @param[in] QHBoxLayout* pclLayout = layout, the buttons shall be added on
          * @param[in] QVector<QPushButton*>& qvecButtons = button vector
          * @param[in] QVector<sJobAtttributes>& qvectAttributes = job attribute vector
          * @param[in] time_t& sStartTimeAbs = reference to variable the start time of the first job shall be stored in
          * @param[out] bool
          */
          bool parseJoblist (DataModel*                pclJobList,
                             QHBoxLayout*              pclLayout,
                             QVector<QPushButton*>&    qvecButtons,
                             QVector<sJobAtttributes>& qvectAttributes,
                             time_t&                   sStartTimeAbs);

        /**
         * Pointer to source joblist from editor.
         */
        DataModel* pclSrcJobList;
        DataModel* pclCmpJobList;

        /**
         * These vectors will finally contain the single buttons that will display the
         * joblist inside the chart.
         */
        QVector<QPushButton*> qvecSrcJoblistButtons;
        QVector<QPushButton*> qvecCmpJoblistButtons;

        /**
         * These vectors will finally contain the main attributes of the single jobs.
         */
        QVector<sJobAtttributes> qvecSrcJobListAttr;
        QVector<sJobAtttributes> qvecCmpJobListAttr;

        /**
         * These attributes store the absolute start-times of the first job of the
         * two joblists.
         */
        time_t sStartTimeSrcJoblist;
        time_t sStartTimeCmpJoblist;

        /**
         * This attribute contains the resolution of the joblist chart.
         */
        double dPixelPerSecond;

        /**
         * Link to GUI class (form)
         */
        Ui::CompareJobLists* ui;
};

#endif // COMPAREJOBLISTS_H
