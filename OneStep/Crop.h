#ifndef CROP_H
#define CROP_H

#include <Arduino.h>

int Crop(int Value, int Min, int Max)
{
    return max(min(Value, Max), Min);
}

#endif // CROP_H
