#ifndef _DEFAULT_VALUES_H_
#define _DEFAULT_VALUES_H_

#include "tools_types.h"

#include <QString>
#include <vector>

namespace DefaultValues
{
    static const unsigned int Frequency = 1024; // Hz
    static const Types::TimeInterval JobDuration( 120 ); // Seconds
    
    static const unsigned int BorderFrequency = 4096; // equal and below are LF(?) things...
    static const unsigned int ChopperAutoFrequency = 512;
    static const unsigned int LongerPauseFrequency = 65536;

    static const unsigned int MaxChannels = 10;

    static const QString TimeFormat( "hh:mm:ss" );
    static const QString TimeFormatForFiles( "hh-mm-ss" );
    static const QString DateFormat( "yyyy-MM-dd" ); // ??
    static const QString DateTimeFormat ( DateFormat + " " + TimeFormat );

    static const QString HwDatabaseFile ( "HwDatabase.xml" );
    static const QString HwConfigFile   ( "HwConfig.xml" );  

    static const QString Off ( "Off" );
    static const QString Undefined ( "Undefined" );
    static const QString Zero ( "0" );
    static const QString One  ( "1" );
    static const QString Auto ( "auto" );
    
    static const QString Ex  ("Ex");
    static const QString Ey  ("Ey");
    static const QString Hx  ("Hx");
    static const QString Hy  ("Hy");
    static const QString Hz  ("Hz");
    static const QString REx ("REx");
    static const QString REy ("REy");
    static const QString RHx ("RHx");
    static const QString RHy ("RHy");
    static const QString RHz ("RHz");

    static const QString EFP06   ("EFP06");
    static const QString EFP07   ("EFP07");
    static const QString EFP07e  ("EFP07e");
    static const QString EFP08e  ("EFP08e");
    static const QString MFS06e  ("MFS06e");
    static const QString MFS07e  ("MFS07e");
    static const QString SHFT02  ("SHFT02");
    static const QString SHFT02e ("SHFT02e");
    static const QString FGS02   ("FGS02");
    static const QString FGS03   ("FGS03");
    static const QString FGS03e  ("FGS03e");
    static const QString MFS06   ("MFS06");
    static const QString MFS07   ("MFS07");

    static const QString CSAMT ( "CSAMT" );
    static const QString MT    ( "MT" );

    static const QString DefaultChannelType( Ex );
    static const QString DefaultSensor( EFP06 );
    static const QString DefaultSerialNumber( Zero );

    static const unsigned int TXMMaxNumDipoles (8);

    static const unsigned int TXMNumElectrodePos (4);

    static const unsigned int aruiParallelChannels [10] = {5, 6, 7, 8, 9, 0, 1, 2, 3, 4};

    typedef std::vector<QString> VectorOfQStrings;

    const VectorOfQStrings& ChannelTypeVector();
    const VectorOfQStrings& SensorTypeVector();
    const VectorOfQStrings& SelectedInputVector();
    const VectorOfQStrings& MeasurementTypesVector();

    extern bool aBiggerB (const double& dA, const double& dB);
}

#include <string>

namespace BuildManifest 
{
#define evil_xstr(s) evil_str(s)
#define evil_str(s) #s

    static const std::string ApplicationName ( evil_xstr( APPLICATION_NAME ) );
    static const std::string BuildDate       ( __DATE__ );
    static const std::string BuildTime       ( __TIME__ );
    static       std::string VersionString      ( evil_xstr( VERSION_STRING ) );
    static const std::string CompilerVersionString ( __VERSION__ );

    static const std::string ProjectRoot         ( evil_xstr( PROJECT_ROOT ) );
    static const QString     ProjectRootAsQString ( evil_xstr( PROJECT_ROOT ) );
    
#undef evil_xstr
#undef evil_str
}

#endif /* _DEFAULT_VALUES_H_ */
