#include <Wire.h>
#include "CapacitiveTouchPad.h"

int IrqPin = 2;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states

CCapacitiveTouchPad TouchPad;

void setup()
{
  for(int idx = 0; idx<TouchPad.GetNumPads(); ++idx)
  {
    touchStates[idx] = false;
  }

  Serial.begin(9600);
  //Wire.begin();

  TouchPad.Begin(IrqPin, 0x5A);
}

void loop()
{
    TouchPad.Read();
    for(int Pad = 0; Pad<TouchPad.GetNumPads(); ++Pad)
    {
      if(touchStates[Pad]!=TouchPad.Get(Pad))
      {
        touchStates[Pad] = TouchPad.Get(Pad);

        Serial.print("Pad ");
        Serial.print(Pad);
        if(touchStates[Pad])
        {
          //Pad was just touched
          Serial.println(" was just touched");
        }
        else
        {
            //Pad was just released
            Serial.println(" was just released");
        }
      }
    }
}
