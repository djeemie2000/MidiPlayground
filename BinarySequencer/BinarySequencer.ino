#include <Wire.h>
#include "CapacitiveTouchPad.h"
#include "LedControlMS.h"
#include "IntBinaryPattern.h"
#include "Stepper.h"
#include "TimerOne.h"


const int NumLedMatrices = 1;
const int DataPin = 12;
const int ClockPin = 11;
const int LoadPin = 10;
LedControl g_LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);

int IrqPin = 8;  // Digital 2
CCapacitiveTouchPad TouchPad;
boolean touchStates[12]; //to keep track of the previous touch states

isl::CBinaryPattern<uint8_t> g_Pattern;
CStepper<int> g_Stepper;
CStepper<int> g_HiResStepper;


void OnTick()
{
  g_HiResStepper.Advance();
  if(0==g_HiResStepper.GetStep())
  {
    g_Stepper.Advance();
  }
}
void setup()
{
  // put your setup code here, to run once:
  for (int idx = 0; idx < TouchPad.GetNumPads(); ++idx)
  {
    touchStates[idx] = false;
  }
  
  Serial.begin(115200);
  Serial.println("BinarySequencer...");

  for (int idx = 0; idx < NumLedMatrices; ++idx)
  {
    /*
     The MAX72XX is in power-saving mode on startup,
     we have to do a wakeup call
     */
    g_LedControl.shutdown(idx, false);
    /* Set the brightness to a low values */
    g_LedControl.setIntensity(idx, 1);
    /* and clear the display */
    delay(1000);
    g_LedControl.clearDisplay(idx);
  }

  g_Pattern.Reset(0x88);
  g_Stepper.SetNumSteps(8);

  g_HiResStepper.SetNumSteps(250);

  unsigned long TimerPeriodMicroSeconds = 1000;// 2*120 bpm
  Timer1.initialize(TimerPeriodMicroSeconds);
  Timer1.attachInterrupt(OnTick);

  TouchPad.Begin(IrqPin);
}

void ShowPattern()
{
    uint8_t Pattern = g_Pattern.Get();
    const int LedMatrixId = 0;
    const int Column = 0;
    g_LedControl.setColumn(LedMatrixId, Column, Pattern);  
}

void ShowStep()
{
    int Step = g_Stepper.GetStep();
    int Bit = g_Pattern.GetBit(Step);
    uint8_t Pattern = Bit << Step;
  
    const int LedMatrixId = 0;
    const int Column = 1;
    g_LedControl.setColumn(LedMatrixId, Column, Pattern);  
  
}

void OnTouch(int Pad)
{
  // Handle touch
  if(Pad==0)
  {
    g_Pattern.Next();
  }
  else if(Pad==4)
  {
    g_Pattern.Prev();
  }
  else if(Pad==1)
  {
    g_Pattern.Invert();
  }
  else if(Pad==5)
  {
    g_Pattern.Mirror();
  }
  else if(Pad==2)
  {
    g_Pattern.RotateNext();
  }
  else if(Pad==6)
  {
    g_Pattern.RotatePrev();
  }
  else if(Pad==3)
  {
    // TODO random
  }
  else if(Pad==7)
  {
    // default
    //g_Pattern.Reset(0x88);//default

    // clear
    g_Pattern.Reset(0x00);
  }
  else if(Pad==10)
  {
    int NumSteps = g_Stepper.GetNumSteps()+1;
    if(NumSteps<=8)
    {
      g_Stepper.SetNumSteps(NumSteps);
    }
  }
  else if(Pad==11)
  {
    int NumSteps = g_Stepper.GetNumSteps()-1;
    if(1<=NumSteps)
    {
      g_Stepper.SetNumSteps(NumSteps);
    }
  }

  Serial.print("OnTouch ");
  Serial.println(Pad);
}

void loop()
{
    TouchPad.Read();
    for(int Pad = 0; Pad<TouchPad.GetNumPads(); ++Pad)
    {
      if(touchStates[Pad]!=TouchPad.Get(Pad))
      {
        touchStates[Pad] = TouchPad.Get(Pad);

        if(touchStates[Pad])
        {
          //Pad was just touched
          OnTouch(Pad);
        }
      }
    }
    
    // 
    ShowPattern();
    ShowStep();

    //delay(10);//avoid touchpad from being too responsive?
}
