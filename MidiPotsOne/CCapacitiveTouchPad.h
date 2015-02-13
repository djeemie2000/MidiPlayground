#ifndef CCAPACITIVETOUCHPAD_H
#define CCAPACITIVETOUCHPAD_H

#include <Arduino.h>

class CCapacitiveTouchPad
{
public:
    CCapacitiveTouchPad();

    void Begin(int IrqPin, uint8_t Address = 0x5A);

    void Read();
    int GetNumPads() const;
    bool Get(int Pad) const;

private:
    int     m_IrqPin;
    uint8_t m_Address;
    uint16_t m_TouchState;// bit = 1 => touched, bit is 0 => not touched
};

#endif // CCAPACITIVETOUCHPAD_H
