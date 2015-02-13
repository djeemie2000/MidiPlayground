#pragma once

#include <Arduino.h>

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include "EditMode.h"

struct SControlChange;

class CDisplay
{
public:
    CDisplay();
    void Begin();

    /*
     * Call update upon change of edit mode and/or parameters.
     */
    void Update(EEditMode EditMode, const SControlChange* ControlChange, int NumControllers);

private:
    LiquidCrystal_I2C	m_lcd; 
};
