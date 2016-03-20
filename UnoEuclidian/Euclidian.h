#pragma once

#include "Arduino.h"//uint32_t

namespace euclidian
{

uint32_t CalcActiveMask(int Length);
uint32_t CalcPattern(int Length, int Fills);

}

