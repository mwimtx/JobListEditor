#ifndef _HWDBQUERYRESULT_H_
#define _HWDBQUERYRESULT_H_

#include "hwkeydata.h"
#include "domelementwalker.h"
#include <QDomElement>
#include <QString>
#include <QSharedPointer>

////////////////////////////////////////////////////////////////////////////////
//
// instance for a query result

class HWDBQueryResult
{
private:

    HWDBKeyData mKeyData;
    
    QDomElement mHardwarePartElement;

public:

    typedef QSharedPointer<HWDBQueryResult> Spt;

    HWDBQueryResult( const HWDBKeyData& keyData ):
        mKeyData( keyData )
        {}

    inline bool     isVaild() const { return not mHardwarePartElement.isNull(); } // we need to alter this, later.

    inline unsigned getGSM () const     { return mKeyData.getGSM();     }
    inline unsigned getType () const    { return mKeyData.getType();    }
    inline unsigned getRevMain () const { return mKeyData.getRevMain(); }
    inline unsigned getRevSub () const  { return mKeyData.getRevSub();  }

    inline bool     hasId() const       { return mKeyData.hasId(); }
    inline unsigned getId() const       { return mKeyData.getId(); }

    inline void setResultDomElement( QDomElement element ) { mHardwarePartElement = element; }

    QDomElement getDomElement() const { return mHardwarePartElement; }

    QString getName() const;

    inline std::string toStdString() const {

        std::stringstream ss;
        // more to follow...
        ss << "is valid: " << std::boolalpha << isVaild() << " " << mKeyData.toStdString() << " Name : " << getName().toStdString();
        return ss.str();

    }
                        
};

#endif /* _HWDBQUERYRESULT_H_ */
