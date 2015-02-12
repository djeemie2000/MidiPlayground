
#include <Wire.h>
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp;
int cntr = 0;

void setup()
{
  // use first 8 gpio's as input with pullup enabled
  Serial.begin(115200);
  
  mcp.begin(0); // use default address 0

  for(int Port = 8; Port<16; ++Port)
  {
    mcp.pinMode(Port, INPUT);
    mcp.pullUp(Port, HIGH); // turn on a 100K pullup internally
  }
}

void loop()
{
  unsigned long Before = millis();
  uint16_t ReadValues = 0;
  for(int Repeat = 0; Repeat<1000; ++Repeat)
  {
    ReadValues = mcp.readGPIOAB();
  }
  unsigned long After = millis();
  Serial.print(cntr++);
  Serial.print("  ");
  Serial.print(After-Before);
  Serial.print("  ");
  Serial.println(ReadValues, BIN);
  delay(500);
}


