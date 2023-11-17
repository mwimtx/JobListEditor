#ifndef _HWKEYDATA_H_
#define _HWKEYDATA_H_

////////////////////////////////////////////////////////////////////////////////
//
// key data for querying  the hw database 

#include <sstream>

class HWDBKeyData 
{
private:

    unsigned mGSM;
    unsigned mType;
    unsigned mRevMain;
    unsigned mRevSub;

    bool     mHasId;
    unsigned mId;

public:

    HWDBKeyData() :
        mHasId( false )
        {}

    inline HWDBKeyData& setGSM     ( const unsigned& value ) { mGSM     = value; return *this; }
    inline HWDBKeyData& setType    ( const unsigned& value ) { mType    = value; return *this; }
    inline HWDBKeyData& setRevMain ( const unsigned& value ) { mRevMain = value; return *this; }
    inline HWDBKeyData& setRevSub  ( const unsigned& value ) { mRevSub  = value; return *this; }
    inline HWDBKeyData& setId      ( const unsigned& value ) { mId      = value; mHasId = true; return *this; }

    inline unsigned   getGSM () const     { return mGSM;     }
    inline unsigned   getType () const    { return mType;    }
    inline unsigned   getRevMain () const { return mRevMain; }
    inline unsigned   getRevSub () const  { return mRevSub;  }

    inline bool hasId()     const { return mHasId; }
    inline unsigned getId() const { return mId;    }

    inline std::string toStdString() const {
        std::stringstream ss;
        ss << "GSM: " << getGSM() << " Type: " << getType() << " RevMain: " << getRevMain() << " RevSub: " << getRevSub();
        return ss.str();
    }

};


#endif /* _HWKEYDATA_H_ */
