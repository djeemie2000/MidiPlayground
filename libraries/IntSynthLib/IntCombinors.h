#pragma once

#include <cmath>
#include "IntOperators.h"

namespace isl
{

typedef int(*IntCombinor)(int, int);


template<class T, int Scale>
T IntHardLimitBipolar(T In)
{
    if(In<IntMinSigned<Scale>())
    {
        return IntMinSigned<Scale>();
    }
    if(IntMaxSigned<Scale>()<=In)
    {
        return IntMaxSigned<Scale>()-1;
    }
    return In;
}

template<class T, int Scale>
T IntHardLimitAdd(T In1, T In2)
{
    return IntHardLimitBipolar<T, Scale>(In1 + In2);// hard limited to +/- 2^Scale-1
}

template<class T, int Scale>
T IntMult(T In1, T In2)
{
    // this in fact is amplitude modulation
    // as opposed to ring modulation below
    return ( In1 * In2 ) >> (Scale-1);//assumes signed integer in [-2^Scale-1, 2^Scale-1]
}

template<class T, int Scale>
T IntMultUnipolar(T In1, T In2)
{
    return IntUnipolarToBipolar<Scale>( IntBipolarToUnipolar<Scale>(In1) * IntBipolarToUnipolar<Scale>(In2) );
}

template<class T, int Scale>
T IntMultFirst(T In1, T In2)
{
    return ( In1 * abs(In2) ) >> (Scale-1);
}

template<class T, int Scale>
T IntMultSecond(T In1, T In2)
{
    return ( abs(In1) * In2 ) >> (Scale-1);
}

template<class T, int Scale>
T IntRingModFirst(T In1, T In2)
{
    // In2 is converted from [-1, 1] to [0,1] and serves as amplitude modulator of In1
    //return In1 * (1 + In2) / 2;
    return ( In1 * IntBipolarToUnipolar<Scale>(In2) ) >> (Scale-1);
}

template<class T, int Scale>
T IntRingModSecond(T In1, T In2)
{
    // In1 is converted from [-1, 1] to [0,1] and serves as amplitude modulator of In2
    //return In2 * (1 + In1) / 2;
    return (In2 * IntBipolarToUnipolar<Scale>(In1)) >> (Scale-1);
}

template<class T, int Scale>
T IntHardLimitDiff(T In1, T In2)
{
    return IntHardLimitBipolar<T, Scale>(In1 - In2);
}

template<class T, int Scale>
T IntDiffAbs(T In1, T In2)
{
    return abs(In1) - abs(In2);
}


template<class T, int Scale>
T IntMax(T In1, T In2)
{
    return max(In1, In2);
}

template<class T, int Scale>
T IntMaxAbs(T In1, T In2)
{
    // return the value with the biggest abs
    return abs(In1) < abs(In2) ? In2 : In1;
}

template<class T, int Scale>
T IntMinAbs(T In1, T In2)
{
    // return the value with the smallest abs
    return abs(In1) < abs(In2) ? In1 : In2;
}

template<class T, int Scale>
T IntMin(T In1, T In2)
{
    return min(In1, In2);
}

template<class T, int Scale>
T IntPosNegAdd(T In1, T In2)
{
    // add the positive part of In1 and the negative part of In2
    return max(In1,static_cast<T>(0)) + min(In2, static_cast<T>(0));
}

template<class T, int Scale>
T IntNegPosAdd(T In1, T In2)
{
    // add the positive part of In2 and the negative part of In1
    return min(In1,static_cast<T>(0)) + max(In2, static_cast<T>(0));
}

template<class T, int Scale>
T IntMultMod1(T In1, T In2)
{
    // return In1*(1+In2);
    return ( In1 * ( (1<<(Scale-1)) + In2 ) ) >> (Scale-1);
}

template<class T, int Scale>
T IntMultMod2(T In1, T In2)
{
    //return In2*(1+In1);
    return ( In2 * ( (1<<(Scale-1)) + In1 ) ) >> (Scale-1);
}

}//namespace isl
