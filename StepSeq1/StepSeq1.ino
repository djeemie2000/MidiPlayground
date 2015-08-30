#include "Wire.h"
#include "LedControlMS.h"
#include "Ticker.h"

const int LedPin = 13;

bool ClockPulse;
int CurrentStep;
const int NumSteps = 8;
int StepValues[NumSteps];
int GateOutValues[NumSteps];

const int SelectPin = 7;
const int TriggerOutPin = 6;
int TriggerOutValue;

const int GateOutPin = 5;
const int CVInPin = A0;
const int StepSpeedInPin = A1;

const int Pcf8591Address = 0x48;

const int NumLedMatrices = 1;
const int DataPin = 12;
const int ClockPin = 11;
const int LoadPin = 10;
LedControl lc = LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);


CTicker Ticker;

void InitSteps()
{
  for(int idx=0; idx<NumSteps; ++idx)
  {
    StepValues[idx] = idx*32;
    GateOutValues[idx] = HIGH;
  }
}

void InitLeds()
{
  for(int idx = 0; idx<NumLedMatrices; ++idx)
  {
    /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
    */
    lc.shutdown(idx,false);
    /* Set the brightness to a low values */
    lc.setIntensity(idx,1);
    /* and clear the display */
    delay(1000);
    lc.clearDisplay(idx);
  }
}

void InitDAC()
{
  Wire.begin();
  Wire.beginTransmission(Pcf8591Address);
  uint8_t Error = Wire.endTransmission(Pcf8591Address);
  if(Error==0)
  {
    Serial.println("Found pcf8591");
  }
  else
  {
    Serial.println("Could not find pcf8591");
  }
}

void WriteDAC(int Value)
{
  Wire.beginTransmission(Pcf8591Address);
  Wire.write(0x40);//AOut
  Wire.write(Value>>2);//scale from 10 bits resolution down to 8 bits
  Wire.endTransmission();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(SelectPin, OUTPUT);
  pinMode(SelectPin+1, OUTPUT);
  pinMode(SelectPin+2, OUTPUT);

  pinMode(CVInPin, INPUT);
  pinMode(StepSpeedInPin, INPUT);

  pinMode(TriggerOutPin, OUTPUT);
  TriggerOutValue = 0;

  pinMode(GateOutPin, OUTPUT);
  digitalWrite(GateOutPin, HIGH);
  
  CurrentStep = 0;
  ClockPulse = false;
  InitSteps();

  InitDAC();
  InitLeds();

  Ticker.Reset();
  Ticker.SetPeriod(250/2);// as default: 120 bpm = 2 Hz = 500 msec period
  // note that toggle trigger pin should happen 2x each period

  // interrupt upon rising edge of digital input 2 (= interrupt number 0)
  attachInterrupt(0, OnClockPulse, RISING);
}

void ToggleTrigger()
{
    //toggle trigger
    TriggerOutValue = 255-TriggerOutValue;
    digitalWrite(TriggerOutPin, TriggerOutValue);
}

void UpdateStepSpeed()
{
  int Value = analogRead(StepSpeedInPin);
  Ticker.SetPeriod((1+Value)*8);//[8 msec, 8192 msec]
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Ticker.Tick()==CTicker::TickOn)
  {
    ToggleTrigger();
  }

  UpdateStepSpeed();

  if(ClockPulse)
  {
    AdvanceStep();

    ApplyStep();

    LogStep();

    UpdateLeds();

    //avoid bouncing?
    //delay(10);
    ClockPulse = false;
  }
}

void AdvanceStep()
{
  CurrentStep = (CurrentStep+1)%NumSteps;
  Serial.print("Advance to step ");
  Serial.println(CurrentStep);
}

void ApplyStep()
{
  // select channel on 4051
  int Value0 = CurrentStep & 0x01;
  int Value1 = CurrentStep>>1 & 0x01;
  int Value2 = CurrentStep>>2 & 0x01;
  digitalWrite(SelectPin, 0!=Value0 ? HIGH : LOW);
  digitalWrite(SelectPin+1, 0!=Value1 ? HIGH : LOW);
  digitalWrite(SelectPin+2, 0!=Value2 ? HIGH : LOW);
  // read analog value from analog input pin
  StepValues[CurrentStep] = analogRead(CVInPin);

  // write to DAC
  WriteDAC(StepValues[CurrentStep]);

  // gate
  digitalWrite(GateOutPin, GateOutValues[CurrentStep]);

  /*Serial.print(Value0);
  Serial.print(" ");
  Serial.print(Value1);
  Serial.print(" ");
  Serial.print(Value2);
  Serial.println();*/
}

void LogStep()
{
  int Value = StepValues[CurrentStep];
  Serial.print("Gate=");
  Serial.print(GateOutValues[CurrentStep]);
  Serial.print(" Value=");
  Serial.println(Value);
}

void UpdateLeds()
{
  int MatrixId = 0;
  lc.clearDisplay(MatrixId);//??

  int Column = 7-CurrentStep;
  // bottom row: step ~ gate on/off
  int GateRow = 0;
  lc.setLed(MatrixId, GateRow, Column, true);

  // one pixel ~CV [0,7]
  int CVRow = 1+StepValues[CurrentStep]>>7;
  for(int Row = 1; Row<=CVRow; ++Row)
  {
    lc.setLed(MatrixId, Row, Column, true);
  }
}

void OnClockPulse()
{
  ClockPulse = true;
}
