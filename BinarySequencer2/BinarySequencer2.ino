#include <EEPROM.h>
#include <Wire.h>
#include "CapacitiveTouchPad.h"
#include "LedControlMS.h"
#include "IntBinaryPattern.h"
#include "Stepper.h"
#include "TimerOne.h"
#include "RotaryEncoder.h"
#include "DigitalIn.h"

const int NumLedMatrices = 1;
const int DataPin = 12;
const int ClockPin = 11;
const int LoadPin = 10;
LedControl g_LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);

CRotaryEncoder g_RotaryA;
int g_RotaryPositionA;
CDigitalIn g_PushButtonA;
CRotaryEncoder g_RotaryB;
int g_RotaryPositionB;
CDigitalIn g_PushButtonB;

int g_EditModeA;
int g_EditModeB;

CStepper<int> g_HiResStepper;

static const int NumPatterns = 2;
isl::CBinaryPattern<uint8_t> g_Pattern[NumPatterns];
CStepper<int> g_Stepper[NumPatterns];
int g_EditPattern;

int g_Gate[NumPatterns];

// gate outputs
static const int GateOutPin = A0;

// Speed
static const int HiResPeriod = 250;

void OnTick()
{
  g_HiResStepper.Advance();
  if(0==g_HiResStepper.GetStep())
  {
    
    // advance steps
    for(int idx = 0; idx<NumPatterns; ++idx)
    {
      g_Stepper[idx].Advance();

      g_Gate[idx] = g_Pattern[idx].GetBit(g_Stepper[idx].GetStep()) ? 1 : 0;
    }  
    // update (logical combination) gates 
    digitalWrite(GateOutPin, g_Gate[0]?HIGH:LOW);
    digitalWrite(GateOutPin+1, g_Gate[1]?HIGH:LOW);
    
    int AndGate = (g_Gate[0] && g_Gate[1]) ? HIGH : LOW;
    int XorGate = (g_Gate[0] || g_Gate[1]) && (g_Gate[0]!=g_Gate[1]) ? HIGH : LOW; 
    int NoneGate = !g_Gate[0] && !g_Gate[1] ? HIGH : LOW;
    int NandGate = !(g_Gate[0]&&g_Gate[1]) ? HIGH : LOW;
    digitalWrite(GateOutPin+2, AndGate);
    digitalWrite(GateOutPin+3, XorGate);
    digitalWrite(GateOutPin+4, NoneGate);
    digitalWrite(GateOutPin+5, NandGate);
  }
  // halfway step => all gates off
  else if(HiResPeriod/2==g_HiResStepper.GetStep())
  {
     digitalWrite(GateOutPin, LOW);
     digitalWrite(GateOutPin+1, LOW);
     digitalWrite(GateOutPin+2, LOW);
     digitalWrite(GateOutPin+3, LOW);
     digitalWrite(GateOutPin+4, LOW);
     digitalWrite(GateOutPin+5, LOW);
  }
}

void Save()
{
  for(int idx = 0; idx<NumPatterns; ++idx)
  {
    EEPROM.update(idx*2, g_Pattern[idx].Get());
    EEPROM.update(idx*2+1, g_Stepper[idx].GetNumSteps());
  }
}

void Load()
{
  for(int idx = 0; idx<NumPatterns; ++idx)
  {
    g_Pattern[idx].Reset(EEPROM.read(idx*2));
    int NumSteps = min(8, EEPROM.read(idx*2+1));
    g_Stepper[idx].SetNumSteps(NumSteps);
  }  
}


void setup()
{
  // put your setup code here, to run once:
  
  
  Serial.begin(115200);
  Serial.println("BinarySequencer...");

  pinMode(GateOutPin, OUTPUT);
  pinMode(GateOutPin+1, OUTPUT);
  pinMode(GateOutPin+2, OUTPUT);
  pinMode(GateOutPin+3, OUTPUT);
  pinMode(GateOutPin+4, OUTPUT);
  pinMode(GateOutPin+5, OUTPUT);

  g_RotaryA.Begin(3,2);
  g_RotaryPositionA = 0;  
  g_PushButtonA.Begin(6);
  g_RotaryB.Begin(5,4);
  g_RotaryPositionB = 0; 
  g_PushButtonB.Begin(7); 

  g_EditModeA = 0;
  g_EditModeB = 0;

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

  g_EditPattern = 0;
  for(int idx = 0; idx<NumPatterns; ++idx)
  {
    g_Pattern[idx].Reset(0x11);
    g_Stepper[idx].SetNumSteps(8);
    g_Gate[idx] = 0;
  }
  g_HiResStepper.SetNumSteps(HiResPeriod);

  Load();

  unsigned long TimerPeriodMicroSeconds = 1000;// 2*120 bpm
  Timer1.initialize(TimerPeriodMicroSeconds);
  Timer1.attachInterrupt(OnTick);

}

