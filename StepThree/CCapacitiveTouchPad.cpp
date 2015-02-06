#include "CCapacitiveTouchPad.h"
#include <Wire.h>
#include "mpr121.h"

CCapacitiveTouchPad::CCapacitiveTouchPad()
    : m_IrqPin(2)
    , m_Address(0x5A)
    , m_TouchState(0)
{
}

namespace
{

void SetRegister(int Address, uint8_t Register, uint8_t Value)
{
    Wire.beginTransmission(Address);
    Wire.write(Register);
    Wire.write(Value);
    Wire.endTransmission();
}

}
void CCapacitiveTouchPad::Begin(int IrqPin, uint8_t Address)
{
    m_IrqPin = IrqPin;
    m_Address = Address;

    pinMode(m_IrqPin, INPUT_PULLUP);

    Wire.begin();//?? needed here or outside this class ??

    SetRegister(m_Address, ELE_CFG, 0x00);

    // Section A - Controls filtering when data is > baseline.
    SetRegister(m_Address, MHD_R, 0x01);
    SetRegister(m_Address, NHD_R, 0x01);
    SetRegister(m_Address, NCL_R, 0x00);
    SetRegister(m_Address, FDL_R, 0x00);

    // Section B - Controls filtering when data is < baseline.
    SetRegister(m_Address, MHD_F, 0x01);
    SetRegister(m_Address, NHD_F, 0x01);
    SetRegister(m_Address, NCL_F, 0xFF);
    SetRegister(m_Address, FDL_F, 0x02);

    // Section C - Sets touch and release thresholds for each electrode
    SetRegister(m_Address, ELE0_T, TOU_THRESH);
    SetRegister(m_Address, ELE0_R, REL_THRESH);

    SetRegister(m_Address, ELE1_T, TOU_THRESH);
    SetRegister(m_Address, ELE1_R, REL_THRESH);

    SetRegister(m_Address, ELE2_T, TOU_THRESH);
    SetRegister(m_Address, ELE2_R, REL_THRESH);

    SetRegister(m_Address, ELE3_T, TOU_THRESH);
    SetRegister(m_Address, ELE3_R, REL_THRESH);

    SetRegister(m_Address, ELE4_T, TOU_THRESH);
    SetRegister(m_Address, ELE4_R, REL_THRESH);

    SetRegister(m_Address, ELE5_T, TOU_THRESH);
    SetRegister(m_Address, ELE5_R, REL_THRESH);

    SetRegister(m_Address, ELE6_T, TOU_THRESH);
    SetRegister(m_Address, ELE6_R, REL_THRESH);

    SetRegister(m_Address, ELE7_T, TOU_THRESH);
    SetRegister(m_Address, ELE7_R, REL_THRESH);

    SetRegister(m_Address, ELE8_T, TOU_THRESH);
    SetRegister(m_Address, ELE8_R, REL_THRESH);

    SetRegister(m_Address, ELE9_T, TOU_THRESH);
    SetRegister(m_Address, ELE9_R, REL_THRESH);

    SetRegister(m_Address, ELE10_T, TOU_THRESH);
    SetRegister(m_Address, ELE10_R, REL_THRESH);

    SetRegister(m_Address, ELE11_T, TOU_THRESH);
    SetRegister(m_Address, ELE11_R, REL_THRESH);

    // Section D
    // Set the Filter Configuration
    // Set ESI2
    SetRegister(m_Address, FIL_CFG, 0x04);

    // Section E
    // Electrode Configuration
    // Set ELE_CFG to 0x00 to return to standby mode
    SetRegister(m_Address, ELE_CFG, 0x0C);  // Enables all 12 Electrodes


    // Section F
    // Enable Auto Config and auto Reconfig
    /*set_register(m_Address, ATO_CFG0, 0x0B);
      set_register(m_Address, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(m_Address, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
      set_register(m_Address, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V

    SetRegister(m_Address, ELE_CFG, 0x0C);
}

void CCapacitiveTouchPad::Read()
{
    if(!digitalRead(m_IrqPin))
    {
        //read the touch state from the MPR121
        uint8_t NumBytes = 2;
        Wire.requestFrom(m_Address,NumBytes);

        uint8_t Lsb = Wire.read();
        uint8_t Msb = Wire.read();

        m_TouchState = ((Msb << 8) | Lsb); //16bits that make up the touch states
    }
}

int CCapacitiveTouchPad::GetNumPads() const
{
    return 12;
}

bool CCapacitiveTouchPad::Get(int Pad) const
{
    if(0<=Pad && Pad<GetNumPads())
    {
        return m_TouchState & (1<<Pad);
    }
    return false;
}

