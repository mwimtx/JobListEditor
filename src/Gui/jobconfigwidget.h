#ifndef JOBCONFIGWIDGET_H
#define JOBCONFIGWIDGET_H

#include <QWidget>
#include <vector>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>

#include "factories.h"
#include "datamodel.h"
#include "codemapper.h"

namespace Ui {
    class JobConfigWidget;
}

class JobConfigWidget;

struct ChannelUiFactoryFOB;

struct ChannelConfigUi  : public QWidget {

    friend struct ChannelUiFactoryFOB;

    Q_OBJECT

public:

    QHBoxLayout *horizontalLayout;
    // QLabel      *lb_channel;
    QPushButton *pb_channel_enabled;
    QPushButton *pb_ext_gain;
    QPushButton *pb_chopper;
    QPushButton *pb_rf_filter;
    QPushButton *pb_gain1;
    QPushButton *pb_dac;
    QPushButton *pb_lp_hp_filter;
    QPushButton *pb_gain2;
    
    // ChannelConfigUi( JobConfigWidget* parent, QGridLayout* gridLayout , const Types::Index& channelNo );

    inline Types::Index getChannelNumber() const { return mChannelNumber; }

private slots:

    void handle_pb_pb_channel_enabled();

    void handle_pb_ext_gain();
    void handle_pb_chopper();
    void handle_pb_rf_filter();
    void handle_pb_gain1();
    void handle_pb_dac();
    void handle_pb_lp_hp_filter();
    void handle_pb_gain2();

private:

    ChannelConfigUi( JobConfigWidget* jobConfigWidget , const Types::Index& );

    inline ChannelConfigUi& setStartRowInGrid    ( unsigned int startRow )            { mStartRow = startRow;               return *this; }
    inline ChannelConfigUi& setStartColumnInGrid ( unsigned int startColumn )         { mStartColumn = startColumn;         return *this; }
    ChannelConfigUi& setupGui();
    void setupSignals();

    JobConfigWidget* mJobConfigWidget;

    const Types::Index mChannelNumber;

    unsigned int mStartRow;
    unsigned int mStartColumn;

};

class JobConfigWidget : public QWidget
{
        Q_OBJECT

    friend struct ChannelUiFactoryFOB;
        
public:

    explicit JobConfigWidget(QWidget *parent = 0);
    ~JobConfigWidget();

    inline void setDataModel( DataModel* dataModel ) { mData = dataModel; }

public slots:

    // this comes from the outside
    void handleJobDataLoaded();
    void handleJobDataChanged();
    void handleSensorConfigLoaded();
    void handleSensorConfigChanged();

    void handleEnableChannel( const Types::Index& , bool ); // switch on/off for current job.

    void setExtGainForChannel    ( const Types::Index& );
    void setChopperForChannel    ( const Types::Index& );
    void setRfFilterForChannel   ( const Types::Index& );
    void setGain1ForChannel      ( const Types::Index& );
    void setDacForChannel        ( const Types::Index& );
    void setLpHpFilterForChannel ( const Types::Index& );
    void setGain2ForChannel      ( const Types::Index& );

    // FIXME: better get notified by the datamodel?
    void jobSelectionChanged( const QItemSelection & , const QItemSelection & );



signals:

    void goToJobTab();


private slots:
    
    void handleBackButton();

    void on_atBeginButton_clicked ();

    void on_atEndButton_clicked ();

    void on_oneBackButton_clicked ();

    void on_oneForewardButton_clicked ();

    void on_chbUseAutoGainAutoOffset_stateChanged ();

    void on_pbImportAutoGainOffset_clicked ();


protected:

    virtual void showEvent ( QShowEvent * );

private:

    /**
     * \brief This function will add ToolTips to the single input elements.
     *
     *-
     *
     */
    void addToolTips (void);

    friend struct ChannelConfigUi;

    Ui::JobConfigWidget *ui;

    QGridLayout* getButtonGrid() const;

    void setupChannelConfigGrid();
    void setupViews();

    typedef std::vector<ChannelConfigUi*>  ChannelConfigVector;
    ChannelConfigVector mChannelConfigVector;

    DataModel* mData;

    void updateChannelStatus();

    Tools::CodeMapper mCodeMapperLF;
    Tools::CodeMapper mCodeMapperMF;

};


#endif // JOBCONFIGWIDGET_H
