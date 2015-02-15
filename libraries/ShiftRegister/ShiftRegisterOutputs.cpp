#include "ShiftRegisterOutputs.h"

CShiftRegisterOutputs::CShiftRegisterOutputs()
 : m_LatchPin(0)
 , m_ClockPin(0)
 , m_DataPin(0)
 , m_State(0x00)
{
}

void CShiftRegisterOutputs::Begin(int LatchPin, int ClockPin, int DataPin)
{
  m_LatchPin = LatchPin;
  m_ClockPin = ClockPin;
  m_DataPin = DataPin;
  pinMode(m_LatchPin, OUTPUT);
  pinMode(m_ClockPin, OUTPUT);
  pinMode(m_DataPin, OUTPUT);

}

void CShiftRegisterOutputs::Set(int Output, bool State)
{
  if(State)
  {
    bitSet(m_State, Output);
  }
  else
  {
    bitClear(m_State, Output);
  }
}

void CShiftRegisterOutputs::Write()
{
  // take the latchPin low so 
  // the outputs don't change while you're sending in bits:
  digitalWrite(m_LatchPin, LOW);
  // shift out the bits:
  shiftOut(m_DataPin, m_ClockPin, MSBFIRST, m_State);  
  //take the latch pin high so the outputs will update:
  digitalWrite(m_LatchPin, HIGH);
}

