#include "default_values.h"

namespace DefaultValues {

    VectorOfQStrings initChannelTypeList() 
    {
        VectorOfQStrings result;

        result.push_back( Ex  );
        result.push_back( Ey  );
        result.push_back( Hx  );
        result.push_back( Hy  );
        result.push_back( Hz  );
        result.push_back( REx );
        result.push_back( REy );
        result.push_back( RHx );
        result.push_back( RHy );
        result.push_back( RHz );

        return result;
    }    

    const VectorOfQStrings& ChannelTypeVector() 
    {
        static VectorOfQStrings channelTypeVector = initChannelTypeList();

        return channelTypeVector;
    }

    VectorOfQStrings initSensorTypeList()
    {
        VectorOfQStrings result;

        result.push_back( EFP06  );
        result.push_back( EFP07  );
        result.push_back( EFP07e );
        result.push_back( EFP08e );
        result.push_back( MFS06e );
        result.push_back( MFS07e );
        result.push_back( SHFT02 );
        result.push_back( SHFT02e );
        result.push_back( FGS02  );
        result.push_back( FGS03  );
        result.push_back( FGS03e );
        result.push_back( MFS06  );
        result.push_back( MFS07  );
        
        return result;
    }

    const VectorOfQStrings& SensorTypeVector()
    {
        static VectorOfQStrings sensorTypeVector = initSensorTypeList();

        return sensorTypeVector;
    }

    VectorOfQStrings initSelectedInputVector()
    {
        VectorOfQStrings result;

        result.push_back( Off );
        result.push_back( Zero );
        result.push_back( One );
        
        return result;

    }

    const VectorOfQStrings& SelectedInputVector()
    {
        static VectorOfQStrings SelectedInputVector = initSelectedInputVector();

        return SelectedInputVector; 
    }

    VectorOfQStrings initMeasurementTypesVector()
    {
        VectorOfQStrings result;

        result.push_back( CSAMT );
        result.push_back( MT    );

        return result;

    }

    const VectorOfQStrings& MeasurementTypesVector()
    {
        static VectorOfQStrings MeasurementTypesVector = initMeasurementTypesVector();

        return MeasurementTypesVector;
    }
    

    bool aBiggerB (const double& dA, const double& dB)
    {
        return (dA < dB);
    }

}
