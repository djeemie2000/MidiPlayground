#pragma once

#include "IntConversions.h"

typedef int(*IntOperator)(int);

template<int Scale>
int IntMaxSigned()
{
    return 1 << (Scale-1);
}

template<int Scale>
int IntMinSigned()
{
    return -1 << (Scale-1);
}

template<int Scale>
int IntSawUp(int Phase)
{
    return Phase;
}

template<int Scale>
int IntSawDown(int Phase)
{
    return -Phase;
}

template<int Scale>
int IntPulse(int Phase)
{
    return Phase<0 ? IntMinSigned<Scale>(): IntMaxSigned<Scale>();
}

template<int Scale>
int IntPseudoSinCalc(int Phase)
{
    return (4 * Phase * (IntMaxSigned<Scale>() - Phase) ) >> (Scale-1);
}

template<int Scale>
int IntFullPseudoSin(int Phase)
{
    return Phase<0 ? - IntPseudoSinCalc<Scale>(-Phase) : IntPseudoSinCalc<Scale>(Phase);
}

template<int Scale>
int IntSemiPseudoSin(int Phase)
{
    return IntUnipolarToBipolar<Scale>(
                IntPseudoSinCalc<Scale>(
                    IntBipolarToUnipolar<Scale>(Phase)));
}

template<int Scale>
int IntQuadraticCalc(int Phase)
{
    return (Phase * Phase) >> (Scale-1);
}

template<int Scale>
int IntSignMultiplier(int In)
{
    return 0<In ? 1 : -1;
}

template<int Scale>
int IntQuadratic(int Phase)
{
    return IntSignMultiplier<Scale>(Phase) * IntQuadraticCalc<Scale>(Phase);
}

template<int Scale>
int IntTriangle(int Phase)
{
  return Phase<0 ?  (1<<(Scale-1)) + (Phase<<1) : (1<<(Scale-1)) - (Phase<<1);
}
