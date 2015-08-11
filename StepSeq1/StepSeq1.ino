#include "Wire.h"
#include "LedControlMS.h"


const int LedPin = 13;
bool ClockPulse;
int CurrentStep;
const int NumSteps = 8;
int StepValues[NumSteps];

const int SelectPin = 7;

const int Pcf8591Address = 0x48;

const int NumLedMatrices = 1;
const int DataPin = 12;
const int ClockPin = 11;
const int LoadPin = 10;
LedControl lc = LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);

void InitSteps()
{
  for(int idx=0; idx<NumSteps; ++idx)
  {
    StepValues[idx] = idx*32;
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
  Wire.write(Value);
  Wire.endTransmission();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(SelectPin, OUTPUT);
  pinMode(SelectPin+1, OUTPUT);
  pinMode(SelectPin+2, OUTPUT);

  pinMode(A0, INPUT);
  
  CurrentStep = 0;
  ClockPulse = false;

  InitDAC();
  InitLeds();
  
  // interrupt upon rising edge of digital input 2 (= interrupt number 0)
  attachInterrupt(0, OnClockPulse, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ClockPulse)
  {
    AdvanceStep();

    ApplyStep();

    LogStep();

    UpdateLeds();

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
  StepValues[CurrentStep] = analogRead(A0);

  // write to DAC
  WriteDAC(StepValues[CurrentStep]);

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
  Serial.print("Value=");
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
  //avoid bouncing?
  delay(10);
}

