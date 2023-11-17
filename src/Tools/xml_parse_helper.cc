#include "xml_parse_helper.h"

#include <stdexcept>

#include <QDebug>

namespace Tools {

    QStringList splitStringToStringList( const QString& s )
    {
        QStringList tmp = s.trimmed().split(",", QString::SkipEmptyParts);
        QStringList result;
        for ( int i = 0; i < tmp.size() ; ++i )
        {
            QString filterName = tmp[ i ].trimmed();
            if ( not filterName.isEmpty() )
            {
                result.append( filterName );
            }
        }
        return result;
    }
    

    void parseFilterTypeTag( QStringList& rfFilterList, QStringList& lpHpFilterList, QString filterDefinitionString )
    {
        QStringList filterList = splitStringToStringList( filterDefinitionString );
        for (int i = 0; i < filterList.size(); ++i)
        {
            QString filterName = filterList[ i ];
            QStringList nameParts = filterName.trimmed().split("_", QString::SkipEmptyParts);
            if (nameParts.size() != 3)
            {
                throw std::runtime_error( std::string( "filtername has not the expected format: " ).append( filterName.toStdString() ) );
            }

            QString filterGrounpKey = nameParts[ 1 ];
            
            if ( filterGrounpKey.compare( "RF" ) == 0 ) {
                rfFilterList.append( filterName );
                continue;
            }
            
            if ( ( filterGrounpKey.compare( "LP" ) == 0 ) or  
                 ( filterGrounpKey.compare( "HP" ) == 0 ) ) {
                lpHpFilterList.append( filterName );
                continue;
            }
            
            throw std::runtime_error( std::string( "unable to parse Filters-tag : Unkown filtergroup key : " ).append( filterName.toStdString() ) );
        }
    }

    void parseOptionalChildInto( const QDomElement& parentElement , const QString& tagName, QString& result )
    {
        result = "";
        QDomElement childElement = parentElement.firstChildElement( tagName );
        if ( childElement.isNull() )
            return;
        childElement.normalize();
        result = childElement.text();
    }


} // namespace
