#ifndef _CONTROLDATA_H_
#define _CONTROLDATA_H_

namespace ControlData
{
    enum AdjustMode
    {
        AdjustIsOff = -1,
        AdjustAuto = 0, // carefull, we abuse this as int and use this for calculation
        Adjust1Min = 1,
        Adjust2Min = 2,
        Adjust3Min = 3,
        Adjust4Min = 4,
        Adjust5Min = 5
    };
    enum MeasurementMode
    {
        UndefindedMeasurementMode,
        NormalMeasurementMode,
        CSAMTMeasurementMode
    };
}

#endif /* _CONTROLDATA_H_ */
