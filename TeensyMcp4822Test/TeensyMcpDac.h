#pragma once

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void mcp48_begin()
{
  pinMode(PIN_CS, OUTPUT);
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2); //AVR: defult is 4 so 16MHz/4 = 4MHz
}

void mcp48_setOutput(unsigned int val)
{
  //assuming single channel, gain=1
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x30;

  digitalWrite(PIN_CS, LOW);//PORTB &= 0xfb;//assuming pin 10
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  digitalWrite(PIN_CS, HIGH);//PORTB |= 0x4;//assuming pin 10
}

void mcp48_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  digitalWrite(PIN_CS, LOW);//PORTB &= 0xfb;//assuming pin 10
  SPI.transfer(highByte);
  SPI.transfer(lowByte);
  digitalWrite(PIN_CS, HIGH);//PORTB |= 0x4;//assuming pin 10
}