void ShowPattern(int Idx)
{
    uint8_t Pattern = g_Pattern[Idx].Get();
    const int LedMatrixId = 0;
    const int Column = Idx*4;
    g_LedControl.setColumn(LedMatrixId, Column, Pattern);  
}

void ShowStep(int Idx)
{
    int Step = g_Stepper[Idx].GetStep();
    int Bit = g_Pattern[Idx].GetBit(Step);
    uint8_t Pattern = Bit << Step;
  
    const int LedMatrixId = 0;
    const int Column = 1 + Idx*4;
    g_LedControl.setColumn(LedMatrixId, Column, Pattern);  
}

void ShowNumSteps(int Idx)
{
    int NumSteps = g_Stepper[Idx].GetNumSteps();
    uint8_t Pattern = 0xFF >> (8-NumSteps);
  
    const int LedMatrixId = 0;
    const int Column = 2 + Idx*4;
    g_LedControl.setColumn(LedMatrixId, Column, Pattern);  
}

void OnTouch(int Pad)
{
  // Handle touch
  if(Pad==0)
  {
    g_Pattern[g_EditPattern].Next();
  }
  else if(Pad==4)
  {
    g_Pattern[g_EditPattern].Prev();
  }
  else if(Pad==1)
  {
    g_Pattern[g_EditPattern].Invert();
  }
  else if(Pad==5)
  {
    g_Pattern[g_EditPattern].Mirror();
  }
  else if(Pad==2)
  {
    g_Pattern[g_EditPattern].RotateNext();
  }
  else if(Pad==6)
  {
    g_Pattern[g_EditPattern].RotatePrev();
  }
  else if(Pad==3)
  {
    // TODO random
  }
  else if(Pad==7)
  {
    // clear
    g_Pattern[g_EditPattern].Reset(0x00);
  }
  else if(Pad==8)
  {
    g_EditPattern = 0;
  }
  else if(Pad==9)
  {
    g_EditPattern = 1;
  }
  else if(Pad==10)
  {
    int NumSteps = g_Stepper[g_EditPattern].GetNumSteps()+1;
    if(NumSteps<=8)
    {
      g_Stepper[g_EditPattern].SetNumSteps(NumSteps);
    }
  }
  else if(Pad==11)
  {
    int NumSteps = g_Stepper[g_EditPattern].GetNumSteps()-1;
    if(1<=NumSteps)
    {
      g_Stepper[g_EditPattern].SetNumSteps(NumSteps);
    }
  }

  Save();

  //Serial.print("OnTouch ");
  //Serial.println(Pad);
}

void loop()
{
   //read rotary encoders
   g_PushButtonA.Read();
   if(g_PushButtonA.IsPressed())
   {
    g_EditModeA = 1-g_EditModeA;
    Serial.println("A pressed");
   }
   g_PushButtonB.Read();
   if(g_PushButtonB.IsPressed())
   {
    Serial.println("B pressed");
    g_EditModeB = 1-g_EditModeB;
   }
   //
    g_RotaryA.Read();
    if(g_RotaryPositionA<g_RotaryA.GetPosition())
    {
      g_EditPattern = 0; 
      if(g_EditModeA==0)
      {
        OnTouch(0);
      }
      else 
      {
        OnTouch(2);
      }
    }
    else if(g_RotaryA.GetPosition()<g_RotaryPositionA)
    {
      g_EditPattern = 0; 
      if(g_EditModeA==0)
      {
        OnTouch(4);
      }
      else
      {
        OnTouch(6);
      }
    }    
    g_RotaryPositionA = g_RotaryA.GetPosition();
    //
    g_RotaryB.Read();
    if(g_RotaryPositionB<g_RotaryB.GetPosition())
    {
      g_EditPattern = 1; 
      if(g_EditModeB==0)
      {
        OnTouch(0);
      }
      else
      {
        OnTouch(2);
      }
    }
    else if(g_RotaryB.GetPosition()<g_RotaryPositionB)
    {
      g_EditPattern = 1; 
      if(g_EditModeB==0)
      {
        OnTouch(4);
      }
      else
      {
        OnTouch(6);
      }
    }
    g_RotaryPositionB = g_RotaryB.GetPosition();
    // 
    for(int idx = 0; idx<NumPatterns; ++idx)
    {
      ShowPattern(idx);
      ShowStep(idx);
      ShowNumSteps(idx);
    }
}
