#pragma once


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

// [-1,1] to [0,1] or [-2^N, 2^N] to [0, 2^N]
template<int Scale>
int IntBipolarToUnipolar(int In)
{
    return (In + (1<<(Scale-1)))>>1;
}

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
