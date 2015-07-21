#ifndef CCAPACITIVETOUCHPAD_H
#define CCAPACITIVETOUCHPAD_H

#include <Arduino.h>

class CCapacitiveTouchPad
{
public:
    static const uint8_t DefaultAddress = 0x5A;
    static const uint8_t DefaultTouchThreshold = 0x06;
    static const uint8_t DefaultReleaseThreshold = 0x0A;

    CCapacitiveTouchPad();

    void Begin(int IrqPin,
                uint8_t TouchThreshold = DefaultTouchThreshold,
                uint8_t ReleaseThreadhold = DefaultReleaseThreshold,
                uint8_t Address = DefaultAddress);
    // TODO return true/false upon I2C device found or not !

    void Read();
    int GetNumPads() const;
    bool Get(int Pad) const;

private:
    int     m_IrqPin;
    uint8_t m_Address;
    uint16_t m_TouchState;// bit = 1 => touched, bit is 0 => not touched
};

#endif // CCAPACITIVETOUCHPAD_H
