#include <Wire.h>
#include "CapacitiveTouchPad.h"

const int IrqPin = 8;  // Digital 2
CCapacitiveTouchPad TouchPad;

const int NumGates = 8;
const int PatternLength = 8;//TODO 16

enum EMode
{
  LiveMode = 0,
  ToggleMode = 1,
  PatternOnMode = 2,
  PatternOffMode = 3
};

bool g_TouchStates[NumGates]; //to keep track of the previous touch states, for toggling, ...

EMode g_Mode[NumGates];
bool g_Gate[NumGates];
uint32_t g_Pattern[NumGates];

int g_Cursor;

void setup()
{
  for(int idx = 0; idx<TouchPad.GetNumPads(); ++idx)
  {
    g_TouchStates[idx] = false;
  }

  for(int idx = 0; idx<NumGates; ++idx)
  {
    g_Mode[idx] = LiveMode;
    g_Gate[idx] = false;
    g_Pattern[idx] = 0;
  }

  g_Cursor = 0;

  Serial.begin(115200);
  Serial.println("Touchpad Gate Sequencer...");

  TouchPad.Begin(IrqPin);//, 0x60, 0x80);
}

void loop()
{
    // TODO read clock input to advance cursor when rising edge
    
    TouchPad.Read();
    for(int Pad = 0; Pad<NumGates; ++Pad)
    {
      // switch mode <=> mode pad pressed and Pad pressed
      if(TouchPad.Get(Pad))
      {
        if(TouchPad.Get(8))
        {
          g_Mode[Pad] = LiveMode;
        }
        else if(TouchPad.Get(9))
        {
          g_Mode[Pad] = ToggleMode;
        }
        else if(TouchPad.Get(10))
        {
          g_Mode[Pad] = PatternOnMode;
        }
        else if(TouchPad.Get(11))
        {
          g_Mode[Pad] = PatternOffMode;
        }
      }
      
      if(g_Mode[Pad] == LiveMode)
      {
        g_Gate[Pad] = TouchPad.Get(Pad);
      }
      else if(g_Mode[Pad] == ToggleMode)
      {      
        if(g_TouchStates[Pad]!=TouchPad.Get(Pad))
        {
          g_Gate[Pad] = !g_Gate[Pad];//Toggle
        }
      }
      else if(g_Mode[Pad]==PatternOnMode)
      {
        if(TouchPad.Get(Pad))
        {
          bitSet(g_Pattern[Pad], g_Cursor);
        }
        g_Gate[Pad] = bitRead(g_Pattern[Pad], g_Cursor);
      }
      else if(g_Mode[Pad]==PatternOffMode)
      {
        if(TouchPad.Get(Pad))
        {
          bitClear(g_Pattern[Pad], g_Cursor);
        }
        g_Gate[Pad] = bitRead(g_Pattern[Pad], g_Cursor);
      }

      g_TouchStates[Pad] = TouchPad.Get(Pad);        

      // outputs update
      digitalWrite(GateOutPin + Pad, g_Gate[Pad]?HIGH:LOW);
    }

    //TODO ledmatrix update 
}
