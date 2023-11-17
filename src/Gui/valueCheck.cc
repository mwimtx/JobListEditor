#ifndef _VALUECHECK_H_
#define _VALUECHECK_H_

#include <cmath>

QString doValueCheck( const QString& minValue , const QString& maxValue, const QString& currentInput , bool floatNumberMode = false )
{
    QString resultString = "";

    {
        int minV = minValue.toInt();
        int maxV = maxValue.toInt();
        int ci = currentInput.toInt();
        int valueToCheck = maxV;
        if ( ci < 0 ) {
            valueToCheck = std::abs( minV );
            ci = std::abs( ci );
        }
        int rest = valueToCheck - ( ci * 10 );
        for ( int i = 0 ; ( i <= 9 ) and ( i <= rest ) ; ++ i )
            resultString += QString::number( i );

        if ( resultString.compare("0") == 0 )
            resultString = "";
    }

    if ( not floatNumberMode )
        return resultString;

    // Post dot part starts here...
    int dotCount = currentInput.count( '.' );
    if ( dotCount > 1 )
        return QString(""); // invalid float input, more than one "."
    
    bool haveDot = dotCount == 1 ;

    if ( not ( haveDot ) and ( resultString.compare( "0123456789" ) == 0 ) ) // ok, there is room for things > 1, so there should be room for thing < 1.
        return QString(".") + resultString;
    
    double maxV = maxValue.toDouble();
    double minV = minValue.toDouble();
    double ci = currentInput.toDouble();
    double valueToCheck = maxV;
    if ( ci < 0 ) {
        valueToCheck = std::abs( minV );
        ci = std::abs( ci );
    }

    double delta = valueToCheck - ci;
    
    if ( not haveDot and ( delta > 0 ) )
        return QString (".");

    if ( not haveDot and ( not ( delta > 0 ) ) ) // no, we wont start epsilon stuff here...
        return QString ("");

    if ( haveDot and ( delta >= 1 ) )
        return QString("0123456789" );

    if ( haveDot and ( delta > 0 ) and ( delta < 1 ) ) {
        resultString = "";
        double dDiggit = delta;
        while ( dDiggit < 1 )
            dDiggit = dDiggit * 10;
        int iDiggit = ::truncf( dDiggit );
        if ( iDiggit == 1 )
            iDiggit = iDiggit * 10;
        for ( int i = 0 ; ( i <= 9 ) and ( i < iDiggit ) ; ++ i )
            resultString += QString::number( i );
        return resultString;
    }

    // when we get here, we reached the moral equivalent to delta == 0.0
    return QString("" );

}

#endif /* _VALUECHECK_H_ */

