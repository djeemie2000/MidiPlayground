#include <EEPROM.h>
#include <Wire.h>
#include "LedControlMS.h"
#include "IntBinaryPattern.h"
#include "Stepper.h"
#include "TimerOne.h"
#include "RotaryEncoder.h"
#include "DigitalIn.h"
#include "Mcp23017.h"
#include "Adafruit_LEDBackpack.h"

const int NumLedMatrices = 1;
const int DataPin = 12;
const int ClockPin = 11;
const int LoadPin = 10;
LedControl g_LedControl(DataPin, ClockPin, LoadPin, NumLedMatrices);

Adafruit_24bargraph g_BarGraph;

CStepper<int> g_HiResStepper;

static const int NumPatterns = 2;

int g_Gate[NumPatterns];
uint8_t g_Outputs;
uint8_t g_Inputs;

static const int MaxNumSteps = 12;
static const int MinNumSteps = 1;

// gate outputs
static const int GateOutPin = A0;

// Speed
static const int HiResPeriod = 250;

void Save();//pre declaration

struct SController
{
  CRotaryEncoder s_Rotary;
  int s_RotaryPosition;
  CDigitalIn s_PushButton;
  int s_EditMode;
  CStepper<int> s_Stepper;

  isl::CBinaryPattern<uint16_t> s_Pattern;

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
        if (NumSteps <= MaxNumSteps)
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
        if (MinNumSteps <= NumSteps)
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
    g_Outputs = 0x0000;

    // advance steps
    for (int idx = 0; idx < NumPatterns; ++idx)
    {
      g_Controller[idx].s_Stepper.Advance();

      g_Gate[idx] = g_Controller[idx].s_Pattern.GetBit(g_Controller[idx].s_Stepper.GetStep()) ? 1 : 0;
    }
    // update (logical combination) gates
      g_Outputs |= g_Gate[0];
      g_Outputs |= g_Gate[1]<<1;
      
    if(g_Gate[0] && g_Gate[1])
    { // AND
      g_Outputs |= 1<<2;
    }
    if(g_Gate[0] || g_Gate[1] && g_Gate[0] != g_Gate[1])
    { // XOR
      g_Outputs |= 1<<3;
    }
    if(!g_Gate[0] && !g_Gate[1])
    { // NONE = 
      g_Outputs |= 1<<4;
    }
    if(!(g_Gate[0]&&g_Gate[1]))
    {// NAND
      g_Outputs |= 1<<5;
    }
    // begin of pattern
    if(g_Controller[0].s_Stepper.GetStep()==0)
    {// 
      g_Outputs |= 1<<6;
    }
    if(g_Controller[1].s_Stepper.GetStep()==0)
    {// 
      g_Outputs |= 1<<7;
    }
  }
  // halfway step => all gates off
  else if (HiResPeriod / 2 == g_HiResStepper.GetStep())
  {
    g_Outputs = 0x00;
  }
}

void Save()
{
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    EEPROM.write(idx * 3, g_Controller[idx].s_Pattern.Get() & 0xFF);
    EEPROM.write(idx * 3 + 1, g_Controller[idx].s_Pattern.Get() >> 8);
    EEPROM.write(idx * 3 + 2, g_Controller[idx].s_Stepper.GetNumSteps());
  }
}

void Load()
{
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    uint16_t Pattern = EEPROM.read(idx * 3) | (EEPROM.read(idx*3+1)<<8);
    g_Controller[idx].s_Pattern.Reset(Pattern);
    int NumSteps = min(MaxNumSteps, EEPROM.read(idx * 3 + 2));
    g_Controller[idx].s_Stepper.SetNumSteps(NumSteps);
  }
}


void setup()
{
  // put your setup code here, to run once:


  Serial.begin(115200);
  Serial.println("BinarySequencer 2...");
  
  g_Outputs = 0x00;
  g_Inputs = 0xFF;//only react upon rise

  Wire.begin();
  
  // set I/O pins bank A to outputs on mcp23017
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_IODIRA); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  
  // set I/O pins bank B to inputs on mcp23017
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_IODIRB); // IODIRB register
  Wire.write(0xFF); // set all of port B to intputs
  Wire.endTransmission();
  
  Wire.beginTransmission(MCP23017_ADDRESS);
  Wire.write(MCP23017_GPPUB); // pullup register
  Wire.write(0xFF); // set all of port B to pullup
  Wire.endTransmission();
  
  //
  g_BarGraph.begin(0x70);  // pass in the address
  g_BarGraph.clear();
  g_BarGraph.setBrightness(2);
  g_BarGraph.writeDisplay();
  //

  //
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

  Serial.println("Starting");
  unsigned long TimerPeriodMicroSeconds = 1000;// 2*120 bpm
  Timer1.initialize(TimerPeriodMicroSeconds);
  Timer1.attachInterrupt(OnTick);
}

