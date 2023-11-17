#ifndef _NAMEMAPPER_H_
#define _NAMEMAPPER_H_

#include <map>
#include <QString>

namespace Tools {

    class CodeMapper
    {
    private:
        
        typedef std::map<QString,QString> Map;
        Map mMapC2V;
        Map mMapV2C;
        
    public:
        
        CodeMapper& addMapping( const QString& code, const QString& visual ); // throws std::runtime_error
        
        QString toVisual( const QString& );
        QString toCode( const QString& );
        
    };

}

#endif /* _NAMEMAPPER_H_ */
