#include <Wire.h>
#include "CapacitiveTouchPad.h"

int IrqPin = 8;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states
const int BuzzerPin = 7;

int FrequencyTable[] = {
  262,
  277,
  294,
  311,
  330,
  349,
  370,
  392,
  415,
  440,
  466,
  494,
  523
  };

CCapacitiveTouchPad TouchPad;

void setup()
{
  for(int idx = 0; idx<TouchPad.GetNumPads(); ++idx)
  {
    touchStates[idx] = false;
  }

  Serial.begin(115200);

  Serial.println("Capacitive Touchpad keyboard...");

  TouchPad.Begin(IrqPin);//, 0x60, 0x80);
}

int getActivePad()
{
  int ActivePad = -1;
  for(int Pad = 0; Pad<12; ++Pad)
  {
    if(touchStates[Pad])
    {
      ActivePad = Pad;//highest touched is active
    }
  }
  return ActivePad;
}

void loop()
{
    int ActivePre = getActivePad();

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

    int ActivePost = getActivePad();

    if(ActivePre != ActivePost)
    {
      if(ActivePost==-1)
      {
        noTone(BuzzerPin);
      }
      else
      {
        int FrequencyHz = FrequencyTable[ActivePost];
        tone(BuzzerPin, FrequencyHz);
      }
    }
}
