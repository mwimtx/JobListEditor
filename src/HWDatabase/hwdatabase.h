#ifndef _HWDATABASE_H_
#define _HWDATABASE_H_

#include <QSharedPointer>
#include <QFile>
#include <QDomDocument>
#include <QStringList>

#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <map>

#include "factories.h"
#include "tools_types.h"
#include "default_values.h"

#include "hwkeydata.h"
#include "hwdbqueryresult.h"

namespace Tools {
    class DomElementSiblingIterator;
}


/**
 * This enum defines the single ADB board types.
 */
enum t_AdbBoardTypes
{
    ADB_BACK_MAIN                   =   1,      ///< main backplane board, including PSU
    ADB_BACK_SUB                    =   2,      ///< sub backplane board with FPGA
    ADB_CLOCK                       =   4,      ///< clock / GPS board
    ADB_CON                         =   5,      ///< connector board
    ADB_CAL                         =   6,      ///< calibration board
    ADB_LF                          =  10,      ///< standard LF ADB board
    ADB_HF                          =  11,      ///< standard HF ADB board
    ADB_MF                          =  12,      ///< standard MF ADB board
    ADB_ADU08_BB                    =  13,      ///< standard ADU-08e BB board
    ADB_ADU09U_10E                  =  14,      ///< standard ADU-09u/10e channels
    ADB_ARS2340                     =  21,      ///< ARS2340 drive for TXM-22
    ADB_ARS2360W                    =  22,      ///< ARS2360W drive for TXM-22
    ADB_ARS2108                     =  23,      ///< ARS2108 drive for TXM-22
    ADB_ARS2310                     =  24,      ///< ARS2310 drive for TXM-22
    ADB_ARS2320                     =  25,      ///< ARS2320 drive for TXM-22
    ADB_CPU_UNKNOWN                 = 100,      ///< unknown CPU board
    ADB_CPU_ARCOM_VIPER             = 101,      ///< Arcom Viper CPU board
    ADB_CPU_LIPPERT_LITERUNNER      = 102,      ///< Lippert Cool Literunner LX800
    ADB_CPU_EMBEDDED_LOGIC_PBATOM   = 103,      ///< Embedded Logic PB-ATOM+
    ADB_CPU_SECO_QUADMO747_OMAP4    = 104,      ///< Seco Quadmo/747-X/OMAP4
    ADB_CPU_ISEE_IGEPv2_OMAP3       = 105,      ///< ISEE Igepv2/OMAP3
    ADB_SENSOR_MFS06E               = 200,      ///< MFS-06e coil
    ADB_SENSOR_MFS07E               = 201,      ///< MFS-07e coil
    ADB_EFD07E                      = 202,      ///< connector sattelite box
    ADB_UNKNOWN_SENSOR              = 203,      ///< unknown sensor
    ADB_SENSOR_SHFT02E              = 204,      ///< SHFT02e super high frequency triplet
    ADB_SENSOR_FGS03E               = 205,      ///< FGS03e fluxgate sensor
    ADB_UNKNOWN                     = 0xFF      ///< unknown or invalid ADB board type
};


/**
 * This enum defines the single Device types
 */
enum t_DeviceTypes
{
    DEVICE_ADU07                    =   0,      ///< ADU-07(e)
    DEVICE_TXB07                    =  20,      ///< TXB-07
    DEVICE_UNKNOWN                  = 0xFF      ///< unknown or invalid ADB board type
};


/**
 * This enum defines all GMS versions.
 */
enum t_GmsVersions
{
    GMS_UNKNOWN = 0,
    GMS_01      = 1,
    GMS_02      = 2,
    GMS_03      = 3,
    GMS_04      = 4,
    GMS_05      = 5,
    GMS_06      = 6,
    GMS_07      = 7,
    GMS_08      = 8,
    GMS_09      = 9,
    GMS_10      = 10
};



////////////////////////////////////////////////////////////////////////////////
//
// result classes for hw queries

class HwInfo 
{
protected:
    
