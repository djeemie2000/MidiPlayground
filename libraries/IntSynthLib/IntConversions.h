#pragma once

namespace isl
{

// [0,1] to [-1,1]  or [0, 2^N] to [-2^N, 2^N]
template<int Scale>
int IntUnipolarToBipolar(int In)
{
    return (In<<1) - (1<<(Scale-1));
}

// [-1,1] to [0,2] or [-2^N, 2^N] to [0, 2^N+1]
template<int Scale>
int IntBipolarToUnsigned(int In)
{
    return (In + (1<<(Scale-1)));
}

// [-1,1] to [0,1] or [-2^N, 2^N] to [0, 2^N]
template<int Scale>
int IntBipolarToUnipolar(int In)
{
    return (In + (1<<(Scale-1)))>>1;
}

// [-2^N, 2^N] to [-1,+1]
template<class IntegerType, class FloatType, int Scale>
FloatType IntBipolarToFloatBipolar(IntegerType In)
{
    return In * FloatType(1) / (1<<(Scale-1));
}

// [-1,+1] to [-2^N, 2^N]
template<class IntegerType, class FloatType, int Scale>
IntegerType FloatBipolarToIntBipolar(FloatType In)
{
    return In * (1<<(Scale-1));
}

}//namespace isl
