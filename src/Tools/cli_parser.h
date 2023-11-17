#ifndef _CLI_PARSER_H_
#define _CLI_PARSER_H_

#include <QStringList>
#include <map>

namespace Tools {

    // args are hardcoded for this application
    class CLIParser
    {
    public:

        explicit CLIParser() :
            mIsValid( false )
            {}

        CLIParser& addOption( const QString& , const QString& defaultValue = QString() );

        void parse( QStringList args ); // throws std::runtime_error

        QString getValue( const QString& optionName ) const ;
        
        std::string toStdString() const;

        bool isValid() const { return mIsValid; }

    private:

        typedef std::map<QString,QString> OptionMapType;

        OptionMapType mOptionMap;

        bool mIsValid;

    };
}



#endif /* _CLI_PARSER_H_ */
