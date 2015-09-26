#pragma once

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void mcp48_begin()
{
  pinMode(PIN_CS, OUTPUT);
  spi4teensy3::init();
}

void mcp48_setOutput(unsigned int val)
{
  //assuming single channel, gain=1
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x30;

  digitalWrite(PIN_CS, LOW);
  spi4teensy3::send(highByte); 
  spi4teensy3::send(lowByte);
  digitalWrite(PIN_CS, HIGH);
}

void mcp48_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  digitalWrite(PIN_CS, LOW);
  spi4teensy3::send(highByte);
  spi4teensy3::send(lowByte);
  digitalWrite(PIN_CS, HIGH);
}

