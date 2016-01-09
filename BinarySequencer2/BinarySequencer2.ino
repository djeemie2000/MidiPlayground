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

CStepper<int> g_HiResStepper;

static const int NumPatterns = 2;

int g_Gate[NumPatterns];

// gate outputs
static const int GateOutPin = A0;

// Speed
static const int HiResPeriod = 500;

void Save();//pre declaration

struct SController
{
  CRotaryEncoder s_Rotary;
  int s_RotaryPosition;
  CDigitalIn s_PushButton;
  int s_EditMode;
  CStepper<int> s_Stepper;

  isl::CBinaryPattern<uint8_t> s_Pattern;

  SController()
    : s_Rotary()
    , s_RotaryPosition(0)
    , s_PushButton()
    , s_EditMode(0)
    , s_Stepper()
  {}

  void Begin(int EncoderPinA, int EncoderPinB, int PushButtonPin)
  {
    s_Rotary.Begin(EncoderPinA, EncoderPinB);
    s_RotaryPosition = 0;
    s_PushButton.Begin(PushButtonPin);
    s_EditMode = 0;
  }

  void Update()
  {
    s_PushButton.Read();
    if (s_PushButton.IsPressed())
    {
      s_EditMode = (s_EditMode+1)%3;
      Serial.println("Btn pressed");
    }

    s_Rotary.Read();
    if(s_RotaryPosition < s_Rotary.GetPosition())
    {//increase
      if(s_EditMode == 0)
      {
        s_Pattern.Next();
      }
      else if(s_EditMode == 1)
      {
        s_Pattern.RotateNext();
      }
      else 
      {
        int NumSteps = s_Stepper.GetNumSteps() + 1;
        if (NumSteps <= 8)
        {
          s_Stepper.SetNumSteps(NumSteps);
        } 
      }

      Save();
    }
    else if(s_Rotary.GetPosition()<s_RotaryPosition)
    {//decrease
      if(s_EditMode == 0)
      {
        s_Pattern.Prev();
      }
      else if(s_EditMode == 1)
      {
        s_Pattern.RotatePrev();
      }
      else
      {
        int NumSteps = s_Stepper.GetNumSteps() - 1;
        if (1 <= NumSteps)
        {
          s_Stepper.SetNumSteps(NumSteps);
        } 
      }

      Save();
    }       
    s_RotaryPosition = s_Rotary.GetPosition();
    
  }

};


SController g_Controller[NumPatterns];


void OnTick()
{
  g_HiResStepper.Advance();
  if (0 == g_HiResStepper.GetStep())
  {

    // advance steps
    for (int idx = 0; idx < NumPatterns; ++idx)
    {
      g_Controller[idx].s_Stepper.Advance();

      g_Gate[idx] = g_Controller[idx].s_Pattern.GetBit(g_Controller[idx].s_Stepper.GetStep()) ? 1 : 0;
    }
    // update (logical combination) gates
    digitalWrite(GateOutPin, g_Gate[0] ? HIGH : LOW);
    digitalWrite(GateOutPin + 1, g_Gate[1] ? HIGH : LOW);

    int AndGate = (g_Gate[0] && g_Gate[1]) ? HIGH : LOW;
    int XorGate = (g_Gate[0] || g_Gate[1]) && (g_Gate[0] != g_Gate[1]) ? HIGH : LOW;
    int NoneGate = !g_Gate[0] && !g_Gate[1] ? HIGH : LOW;
    int NandGate = !(g_Gate[0] && g_Gate[1]) ? HIGH : LOW;
    digitalWrite(GateOutPin + 2, AndGate);
    digitalWrite(GateOutPin + 3, XorGate);
    digitalWrite(GateOutPin + 4, NoneGate);
    digitalWrite(GateOutPin + 5, NandGate);
  }
  // halfway step => all gates off
  else if (HiResPeriod / 2 == g_HiResStepper.GetStep())
  {
    digitalWrite(GateOutPin, LOW);
    digitalWrite(GateOutPin + 1, LOW);
    digitalWrite(GateOutPin + 2, LOW);
    digitalWrite(GateOutPin + 3, LOW);
    digitalWrite(GateOutPin + 4, LOW);
    digitalWrite(GateOutPin + 5, LOW);
  }
}

void Save()
{
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    EEPROM.update(idx * 2, g_Controller[idx].s_Pattern.Get());
    EEPROM.update(idx * 2 + 1, g_Controller[idx].s_Stepper.GetNumSteps());
  }
}

void Load()
{
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    g_Controller[idx].s_Pattern.Reset(EEPROM.read(idx * 2));
    int NumSteps = min(8, EEPROM.read(idx * 2 + 1));
    g_Controller[idx].s_Stepper.SetNumSteps(NumSteps);
  }
}


void setup()
{
  // put your setup code here, to run once:


  Serial.begin(115200);
  Serial.println("BinarySequencer...");

  pinMode(GateOutPin, OUTPUT);
  pinMode(GateOutPin + 1, OUTPUT);
  pinMode(GateOutPin + 2, OUTPUT);
  pinMode(GateOutPin + 3, OUTPUT);
  pinMode(GateOutPin + 4, OUTPUT);
  pinMode(GateOutPin + 5, OUTPUT);

  g_Controller[0].Begin(3,2,6);
  g_Controller[1].Begin(5,4,7);

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

  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    g_Controller[idx].s_Pattern.Reset(0x11);
    g_Controller[idx].s_Stepper.SetNumSteps(8);
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
  uint8_t Pattern = g_Controller[Idx].s_Pattern.Get();
  const int LedMatrixId = 0;
  const int Column = Idx * 4;
  g_LedControl.setColumn(LedMatrixId, Column, Pattern);
}

void ShowStep(int Idx)
{
  int Step = g_Controller[Idx].s_Stepper.GetStep();
  int Bit = g_Controller[Idx].s_Pattern.GetBit(Step);
  uint8_t Pattern = Bit << Step;

  const int LedMatrixId = 0;
  const int Column = 1 + Idx * 4;
  g_LedControl.setColumn(LedMatrixId, Column, Pattern);
}

void ShowNumSteps(int Idx)
{
  int NumSteps = g_Controller[Idx].s_Stepper.GetNumSteps();
  uint8_t Pattern = 0xFF >> (8 - NumSteps);

  const int LedMatrixId = 0;
  const int Column = 2 + Idx * 4;
  g_LedControl.setColumn(LedMatrixId, Column, Pattern);
}

void loop()
{
  //read rotary encoders
  for(int idx = 0; idx<NumPatterns; ++idx)
  {
    g_Controller[idx].Update();
  }

  //
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    ShowPattern(idx);
    ShowStep(idx);
    ShowNumSteps(idx);
  }
}
