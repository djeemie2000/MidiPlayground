#include <Wire.h>
#include <SPI.h>
#include "UnoMcpDac.h"
#include "IntMultiStageEnvelope2.h"
#include "TimerOne.h"
#include "CapacitiveTouchPad.h"
#include "LedControlMS.h"


int IrqPin = 8;  // Digital 2
boolean touchStates[12]; //to keep track of the previous touch states
CCapacitiveTouchPad g_TouchPad;

const int NumLedMatrices = 1;
const int DataPin = 7;
const int ClockPin = 6;
const int LoadPin = 5;
LedControl g_LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);


static const unsigned long SamplingFrequency = 1<<14;

static const int DurationScale = 14;//16;//[0,65353[
static const int NumEnvelopeStages = 4;
typedef isl::CMultiStageEnvelope2<long, NumEnvelopeStages, DurationScale> EnvelopeType;
EnvelopeType g_Envelope;

static const int TargetInPin = A0;

void WriteDac()
{
  unsigned int DacValue = g_Envelope();
  mcp48dac::SetOutput(DacValue, mcp48dac::Channel_A, mcp48dac::Gain_x1);
  unsigned int SteppedValue = g_Envelope.GetStepped();
  mcp48dac::SetOutput(SteppedValue, mcp48dac::Channel_B, mcp48dac::Gain_x1);
}

void TestAccuracy()
{
  //high-res saw wave
  for (unsigned int DacValue = 0; DacValue < 4096; DacValue += 32)
  {
    mcp48dac::SetOutput(DacValue, mcp48dac::Channel_A, mcp48dac::Gain_x1);
    
    int ValueA = analogRead(A0);
    int ValueB = analogRead(A1);
    Serial.print("DAC out=");
    Serial.print(DacValue);
    Serial.print(" ADC in=");
    Serial.print(ValueA);
    Serial.print(" ");
    Serial.print(ValueB);
    Serial.println();

    delay(250);
  }
}

void TestDacSpeed()
{
  unsigned long Number = SamplingFrequency;
  unsigned long Before = millis();

  //high-res saw wave
  for (unsigned long Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
    unsigned int DacValue = Repeat%(1<<12);
    mcp48dac::SetOutput(DacValue, mcp48dac::Channel_A, mcp48dac::Gain_x1);
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;

  Serial.print("DAC out x ");
  Serial.print(Number);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void TestEnvelopeSpeed()
{
  unsigned long Number = SamplingFrequency;
  unsigned long Before = millis();

  long Tmp = 0;
  for (unsigned long Repeat = 0; Repeat < SamplingFrequency; ++Repeat)
  {
      Tmp += g_Envelope();
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;

  Serial.print("envelope calc x ");
  Serial.print(Number);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec (");
  Serial.print(Tmp);
  Serial.println();
}

void ShowAction(int Stage, bool Gate)
{
   EnvelopeType::EAction Action = g_Envelope.GetAction(Stage, Gate);

      const int LedMatrixId = 0;
      int Row = 7-Stage;
      int Column = Gate ? 0 : 4;
      g_LedControl.setLed(LedMatrixId, Row, Column, Action==EnvelopeType::AdvanceAction);
      g_LedControl.setLed(LedMatrixId, Row, Column+1, Action==EnvelopeType::HoldAction);
      g_LedControl.setLed(LedMatrixId, Row, Column+2, Action==EnvelopeType::ResetAction);
}

void setup()
{
  Serial.begin(115200);
  Serial.println("UnoMultiStageEnvelope...");

  for(int idx = 0; idx<NumEnvelopeStages; ++idx)
  {
    pinMode(TargetInPin+idx, INPUT);
  }

  for(int idx = 0; idx<g_TouchPad.GetNumPads(); ++idx)
  {
    touchStates[idx] = false;
  }
  g_TouchPad.Begin(IrqPin);//, 0x60, 0x80);

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

  for(int Stage = 0; Stage<NumEnvelopeStages; ++Stage)
  {
    g_Envelope.SetDuration(Stage, SamplingFrequency/2);
    g_Envelope.SetTarget(Stage, Stage%2 ? 0 : 4095);
    g_Envelope.SetAction(Stage, true, EnvelopeType::AdvanceAction);
    g_Envelope.SetAction(Stage, false, EnvelopeType::HoldAction);

    ShowAction(Stage, true);
    ShowAction(Stage, false);
  }

  mcp48dac::Begin();

  Serial.println("Testing...");
  TestDacSpeed();
  TestEnvelopeSpeed();
 
  
  Serial.println("Starting...");
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
}

void ToggleAction(int Stage, bool Gate)
{
   EnvelopeType::EAction OldAction = g_Envelope.GetAction(Stage, Gate);
   g_Envelope.ToggleAction(Stage, Gate);
   
   // set led matrix accordingly
   ShowAction(Stage, Gate);

   Serial.print("Toggled action ");
   Serial.print(Stage);
   Serial.print(" ");
   Serial.print(Gate?1:0);
   Serial.print(" from ");   
   Serial.println(static_cast<int>(OldAction));
//   Serial.print(" to ");
//   Serial.println(static_cast<int>(NewAction));
}

void OnButtonChange(int Button, bool Pressed)//pressed or released
{
  if(Pressed)
  {
    if(Button==11)
    {
      g_Envelope.NoteOn();
    }
    else if(Button==0)
    {
      //toggle stage 0 gate off action
      ToggleAction(0, false);
    }
    else if(Button==1)
    {
      //toggle stage 0 gate off action
     ToggleAction(0, true);
    }
    else if(Button==2)
    {
      ToggleAction(1, false);
    }
    else if(Button==3)
    {
      ToggleAction(1, true);
    }
    else if(Button==5)
    {
      ToggleAction(2, false);
    }
    else if(Button==6)
    {
      ToggleAction(2, true);
    }
    else if(Button==7)
    {
      ToggleAction(3, false);
    }
    else if(Button==8)
    {
      ToggleAction(3, true);
    }
    
    Serial.print("Pad ");
    Serial.print(Button);
    Serial.println(" was just touched");
  }
  else
  {
    if(Button==11)
    {
      g_Envelope.NoteOff();
    }
    Serial.print("Pad ");
    Serial.print(Button);
    Serial.println(" was just released");
  }
}

void ShowPattern()
{
    bool Gate = g_Envelope.GetGate();
    int Stage = g_Envelope.GetStage();
  
    uint8_t GateOnPattern = Gate ? (1<<Stage): 0x00;
    uint8_t GateOffPattern = Gate ? 0x00 : (1<<Stage);
    
    const int LedMatrixId = 0;
    g_LedControl.setColumn(LedMatrixId, 3, GateOnPattern);  
    g_LedControl.setColumn(LedMatrixId, 7, GateOffPattern);  
}

void loop()
{
    g_TouchPad.Read();
    for(int Pad = 0; Pad<g_TouchPad.GetNumPads(); ++Pad)
    {
      if(touchStates[Pad]!=g_TouchPad.Get(Pad))
      {
        touchStates[Pad] = g_TouchPad.Get(Pad);

        OnButtonChange(Pad, touchStates[Pad]);
      }
    }

    for(int idx = 0; idx<NumEnvelopeStages; ++idx)
    {
      int Value = analogRead(TargetInPin+idx); // [0,1024[
      g_Envelope.SetTarget(idx, Value<<2); // [0, 4096[
    }

    ShowPattern();
    
    delay(1);//???
 }

