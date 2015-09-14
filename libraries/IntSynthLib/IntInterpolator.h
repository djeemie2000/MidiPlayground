#pragma once

namespace isl
{

template<int SizeScale, int IndexScale = 16>
class CIntegerInterpolator
{
public:
    static const int Rescale = IndexScale-SizeScale;
    static const uint32_t WeightMask = 0xFFFFFFFF>>(32-IndexScale+SizeScale);//32 is # bits in uint32_t
    static const uint32_t MaxWeight = 1<<(IndexScale-SizeScale);

    CIntegerInterpolator(){}

    int GetInterpolatedValue(const int* Values, uint32_t ScaledIndex)
    {
        int Index = ScaledIndex>>Rescale;

        int WeightB = ScaledIndex & WeightMask;
        int WeightA = MaxWeight - WeightB;

        int Val =  ( WeightA * Values[Index] + WeightB * Values[Index+1] ) >> Rescale;
    //    int Val =  ( (Values[Index]<<(IndexScale-SizeScale)) + (WeightB * ( Values[Index+1] - Values[Index])) ) >> (IndexScale-SizeScale);
        return Val;
    }
};

}
