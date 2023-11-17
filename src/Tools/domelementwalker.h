#ifndef DOMELEMENTWALKER_H
#define DOMELEMENTWALKER_H


#include <QDomDocument>
#include <QDebug>

#include <iostream>
#include <stdexcept>


namespace Tools {

    class DomElementSiblingIterator;

    class DomElementWalker
    {
        
    public:
        
        // init the walker object to the document element
        explicit DomElementWalker( QDomDocument& );
        
        explicit DomElementWalker( const DomElementWalker& other );

        // explicit DomElementWalker( QDomElement& );
        explicit DomElementWalker( const QDomElement& );
        
        explicit DomElementWalker( const DomElementSiblingIterator & );
        
        DomElementWalker& goToFirstChild( const QString& ); // throws if child does not exist!

        bool hasChild( const QString& ) const;
        
        DomElementWalker& goToFirstSiblingWithAttributeAndAttributeValue( const QString& , const QString& );
        
        std::string getCurrentElementNameStd() const { return mCurrentElement.tagName().toStdString(); }

        QString getCurrentElementName() const { return mCurrentElement.tagName(); }

        bool getAttribute( const QString& attrName, QString& attrValue ) const;

        DomElementWalker& assertElementName( const QString& );
        
        QDomElement get() const { return mCurrentElement; }

        void walkTo( QDomElement );

        void walkTo( DomElementSiblingIterator&  ) ;

        /**
         * \brief This function is used to return the number of child nodes with a given name.
         *
         * This function will return the number of child nodes that have the name qstrChildname.
         *
         * @param[in] const QString& qstrChildname = child node name
         * @param[out] unsigned int = number of child nodes with given name
         *
         * \author MWI
         * \date 2013-10-22
         */
        unsigned int getNumChilds (const QString& qstrChildname);

        /**
         * \brief This function is used to go to a list node that is defined by node name, attribute name and attribute value.
         *
         * This function will go to the next subnode that is identified by the node name,
         * attribute name and a  attribute value. If the attribute name is left blank (empty),
         * the attribute is ingnored and the first child node will be used.
         *
         * @param[in] const QString& qstrChildName = child node name
         * @param[in] const QString& qstrAttrName = attribute name
         * @param[in] const QString& qstrAttrValue = attribute value
         * @param[out] bool = false: node not found, true: node found
         *
         * \author MWI
         * \date 2013-10-22
         */
        bool goToChildNode (const QString& qstrChildName, const QString& qstrAttrName, const QString& qstrAttrValue);

        /**
         * \brief This function is used to go back to the parent node.
         *
         * -
         *
         * @param[out] bool = false: no parent node / true: walked to parent node
         *
         * \author MWI
         * \date 2013-10-22
         */
        bool goToParent (void);

        /**
         * \brief This function is used to remove a child node.
         *
         * This function will search for the child node with given name, attribute and attribute values
         * and remove it from the parent node.
         *
         * @param[in] const QString& qstrChildName = child node name
         * @param[in] const QString& qstrAttrName = attribute name
         * @param[in] const QString& qstrAttrValue = attribute value
         * @param[out] bool = false: node not found, true: node found
         *
         * \author MWI
         * \date 2013-10-22
         */
        bool removeChildNode (const QString& qstrChildName, const QString& qstrAttrName, const QString& qstrAttrValue);

        /**
         * \brief This function is used to add a new child node.
         *
         * This function is used to add a new child node to the current node, using the given
         * node name, attribute, attribute value and node valu.
         * If the attribute name is an empty string, no attribute will be added.
         *
         * @param[in] const QString& qstrChildName = child node name
         * @param[in] const QString& qstrChildValue = child node value
         * @param[in] const QString& qstrAttrName = attribute name
         * @param[in] const QString& qstrAttrValue = attribute value
         * @param[out] bool = false: node not found, true: node found
         *
         * \author MWI
         * \date 2013-10-22
         */
        bool addChild (const QString& qstrChildName, const QString& qstrChildValue, const QString& qstrAttrName, const QString& qstrAttrValue);

    protected:
        
        QDomElement  mCurrentElement;

        QDomDocument* pclDoc;
    
    };

    class  DomElementSiblingIterator : protected DomElementWalker
    {
    public:

        std::string getCurrentElementNameStd() const { return DomElementWalker::getCurrentElementNameStd(); }
        QString getCurrentElementName() const        { return DomElementWalker::getCurrentElementName();    }

        DomElementSiblingIterator( const DomElementWalker& other );

        bool isValid() const;

        QDomElement get() const { return DomElementWalker::get(); }

        void next();

        void assertElementName( const QString& name ) { DomElementWalker::assertElementName( name ); } 


    };


}

#endif // DOMELEMENTWALKER_H
