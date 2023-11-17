#ifndef SENSORSWIDGET_H
#define SENSORSWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QSharedPointer>
#include <QComboBox>
#include <vector>

#include "tools_types.h"
#include "factories.h"
#include "datamodel.h"

namespace Ui {
    class SensorsWidget;
}

class SensorsWidget;

struct SetupChannels;
struct UpdateChannels;

//FIMXE: put HW into this name so that we don not get confused with "soft" channel configs.
class SensorHwChannelUi : public QObject
{

    friend struct SetupChannels;
    friend struct UpdateChannels;

    Q_OBJECT
       
public:

    // sorry, braindead qt does not allow copy ctor for qobjects an so it does not meet the requirements of stl container.
    typedef QSharedPointer<SensorHwChannelUi> Spt;

    ////////////////////////////////////////////////////////////////////////////////
    //
    // set underlying widgets

    SensorHwChannelUi& setPbSensorType         ( QPushButton* value ) { mPbSensorType         = value; return *this; }
    SensorHwChannelUi& setPbChannelName        ( QPushButton* value ) { mPbChannelName        = value; return *this; }
    SensorHwChannelUi& setLbChannelNumber      ( QLabel* value      ) { mLbChannelNumber      = value; return *this; }
    SensorHwChannelUi& setPbInput              ( QPushButton* value ) { mPbInput              = value; return *this; }
    SensorHwChannelUi& setPbSensorSerialNumber ( QPushButton* value ) { mPbSensorSerialNumber = value; return *this; }    

    QPushButton* getPbInput ()             const { return mPbInput;         }
    QLabel*      getLbChannelNumber ()     const { return mLbChannelNumber; }
    QPushButton* getPbChannelName ()       const { return mPbChannelName;   }
    QPushButton* getPbSensorType ()        const { return mPbSensorType;    }
    QPushButton* getPbSensorSerialNumber() const { return mPbSensorSerialNumber; }

    ////////////////////////////////////////////////////////////////////////////////
    //
    // get / set channel data

    SensorHwChannelUi& setEnabled( bool ) ;
    inline bool getIsEnabled() const { return mIsEnabled; }

    inline int getChannelNumber () const { return mChannelNumber; }
    SensorHwChannelUi& setChannelNumber ( const int& value ) { mChannelNumber = value; return *this; }

    // InputConfig getInputConfig () const { return mInputConfig; }
    QString  getInputConfig () const { return mInputConfig; }
    // SensorHwChannelUi&  setInputConfig ( const InputConfig& value ) { mInputConfig = value; return *this; }
    SensorHwChannelUi&  setInputConfig ( const QString& value ) { mInputConfig = value; return *this; }

    SensorHwChannelUi&  setSensorType( const QString& );
    
public slots:

    void handlePbInput();
    void handlePbChannelName();
    void handlePbSensorType();
    void handlePbSensorSerialNumber();

signals:
    void sigUpdateDisplayAllChannels (void);

private:

    void connectSignals();
    void updateDisplay();

    DataModel* getDataModel() const;

    // FIXME: this should move into tools...
    QString getNextValue( const DefaultValues::VectorOfQStrings& , const QString&  );

    QPushButton* mPbInput;
    QLabel*      mLbChannelNumber;
    QPushButton* mPbChannelName;
    QPushButton* mPbSensorType;
    QPushButton* mPbSensorSerialNumber;

    int mChannelNumber;
    bool mIsEnabled;

    QString     mInputConfig;
    QString     mSensorType;

    SensorsWidget* mParent;

public:

    SensorHwChannelUi( SensorsWidget* parent ) :
        QObject( 0 ),
        mIsEnabled( false ),
        mInputConfig( DefaultValues::Undefined ),
        mSensorType( "" ),
        mParent( parent )
        {}
};

typedef Tools::CreateWithArg<SensorHwChannelUi,SensorsWidget*> SensorHwChannelUiFactory;

class SensorsWidget : public QWidget
{
    // Q_OBJECT, i hate you.
    Q_OBJECT
        
public:
    explicit SensorsWidget(QWidget *parent = 0);
    ~SensorsWidget();

    // why the hell is QObject... *sigh* 
    // FIXME: find another way to make sure we have data set here!!!
    // turn this into a class to inherit with some lazy init magic... ?
    inline void       setDataModel( DataModel* dataModel ) { mData = dataModel; }
    inline DataModel* getDataModel() const { return mData; }

    QPushButton* pbLinkLFHF;

public slots:

    // this comes from the outside
    void handleSensorConfigLoaded();
    void handleSensorConfigChanged();

signals:

    void goToJobTab();

private slots:

    void handleBackButton();

    void handleAdaptSensorType ();

    void on_pbLinkLFHF_clicked (void);

    void slotUpdateDisplayAllChannels (void);
        
private:

    QGridLayout *mInnerGridLayout;
    QHBoxLayout *mInnerHBoxLayout;
    QFrame      *mLine;
    QPushButton *mPb_back;

    QComboBox* pcbAdaptSensorType;


    typedef std::vector<SensorHwChannelUi::Spt> SensorHwChannelUiVector;
    SensorHwChannelUiVector mSensorHwChannelUiVector;

    // aggegate ui-stuff to per-channel units
    void setupObjectPlumbing();

    // wire up the channels...
    void setupChannels();

    // enable / disable stuff based on config
    void setEnabledForChannelTo( const Types::Index& , bool );// throws std::out_of_range 

    DataModel* mData;

};

#endif // SENSORSWIDGET_H
