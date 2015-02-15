#include "ShiftRegisterInputs.h"

CShiftRegisterInputs::CShiftRegisterInputs()
 : m_EnablePin(0)
 , m_LoadPin(0)
 , m_ClockPin(0)
 , m_DataPin(0)
 , m_State(0x00)
{
}

void CShiftRegisterInputs::Begin(int EnablePin, int LatchPin, int ClockPin, int DataPin)
{
  m_EnablePin = EnablePin;
  m_LoadPin = LatchPin;
  m_ClockPin = ClockPin;
  m_DataPin = DataPin;

  pinMode(m_EnablePin,OUTPUT);
  pinMode(m_LoadPin,OUTPUT);
  pinMode(m_ClockPin,OUTPUT);
  pinMode(m_DataPin,INPUT);

  digitalWrite(m_LoadPin,HIGH);
  digitalWrite(m_EnablePin,HIGH);
}

bool CShiftRegisterInputs::Get(int Output) const
{
  return bitRead(m_State, Output)==1;
}

void CShiftRegisterInputs::Read()
{
    // pulse 5 microsec on PL
    digitalWrite(m_LoadPin,LOW);
    delayMicroseconds(5);
    digitalWrite(m_LoadPin,HIGH);
    delayMicroseconds(5);

    digitalWrite(m_ClockPin,HIGH);
    digitalWrite(m_EnablePin,LOW);

    m_State = shiftIn(m_DataPin, m_ClockPin, MSBFIRST);

    digitalWrite(m_EnablePin,HIGH);
}

