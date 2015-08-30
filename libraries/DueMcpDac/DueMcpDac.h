#pragma once

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void mcp48_begin()
{
  SPI.begin(PIN_CS);//can be 4, 10 or ??
  SPI.setClockDivider(PIN_CS, 4);//DUE: default is 21 corresponding to 4MHz as in AVR
}

void mcp48_setOutput(unsigned int val)
{
  //assuming single channel, gain=2
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x10;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}

void mcp48_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}
