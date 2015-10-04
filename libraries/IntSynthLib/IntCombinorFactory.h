#pragma once

//#include <vector>
//#include <string>
#include "IntCombinors.h"

namespace isl
{

template<int Scale>
class CCombinorFactory
{
public:
    static const int NumCombinors = 21;

    static IntCombinor GetOperator(int Selected)
    {
        if(Selected == 0)
        {
            return IntAdd<int, Scale>;// +
        }
        else if(Selected == 1)
        {
            return IntHardLimitAdd<int, Scale>;// +L
        }
        else if(Selected == 2)
        {
            return IntMultFirst<int, Scale>;// *A
        }
        else if(Selected == 3)
        {
            return IntMultSecond<int, Scale>;// *B
        }
        else if(Selected == 4)
        {
            return IntMult<int, Scale>;// *
        }
        else if(Selected == 5)
        {
            return IntRingModFirst<int, Scale>;// RA
        }
        else if(Selected == 6)
        {
            return IntRingModSecond<int, Scale>;// RB
        }
        else if(Selected == 7)
        {
            return IntMultUnipolar<int, Scale>;// *U
        }
        else if(Selected == 8)
        {
            return IntHardLimitDiff<int, Scale>;// -L
        }
        else if(Selected == 9)
        {
            return IntDiffAbs<int, Scale>;// |A|-|B|
        }
        else if(Selected == 10)
        {
            return IntMaxAbs<int, Scale>;// M||
        }
        else if(Selected == 11)
        {
            return IntMinAbs<int, Scale>;// m||
        }
        else if(Selected == 12)
        {
            return IntMax<int, Scale>;// M
        }
        else if(Selected == 13)
        {
            return IntMin<int, Scale>;// m
        }
        else if(Selected == 14)
        {
            return IntPosNegAdd<int, Scale>;// P + N
        }
        else if(Selected == 15)
        {
            return IntNegPosAdd<int, Scale>;// N + P
        }
        else if(Selected == 16)
        {
            return IntMultMod1<int, Scale>;// A(1+B)
        }
        else if(Selected == 17)
        {
            return IntMultMod2<int, Scale>;// B(1+A)
        }
        else if(Selected == 18)
        {
            return IntMultAbs<int, Scale>;// |A|*|B|
        }
        else if(Selected == 19)
        {
            return IntMinAbsFirst<int, Scale>;// m|A|B
        }
        else if(Selected == 20)
        {
            return IntMinAbsSecond<int, Scale>;// mA|B|
        }
        //default
        return IntAdd<int, Scale>;
    }

    //static std::vector<std::string> AvailableOperatorNames()
    //{
    //    return { "A+B", "LA+B",
    //        "A*|B|", "B*|A|", "*",
    //        "A*UB", "B*UA", "UA*UB",
    //        "LA-B", "|A|-|B|",
    //        "M||","m||","M","m",
    //        "P+N", "N+P",
    //        "A(1+B)", "B(1+A)",
    //        "|A|*|B|",
    //        "mB|A|", "mA|B|"};
    //}
};

}//namespace isl
