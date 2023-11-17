#include "codemapper.h"
#include <sstream>
#include <stdexcept>
#include "default_values.h"

namespace Tools {

    CodeMapper& CodeMapper::addMapping( const QString& code, const QString& visual )
    {
        if ( not ( ( mMapC2V.insert( Map::value_type( code , visual ) ).second ) and 
                   ( mMapV2C.insert( Map::value_type( visual , code ) ).second ) )
            ) {
            std::stringstream ss; 
            ss << __PRETTY_FUNCTION__ << " unable to add (" << code.toStdString() << " , " << visual.toStdString() << " ) mapping!";
            throw std::runtime_error( ss.str() );
        }
        return *this;
    }
    
    QString CodeMapper::toVisual ( const QString& code )
    {    
        Map::iterator it = mMapC2V.find( code );
        return it == mMapC2V.end() ? DefaultValues::Off : it -> second;
    }

    QString CodeMapper::toCode   ( const QString& visual)
    {
        Map::iterator it = mMapV2C.find( visual );
        return it == mMapV2C.end() ? DefaultValues::Off : it -> second;
    }
}    
