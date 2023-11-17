#include "hwdbqueryresult.h"

QString HWDBQueryResult::getName() const
{ 
    if ( not isVaild() )
        return "Invalid";
    
    Tools::DomElementWalker w( mHardwarePartElement );
    w.goToFirstChild( "Name" );
    return w.get().text();
    
}


