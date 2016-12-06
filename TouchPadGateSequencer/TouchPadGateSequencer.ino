#include <Wire.h>
#include "CapacitiveTouchPad.h"
#include "DigitalIn.h"

const int IrqPin = 8;  // 
CCapacitiveTouchPad TouchPad;
const int GateOutPin = 2;//TODO
const int ClockInPin = 11;
CDigitalIn g_ClockIn;

const int NumGates = 8;
const int PatternLength = 8;//TODO 16

enum EMode
{
  LiveMode = 0,
  ToggleMode = 1,
  PatternOnMode = 2,
  PatternOffMode = 3
};

EMode g_Mode[NumGates];
bool g_Gate[NumGates];
uint32_t g_Pattern[NumGates];

int g_Cursor;

void setup()
{
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
  g_ClockIn.Begin(ClockInPin);
}

void loop()
{
    // TODO read clock input to advance cursor when rising edge
    g_ClockIn.Read();
    if(g_ClockIn.IsRising())
    {
      g_Cursor = (g_Cursor+1)%PatternLength;
    }
    
    TouchPad.Read();
    for(int Pad = 0; Pad<NumGates; ++Pad)
    {
      // switch mode <=> mode pad pressed and Pad pressed
      if(TouchPad.IsClicked(Pad))
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
        if(TouchPad.IsClicked(Pad))
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

      // outputs update
      digitalWrite(GateOutPin + Pad, g_Gate[Pad]?HIGH:LOW);
    }

    //TODO ledmatrix update 
}
