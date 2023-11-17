#include "xml_dom_helper.h"
#include "default_values.h"

#include <QDebug>
namespace Tools
{
    void updateDomTimeElement( QDomElement timeElement, const QTime& newTime )
    {
        timeElement.normalize();
        QDomText text = timeElement.firstChild().toText();
        text.setData( newTime.toString( DefaultValues::TimeFormat ) );
    }

    void updateDomDateElement( QDomElement dateElement, const QDate& newDate )
    {
        dateElement.normalize();
        QDomText text = dateElement.firstChild().toText();
        text.setData( newDate.toString( DefaultValues::DateFormat ) );
    }

    void updateDomUIntElement( QDomElement uintElement , unsigned int newUInt )
    {
        uintElement.normalize();
        QDomText text = uintElement.firstChild().toText();
        text.setData( QString::number( newUInt ) );
    }

    void updateDomTextElement( QDomElement textElement, const QString& value )
    {
        if (textElement.hasChildNodes() == false)
        {
            QDomText clNewTextNode = textElement.toDocument().createTextNode(value);
            textElement.appendChild(clNewTextNode);
        }

        textElement.normalize();
        QDomText text = textElement.firstChild().toText();
        text.setData( value );        
    }

    QDomElement createTextElement( QDomDocument& doc, const QString& tagName , const QString& tagValue )
    {
        QDomElement newOuterElem = doc.createElement( tagName );
        QDomText    contentNode  = doc.createTextNode( tagValue );
        newOuterElem.appendChild( contentNode );
        return newOuterElem;
    }

    void createOrUpdateChildElement( QDomDocument& doc, QDomElement& parent,  const QString& tagName , const QString& tagValue )
    {
        QDomElement child = parent.firstChildElement( tagName );
        if ( child.isNull() ) {
            child = createTextElement( doc, tagName, tagValue );
            parent.appendChild( child );
            return;
        }

        QDomElement newChild = createTextElement( doc, tagName, tagValue );
        parent.replaceChild( newChild, child );

    }
    
}
