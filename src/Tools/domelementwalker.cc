#include "domelementwalker.h"

#include <sstream>

namespace Tools {

    DomElementWalker::DomElementWalker( QDomDocument& doc ) :
        mCurrentElement (doc.documentElement()),
        pclDoc(&doc)
    {
    }

    DomElementWalker::DomElementWalker( const DomElementWalker& other ) :
        mCurrentElement( other.mCurrentElement )
    {
        pclDoc = NULL;
    }

    DomElementWalker::DomElementWalker( const DomElementSiblingIterator & other ) :
        mCurrentElement( other.get() )
    {
        pclDoc = NULL;
    }


    DomElementWalker::DomElementWalker( const QDomElement& element ) :
        mCurrentElement( element )
    {
        pclDoc = NULL;
    }


// throws if child does not exist!
    DomElementWalker& DomElementWalker::goToFirstChild( const QString& childElementName )
    {
        QDomElement newCurrentElement = mCurrentElement.firstChildElement( childElementName );
        if ( newCurrentElement.isNull() )
            throw std::runtime_error( QString( "DomElementWalker: try to go to nonexistant child : \"%1\"" ).arg( childElementName ).toStdString() );
        mCurrentElement = newCurrentElement;

        return *this;
    }


    bool DomElementWalker::goToParent (void)
    {
        // declaration of variables
        bool bRetValue = true;

        if (mCurrentElement.parentNode().isNull() == true)
        {
            bRetValue = false;
        }
        else
        {
            mCurrentElement = mCurrentElement.parentNode().toElement();
        }

        return (bRetValue);
    }


    bool DomElementWalker::goToChildNode (const QString& qstrChildName, const QString& qstrAttrName, const QString& qstrAttrValue)
    {
        // declaration of variables
        bool             bRetValue = true;
        unsigned int     uiCounter;
        QDomNodeList     clNodeList;
        QDomNamedNodeMap clNodeMap;

        if (qstrAttrName.size() <= 0)
        {
            (void) this->goToFirstChild(qstrChildName);
        }
        else
        {
            clNodeList = mCurrentElement.elementsByTagName(qstrChildName);
            if (clNodeList.length() <= 0)
            {
                bRetValue = false;
            }
            else
            {
                for (uiCounter = 0; ((int)uiCounter) < clNodeList.length(); uiCounter++)
                {
                    clNodeMap = clNodeList.at(uiCounter).attributes();
                    if (clNodeMap.contains(qstrAttrName) == true)
                    {
                        if (clNodeMap.namedItem(qstrAttrName).nodeValue() == qstrAttrValue)
                        {
                            mCurrentElement = clNodeList.at(uiCounter).toElement();
                        }
                    }
                }
            }
        }

        return (bRetValue);
    }


    bool DomElementWalker::removeChildNode (const QString& qstrChildName, const QString& qstrAttrName, const QString& qstrAttrValue)
    {
        // declaration of variables
        bool bRetValue = true;

        if (this->goToChildNode(qstrChildName, qstrAttrName, qstrAttrValue) == false)
        {
            bRetValue = false;
        }
        else
        {
            // save current element node and go up to parent node
            QDomElement clNodeToDelete = this->mCurrentElement;
            if (this->goToParent() == false)
            {
                bRetValue = false;
            }
            else
            {
                this->mCurrentElement.removeChild(clNodeToDelete);
            }
        }

        return (bRetValue);
    }


    bool DomElementWalker::addChild (const QString& qstrChildName, const QString& qstrChildValue, const QString& qstrAttrName, const QString& qstrAttrValue)
    {
        // declaration of variables
        bool bRetValue = true;

        if (pclDoc == NULL)
        {
            bRetValue = false;
        }
        else
        {

            QDomElement clNewNode = pclDoc->createElement(qstrChildName);
            clNewNode.setAttribute(qstrAttrName, qstrAttrValue.toUInt());

            QDomText clDataNode = pclDoc->createTextNode(qstrChildValue);

            clNewNode.appendChild(clDataNode);

            this->mCurrentElement.appendChild(clNewNode);
        }

        return (bRetValue);
    }


