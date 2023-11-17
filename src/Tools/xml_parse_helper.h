#ifndef _XML_PARSE_HELPER_H_
#define _XML_PARSE_HELPER_H_

#include <QStringList>
#include <QString>
#include <QDomElement>

namespace Tools {

    QStringList splitStringToStringList( const QString& s );

    void parseFilterTypeTag( QStringList& rfFilterList, QStringList& lpHpFilterList, QString filterDefinitionString );

    void parseOptionalChildInto( const QDomElement& parentElement , const QString& tagName, QString& result );

}

#endif /* _XML_PARSE_HELPER_H_ */
