#ifndef _XML_DOM_HELPER_H_
#define _XML_DOM_HELPER_H_

#include <QDomElement>
#include <QTime>
#include <QDate>
#include <QString>

namespace Tools {

    void updateDomTimeElement( QDomElement timeElement, const QTime& newTime );
    void updateDomDateElement( QDomElement dateElement, const QDate& newDate );
    void updateDomUIntElement( QDomElement uintElement , unsigned int newUInt );
    void updateDomTextElement( QDomElement textElement, const QString& value );
    void createOrUpdateChildElement( QDomDocument& doc, QDomElement& parent,  const QString& tagName , const QString& tagValue );
    QDomElement createTextElement( QDomDocument& doc, const QString& tagName , const QString& tagValue );

}

#endif /* _XML_DOM_HELPER_H_ */
