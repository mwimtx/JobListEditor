#ifndef SENSORPOSITIONS_H
#define SENSORPOSITIONS_H

#include <QWidget>
#include "datamodel.h"
#include "factories.h"

namespace Ui {
    class SensorPositions;
}

#include <QLabel>
#include <QPushButton>
#include <vector>
#include <QSharedPointer>

class SensorPositions;

class SensorChannelPositionsUi : public QObject
{

    Q_OBJECT
       
public:

    typedef QSharedPointer<SensorChannelPositionsUi> Spt;

    SensorChannelPositionsUi( SensorPositions* sensorPositions , const Types::Index& channelNumber ) :
        mChannelNumber( channelNumber ),
        mSensorPositionsWidget ( sensorPositions )
        {}

    SensorChannelPositionsUi& setlbChannelNumber( QLabel* lbChannelNumber ) { mLbChannelNumber = lbChannelNumber; return *this; }
    SensorChannelPositionsUi& setlbChannelType( QLabel* lbChannelType )     { mLbChannelType = lbChannelType; return *this; }
    SensorChannelPositionsUi& setpbNorth( QPushButton *pbNorth )            { mPbNorth = pbNorth; return *this; }
    SensorChannelPositionsUi& setpbSouth( QPushButton *pbSouth )            { mPbSouth = pbSouth; return *this; }
    SensorChannelPositionsUi& setpbEast( QPushButton *pbEast )              { mPbEast = pbEast; return *this; }
    SensorChannelPositionsUi& setpbWest( QPushButton *pbWest )              { mPbWest = pbWest; return *this; }
    
    Types::Index getChannelNumber() const { return mChannelNumber; }

    QLabel* getLbChannelNumber() const { return mLbChannelNumber; }
    QLabel* getLbChannelType() const { return mLbChannelType; }
    QPushButton* getPbNorth() const { return mPbNorth; }
    QPushButton* getPbSouth() const { return mPbSouth; }
    QPushButton* getPbEast() const { return mPbEast; }
    QPushButton* getPbWest() const { return mPbWest; }

private slots:

    void handlePbNorth();
    void handlePbSouth();
    void handlePbEast();
    void handlePbWest();

private:

    const Types::Index mChannelNumber;

    QLabel* mLbChannelNumber;
    QLabel* mLbChannelType;
    QPushButton *mPbNorth;
    QPushButton *mPbSouth;
    QPushButton *mPbEast;
    QPushButton *mPbWest;

    SensorPositions* mSensorPositionsWidget;

};

typedef Tools::CreateWithTwoArgs<SensorChannelPositionsUi,SensorPositions*,const Types::Index&> SensorChannelPositionsUiFactory;

class SensorPositions : public QWidget
{
        Q_OBJECT
        
    public:

    typedef std::vector<SensorChannelPositionsUi::Spt> SensorChannelPositionsUiVector;

    explicit SensorPositions(QWidget *parent = 0);
    ~SensorPositions();
    
    inline void setDataModel( DataModel* dataModel ) { mData = dataModel; }
    
    void handlePbNorth( const Types::Index&  );
    void handlePbSouth( const Types::Index& );
    void handlePbEast( const Types::Index& );
    void handlePbWest( const Types::Index& );

signals:

    void goToJobTab();
        
protected:

    virtual void showEvent ( QShowEvent * );

private slots:
    
    void handleBackButton();

    void on_chbAdaptSensorSpacing_stateChanged (int);

    void on_pbLinkLFHF_clicked (void);


private:
    Ui::SensorPositions *ui;

    void setupGui();
    void updateGui();
    void updateDataModel();

    DataModel* mData;

    SensorChannelPositionsUiVector mSensorChannelPositionsUiVector; 

};

#endif // SENSORPOSITIONS_H
