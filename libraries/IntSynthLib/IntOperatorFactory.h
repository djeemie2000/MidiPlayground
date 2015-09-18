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
    static IntOperator GetOperator(int Selected)
    {
        const IntOperator AvailableOperators[] = { IntSawUp<Scale>,
                                                              IntSawDown<Scale>,
                                                              IntPulse<Scale>,
                                                              IntFullPseudoSin<Scale>,
                                                              IntSemiPseudoSin<Scale>,
                                                              IntTriangle<Scale>,
                                                              IntQuadratic<Scale>,
                                                              IntNoOp<Scale>};
        return AvailableOperators[Selected];
    }

    //static std::vector<std::string> AvailableOperatorNames()
    //{
    //    return { "Saw+", "Saw-", "Pulse-", "Sin", "Sinh", "Triangle", "Quadratic", "NoOp" };
    //}
};

}//namespace isl
