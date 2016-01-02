#pragma once

#include "CommonMcpDac.h"

// old interface for backwards compatibility
const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void mcp48_begin()
{
  pinMode(PIN_CS, OUTPUT);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void mcp48_setOutput(unsigned int val)
{
  //assuming single channel, gain=1
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x30;

  PORTB &= 0xfb;//assuming pin 10
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4;//assuming pin 10
}

void mcp48_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  PORTB &= 0xfb;//assuming pin 10
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  PORTB |= 0x4;//assuming pin 10
}

namespace mcp48dac
{

static const int MaxNumDac = 3;
static const int Pin_ChipSelect[MaxNumDac] = {10, 9, 8};
static const byte PortMask_ChipSelect[MaxNumDac] = {0x04, 0x02, 0x01};//all pins are on PORTB !!!!

void Begin(int NumDac =1)
{
  for(int Dac = 0; Dac<NumDac && Dac<MaxNumDac; ++Dac)
  {
    pinMode(Pin_ChipSelect[Dac], OUTPUT);
  }
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void SetOutput(unsigned int Value, byte Channel, byte Gain, int Dac = 0)
{
  byte LowByte = Value & 0xff;
  byte HighByte = ((Value >> 8) & 0xff) | Channel | Gain | Active;

  byte PortMask = PortMask_ChipSelect[Dac];
  PORTB &= ~PortMask;//digitalWrite(Pin, LOW);
  SPI.transfer(HighByte);
  SPI.transfer(LowByte);
  PORTB |= PortMask;// digitalWrite(Pin, HIGH); 
} 

}