    HwInfo( HWDBQueryResult::Spt queryResult ) :
        mQueryResult( queryResult )
        {}
    
public:
    
    virtual void parse() = 0; // throws std::runtime_error

    virtual bool isVaild() const { return mQueryResult -> isVaild(); }
 
    virtual unsigned getGSM     () const { return mQueryResult -> getGSM();     }
    virtual unsigned getType    () const { return mQueryResult -> getType();    }
    virtual unsigned getRevMain () const { return mQueryResult -> getRevMain(); }
    virtual unsigned getRevSub  () const { return mQueryResult -> getRevSub();  }

    virtual QString getName() const { return mQueryResult -> getName(); }

    virtual ~HwInfo() {} // any other idea to pervent instances of this? If we could use c++11 ....

protected:

    inline bool     hasId() const       { return mQueryResult -> hasId(); }
    inline unsigned getId() const       { return mQueryResult -> getId(); }

    inline QDomElement getDomElement() const { return mQueryResult -> getDomElement(); }

    HWDBQueryResult::Spt mQueryResult;


};

////////////////////////////////////////////////////////////////////////////////
//
// sensor specific data

class SensorInfo : public HwInfo
{
public:

    typedef QSharedPointer<SensorInfo> Spt;

    SensorInfo( HWDBQueryResult::Spt queryResult ) :
        HwInfo( queryResult )
        {}
    
    virtual void parse();  // throws std::runtime_error

    unsigned getChannelId() const { 
        if ( not hasId() )
            throw std::runtime_error( "try to access undefined channel id!" );
        return getId();
    }
    
};

typedef Tools::CreateWithArg<SensorInfo,HWDBQueryResult::Spt> SensorInfoFactory;

////////////////////////////////////////////////////////////////////////////////
//
// AdbBoard specific data

class AdbBoardInfo : public HwInfo
{
public:

    typedef QSharedPointer<AdbBoardInfo> Spt;

    explicit AdbBoardInfo( HWDBQueryResult::Spt queryResult );

    virtual void parse();  // throws std::runtime_error

    unsigned getChannelId() const { 
        if ( not hasId() )
            throw std::runtime_error( "try to access undefined channel id!" );
        return getId();
    }

    QStringList getExtGainRawList() const; // need this?

    QString     getExtGainMin()   const;
    QString     getExtGainMax()   const;
    QStringList getChopper()      const; // fake till now; does not exist in HWDB
    QStringList getRfFilter()     const;
    QStringList getGainStage1()   const;
    QStringList getGainStage2()   const;
    QStringList getSampleFreqs()  const;
    int         getDacRangeLow()  const;
    int         getDacRangeHigh() const;
    bool        getHasDac()       const; 
    QStringList getLpHpFilter()   const;

private:

    void parseExtGain();
    void parseFilters();
    void parseGainStage1();
    void parseGainStage2();
    void parseSampleFreqs();
    void parseDac();

    const QStringList mChopperList; // does not exist in HWDB

    QStringList mExtGain;
    QStringList mRfFilter;
    QStringList mGainStage1;
    QStringList mGainStage2;
    QStringList mSampleFreqs;
    int mDacLow;
    int mDacHigh;
    QStringList mLpHpFilter;


};

typedef Tools::CreateWithArg<AdbBoardInfo,HWDBQueryResult::Spt> AdbBoardInfoFactory;

////////////////////////////////////////////////////////////////////////////////
//
// the hardware db itself

class HardwareDB 
{
public:

    typedef QSharedPointer<HardwareDB> Spt;

    static Spt createFromFile( QFile& );

    // meeting minutes: "Primärschlüssel beim Mapping der von der ADU erkannten HW-Daten auf die Hardwaredatenbank sind GSM,Type,Revision,SubRevision."
    // <GMS>7</GMS>
    // <Type>25</Type>
    // <RevMain>1</RevMain>
    // <RevSub>0</RevSub>

    QString getVersion() const { return mVersionString; }

