#include "cli_parser.h"

#include <QDebug>
#include <string>
#include <stdexcept>
#include <sstream>

namespace Tools 
{

    CLIParser& CLIParser::addOption( const QString& optionName , const QString& defaultValue )
    {
        mOptionMap.insert( OptionMapType::value_type( optionName, defaultValue ) );
        return *this;
    }


    QString CLIParser::getValue( const QString& optionName ) const 
    {
        OptionMapType::const_iterator it = mOptionMap.find( optionName );
        if ( it == mOptionMap.end() )
            throw std::runtime_error( std::string("no such option name : ").append( optionName.toStdString() ) );
        return it -> second;
    }


    void CLIParser::parse( QStringList args ) // throws std::runtime_error
    {
        mIsValid = false;
        int i = 1;
        while ( i < args.size() )
        { 
            OptionMapType::iterator it = mOptionMap.find( args[i] );
            if ( it == mOptionMap.end() ) {
                qDebug()  << "Unkown option : " << args[i] << " abort.";
                break;
            }
            mOptionMap.erase( it );
            mOptionMap.insert( OptionMapType::value_type( args[i], args[i+1] ) );
            i += 2;
        }
        mIsValid = true;
    }

    std::string CLIParser::toStdString() const
    {
        std::stringstream ss;
        for ( OptionMapType::const_iterator it = mOptionMap.begin();
              it != mOptionMap.end();
              ++it )
            ss << "[" << (*it).first.toStdString() << "] --> [" << (*it).second.toStdString() << "]\n";
                  
        return ss.str();
            
    }


}