void ShowPattern(int Idx)
{
  uint16_t Pattern = g_Controller[Idx].s_Pattern.Get();  
  const int LedMatrixId = 0;
  const int Column = Idx * 4;
  g_LedControl.setColumn(LedMatrixId, Column, Pattern);
}

void ShowStep(int Idx)
{  
  int Step = g_Controller[Idx].s_Stepper.GetStep();
  uint16_t Pattern = 1 << Step;
  
  const int LedMatrixId = 0;
  const int Column = 1 + Idx * 4;
  g_LedControl.setColumn(LedMatrixId, Column, Pattern);
}

void ShowBargraph(int Idx)
{
  int Offset = Idx ? 12 : 0;//0 or 16
  uint16_t Pattern = g_Controller[Idx].s_Pattern.Get();  
  int Step = g_Controller[Idx].s_Stepper.GetStep();
  int PatternLength = g_Controller[Idx].s_Stepper.GetNumSteps();
  for(int Bar = 0; Bar<MaxNumSteps; ++Bar)
  {
    uint8_t Color = LED_OFF;
    if(Bar==Step)
    {
      Color = LED_YELLOW;
    }
    else //if(Bar<PatternLength)
    {
      uint16_t Mask = 1<<Bar;
      if(Pattern & Mask)
      {
        Color = LED_RED;
      }
      else
      {
        Color = LED_OFF;//LED_GREEN;
      }
    }
    g_BarGraph.setBar(Bar+Offset, Color);  
  }
  g_BarGraph.writeDisplay();  
}

void OnInputRise(int Bit)
{
  Serial.print(Bit);
  Serial.println(" input rise");
  
  if(Bit == 0)
  {
    g_Controller[0].s_Pattern.Next();
  }
  else if(Bit==1)
  {
    g_Controller[0].s_Pattern.Prev();
  }
  else if(Bit==2)
  {
    g_Controller[0].s_Pattern.RotateNext();
  }
  else if(Bit==3)
  {
    g_Controller[0].s_Pattern.RotatePrev();
  }
  else if(Bit==4)
  {
       g_Controller[1].s_Pattern.Next();
  }
  else if(Bit==5)
  {
    g_Controller[1].s_Pattern.Prev();
  }
  else if(Bit==6)
  {
    g_Controller[1].s_Pattern.RotateNext();
  }
  else if(Bit==7)
  {
    g_Controller[1].s_Pattern.RotatePrev();
  } 
}

void loop()
{
  // update user input
  for(int idx = 0; idx<NumPatterns; ++idx)
  {
    g_Controller[idx].Update();
  }

  // update outputs
   Wire.beginTransmission(MCP23017_ADDRESS);
   Wire.write(MCP23017_GPIOA); // GPIOA
   Wire.write(g_Outputs); // port A
   Wire.endTransmission();
   
   // read inputs
   uint8_t PrevInputs = g_Inputs;
   Wire.beginTransmission(MCP23017_ADDRESS);
   Wire.write(MCP23017_GPIOB); // GPIOB
   Wire.endTransmission();
   Wire.requestFrom(MCP23017_ADDRESS, 1);//request 1 byte
   g_Inputs = Wire.read();
   
   // TODO process inputs -> only react upon rise
   if(PrevInputs != g_Inputs)
   {
     //TODO
     for(int Bit = 0; Bit<8; ++Bit)
     {
       uint8_t Mask = 1<<Bit;
       if( !(PrevInputs & Mask) && (g_Inputs & Mask))
       {
         // rise on Bit
         OnInputRise(Bit);
       }
     }
   }
   //Serial.println(g_Inputs, HEX);//Debug!!!
   
  // update leds 
  for (int idx = 0; idx < NumPatterns; ++idx)
  {
    //ShowPattern(idx);
    //ShowStep(idx);
    ShowBargraph(idx);
  }
}