    HWDBQueryResult::Spt query( const HWDBKeyData& );

private:

    HardwareDB();

    void setup();

    QString mVersionString;

    QDomDocument mHWDbDocument;

};

////////////////////////////////////////////////////////////////////////////////&
//
// the currently available hardwareconfiguration

class HardwareConfig
{
public:

    typedef QSharedPointer<HardwareConfig> Spt;

    static Spt createFromFile( QFile& , HardwareDB::Spt );

    QString getVersion() const { return mVersionString; }

    SensorInfo::Spt getSensorInfoForChannel( const Types::Index& ); // returns the SensorInfo or an empty Spt if the channel does not exist. throws std::out_of_range

    AdbBoardInfo::Spt getAdbBoardInfoForChannel( const Types::Index& ); // returns the AdbBoardInfo or an empty Spt if the channel does not exist. throws std::out_of_range

    /**
     * \brief Checks, if the hardware config of the joblist is created for a TXM Transmitter.
     *
     * This function is used to check, if the hardware config, that belongs to this joblist
     * was created on a transmitter (TXM-22) machine.
     *
     * @param[out] const bool = true: joblist is meant to run on transmitter / false: is meant to run on ADU
     *
     * \author MWI
     * \date 2014-07-17
     */
    bool isTXM (void);

    /**
     * \brief Checks, if the hardware config of the joblist is created for a MF board ADU.
     *
     * This function is used to check, if the hardware config, that belongs to this joblist
     * was created on a ADU-07e machine with MF boards installed.
     *
     * @param[out] const bool = true: joblist is meant to run on MF board ADU / false: is meant to run on LF/HF board ADU
     *
     * \author MWI
     * \date 2014-07-17
     */
    bool isMF (void);


    /**
     * \brief Checks, if the hardware config of the joblist is created for a ADU-08e BB board ADU.
     *
     * This function is used to check, if the hardware config, that belongs to this joblist
     * was created on a ADU-87e machine with ADU-08e BB boards installed.
     *
     * @param[out] const bool = true: joblist is meant to run on ADU-08e BB board ADU / false: is meant to run on LF/HF board ADU-07e
     *
     * \author MWI
     * \date 2016-06-02
     */
    bool isADU08eBB (void);


    /**
     * \brief Checks, if the hardware config of the joblist is created for a ADU-10e board ADU.
     *
     * This function is used to check, if the hardware config, that belongs to this joblist
     * was created on a ADU-10e machine.
     *
     * @param[out] const bool = true: joblist is meant to run on ADU-10e BB board ADU / false: is meant to run on LF/HF board ADU-07e
     *
     * \author MWI
     * \date 2021-10-06
     */
    bool isADU09u10e (void);



    QDomElement getXMLHwConfigNode() const;

private:

    typedef std::vector<HWDBQueryResult::Spt> HwDataVector;

    HardwareConfig();

    void setup();
  
    void loadAdbBoards();
    void loadBoards();
    void loadSensors(); // throws std::runtime_error
    void createSensorInfoVector(); // throws std::runtime_error
    void createAdbBoardInfoVector(); // throws std::runtime_error

    typedef std::vector<SensorInfo::Spt> SensorInfoVector;
    SensorInfoVector mSensorInfoVector;
    typedef std::vector<AdbBoardInfo::Spt> AdbBoardInfoVector;
    AdbBoardInfoVector mAdbBoardInfoVector;

    void loadHardwareConfigData( Tools::DomElementSiblingIterator& , HwDataVector&  );

    QString mVersionString;

    QDomDocument mHWCfgDocument;

    HardwareDB::Spt mHwDb;

    // FIXME: this will be replaced by hardware part specific vectors: sensors, boards... whatever..
    // FIXME: these will become obsolete when data is stored in the respective info vector
    HwDataVector mHwDataVector;
    HwDataVector mHwDeviceVector;
    HwDataVector mSensorsVector;
    HwDataVector mAdbBoardVector;
};


#endif /* _HWDATABASE_H_ */
