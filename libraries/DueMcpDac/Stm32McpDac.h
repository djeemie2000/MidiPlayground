#pragma once

#include "CommonMcpDac.h"

namespace mcp48dac
{

static const int MaxNumDac = 1;
static const int Pin_ChipSelect[MaxNumDac] = {PA4};

//

void Begin(int NumDac =1)
{
  for(int Dac = 0; Dac<NumDac && Dac<MaxNumDac; ++Dac)
  {
    pinMode(Pin_ChipSelect[Dac], OUTPUT);
  }
  //??needed??
  // PA4 -> CS
  // PB3 / PA5 -> SCLK
  // PB5 / PA6 -> MOSI
  // PB4 / PA7 -> MISO
  //pinMode(PA5, OUTPUT);
  //pinMode(PA7, OUTPUT);

  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void SetOutput(unsigned int Value, byte Channel, byte Gain, int Dac = 0)
{
  byte LowByte = Value & 0xff;
  byte HighByte = ((Value >> 8) & 0xff) | Channel | Gain | Active;

  // begin / end transaction???
  //SPI.beginTransaction(SPISettings(16000000L, MSBFIRST, SPI_MODE0));
  int Pin = Pin_ChipSelect[Dac];
  digitalWrite(Pin, LOW);
  SPI.transfer(HighByte);
  SPI.transfer(LowByte);
  digitalWrite(Pin, HIGH);
  //SPI.endTransaction();
}

}
