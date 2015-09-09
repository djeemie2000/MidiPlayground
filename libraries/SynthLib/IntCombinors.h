#pragma once

#include <cmath>
#include "IntOperators.h"


typedef int(*IntCombinor)(int, int);


template<class T, int Scale>
T HardLimitBipolar(T In)
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
T IntHardLimitAdd(T In1, T In2) const
{
    return IntHardLimitBipolar<Scale>(In1 + In2);// hard limited to +/- 2^Scale-1
}

template<class T, int Scale>
T IntMult(T In1, T In2) const
{
    // this in fact is amplitude modulation
    // as opposed to ring modulation below
    return ( In1 * In2 ) >> (Scale-1);//assumes signed integer in [-2^Scale-1, 2^Scale-1]
}

template<class T, int Scale>
T IntMultUnipolar()(T In1, T In2) const
{
    return IntUnipolarToBipolar<Scale>( IntBipolarToUnipolar<Scale>(In1) * IntBipolarToUnipolar<Scale>(In2) );
}

template<class T, int Scale>
T IntMultFirst(T In1, T In2) const
{
    return ( In1 * abs(In2) ) >> (Scale-1);
}

template<class T, int Scale>
T IntMultSecond(T In1, T In2) const
{
    return ( abs(In1) * In2 ) >> (Scale-1);
}

template<class T, int Scale>
T IntRingModFirst(T In1, T In2) const
{
    // In2 is converted from [-1, 1] to [0,1] and serves as amplitude modulator of In1
    //return In1 * (1 + In2) / 2;
    return ( In1 * IntBipolarToUnipolar<Scale>(In2) ) >> (Scale-1);
}

template<class T, int Scale>
T IntRingModSecond()(T In1, T In2) const
{
    // In1 is converted from [-1, 1] to [0,1] and serves as amplitude modulator of In2
    //return In2 * (1 + In1) / 2;
    return (In2 * IntBipolarToUnipolar<Scale>(In1)) >> (Scale-1);
}

template<class T, int Scale>
T IntHardLimitDiff(T In1, T In2) const
{
    return HardLimitBipolar(In1 - In2);
}

template<class T, int Scale>
T IntDiffAbs(T In1, T In2) const
{
    return abs(In1) - abs(In2);
}


template<class T, int Scale>
T IntMax(T In1, T In2) const
{
    return max(In1, In2);
}

template<class T, int Scale>
T IntMaxAbs()(T In1, T In2) const
{
    // return the value with the biggest abs
    return abs(In1) < abs(In2) ? In2 : In1;
}

template<class T, int Scale>
T IntMinAbs()(T In1, T In2) const
{
    // return the value with the smallest abs
    return abs(In1) < abs(In2) ? In1 : In2;
}

template<class T, int Scale>
T IntMin(T In1, T In2) const
{
    return min(In1, In2);
}

template<class T, int Scale>
T IntPosNegAdd()(T In1, T In2) const
{
    // add the positive part of In1 and the negative part of In2
    return max(In1,static_cast<T>(0)) + min(In2, static_cast<T>(0));
}

template<class T, int Scale>
T IntNegPosAdd()(T In1, T In2) const
{
    // add the positive part of In2 and the negative part of In1
    return min(In1,static_cast<T>(0)) + max(In2, static_cast<T>(0));
}

template<class T, int Scale>
T IntMultMod1(T In1, T In2) const
{
    // return In1*(1+In2);
    return ( In1 * ( (1<<(Scale-1)) + In2 ) ) >> (Scale-1);
}

template<class T, int Scale>
T IntMultMod2()(T In1, T In2) const
{
    //return In2*(1+In1);
    return ( In2 * ( (1<<(Scale-1)) + In1 ) ) >> (Scale-1);
}
