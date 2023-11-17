#include "tools_types.h"

#include <QRegExp>


// global variables
QSettings* pclSettings = NULL;


// static method, some sort of friendly factory
Types::TimeInterval Types::TimeInterval::parseIntervalString( const QString& durationString )
{

    QRegExp regEx("^\\s*([0-9]{1,4})\\s*:\\s*([0-9]{1,2})\\s*:\\s*([0-9]{1,2})\\s*");

    if ( not regEx.exactMatch( durationString ) )
        throw std::runtime_error( std::string("\"").append( durationString.toStdString() ).append( "\" not parsable!" ) );

    int hour   = regEx.cap( 1 ).toInt();
    int minute = regEx.cap( 2 ).toInt();
    int second = regEx.cap( 3 ).toInt();

    Types::TimeInterval result;

    result
        .addHours( hour )
        .addMinutes( minute )
        .addSeconds( second );

    return result;
}

