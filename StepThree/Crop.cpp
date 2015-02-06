#include "Crop.h"

#include <Arduino.h>

int Crop(int Value, int Min, int Max)
{
    return max(min(Value, Max), Min);
}
