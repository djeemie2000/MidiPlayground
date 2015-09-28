#pragma once

//#include <vector>
//#include <string>
#include "IntOperators.h"

namespace isl
{

template<int Scale>
class COperatorFactory
{
public:
    static const int NumOperators = 8;

    static IntOperator GetOperator(int Selected)
    {
        if(Selected == 0)
        {
            return IntSawUp<Scale>;
        }
        else if(Selected == 1)
        {
            return IntSawDown<Scale>;
        }
        else if(Selected == 2)
        {
            return IntPulsePos<Scale>;
        }
        else if(Selected == 3)
        {
            return IntPulseNeg<Scale>;
        }
        else if(Selected == 4)
        {
            return IntFullPseudoSin<Scale>;
        }
        else if(Selected == 5)
        {
            return IntSemiPseudoSin<Scale>;
        }
        else if(Selected == 6)
        {
            return IntTriangle<Scale>;
        }
        else if(Selected == 7)
        {
            return IntQuadratic<Scale>;
        }

        return IntNoOp<Scale>;
    }

    //static std::vector<std::string> AvailableOperatorNames()
    //{
    //    return { "Saw+", "Saw-", "Pulse+", "Pulse-", "Sin", "Sinh", "Triangle", "Quadratic", "NoOp" };
    //}
};

}//namespace isl