    bool DomElementWalker::hasChild( const QString& childElementName ) const
    {
        return not ( mCurrentElement.firstChildElement( childElementName ).isNull() );
    }


    unsigned int DomElementWalker::getNumChilds (const QString& qstrChildname)
    {
        // declaration of variables

        QDomNodeList clNodeList = mCurrentElement.elementsByTagName(qstrChildname);

        return (clNodeList.length());
    }



    DomElementWalker& DomElementWalker::goToFirstSiblingWithAttributeAndAttributeValue( const QString& attribute , const QString& value )
    {
        #ifdef DEBUG_OUTPUT
        std::cout << "DomElementWalker sits on [" << getCurrentElementNameStd() 
                  << "] try to go to sibling or self with attribute[" << attribute.toStdString() 
                  << "] and attribute value [" << value.toStdString() << "]" << std::endl;
        #endif

        QString currentElementName = mCurrentElement.tagName();
        QDomElement resultElement;
        for ( QDomElement tmp = mCurrentElement; not( tmp.isNull() ) ; tmp = tmp.nextSiblingElement( currentElementName ) ) {
            // std::cout << "tmp.tagName() " << tmp.tagName().toStdString() << std::endl;
            QDomNamedNodeMap attrMap = tmp.attributes();
            QDomNode node = attrMap.namedItem( attribute );
            if ( not node.isAttr() ) 
                continue; // mus tnop happen, anyway...

            QDomAttr attr = node.toAttr();
            // std::cout << "attr.name() " << attr.name().toStdString() << " value " << attr.value().toStdString() << std::endl;
            if ( value.compare( attr.value() ) == 0 ) {
                resultElement = tmp;
                break;
            }
        }

        if ( resultElement.isNull() )
            throw std::runtime_error( QString( "DomElementWalker: try to go find nonexistant attribute value pair : %1 %2 " ).arg( attribute ).arg( value ).toStdString() );
        mCurrentElement = resultElement;

        #ifdef DEBUG_OUTPUT
        std::cout  << "DomElementWalker now sits on [" << getCurrentElementNameStd()  << "]"
                   << " with attribute [" << attribute.toStdString() << "]"
                   << " and attribute value [" << value.toStdString() << "]"
                   << std::endl;
        #endif

        return *this;
    }

    bool DomElementWalker::getAttribute( const QString& attrName, QString& attrValue ) const
    {
        attrValue = mCurrentElement.attribute( attrName );
        return ( not attrValue.isNull() );
    }


    DomElementWalker& DomElementWalker::assertElementName( const QString& elemName ) {
     
        if ( not mCurrentElement.tagName().compare( elemName ) == 0 ) 
        {
            std::stringstream ss;
            ss << "element name assertion failed: expected [" << elemName.toStdString() << "]"
               << " walker object sits on [" << getCurrentElementNameStd() << "]";
            throw std::runtime_error( ss.str() );
        }
        return *this;
    }

    void DomElementWalker::walkTo( QDomElement gotoElement ) 
    { 
        mCurrentElement = gotoElement; 
    }
    
    void DomElementWalker::walkTo( DomElementSiblingIterator& iterator ) 
    { 
        mCurrentElement = iterator.get(); 
    }


    ////////////////////////////////////////////////////////////////////////////////
    //
    // DomElementSiblingIterator

    DomElementSiblingIterator::DomElementSiblingIterator( const DomElementWalker& other ) :
        DomElementWalker( other )
    {}

    bool DomElementSiblingIterator::isValid() const 
    {
        return not mCurrentElement.isNull();
    }

    void DomElementSiblingIterator::next() 
    {
        if ( mCurrentElement.isNull() )
            return;

        QDomNode nextSibling = mCurrentElement.nextSiblingElement( mCurrentElement.tagName() );

        mCurrentElement = nextSibling.toElement(); // Converts a QDomNode into a QDomElement. If the node is not an element the returned object will be null.

    }

}
