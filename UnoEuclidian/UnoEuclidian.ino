#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "BoundedValue.h"
#include "Euclidian.h"
#include "RotaryEncoder.h"
#include "DigitalIn.h"

const int  g_NumLeds = 24;
Adafruit_24bargraph g_LedBar;
CRotaryEncoder g_EncoderA;
CRotaryEncoder g_EncoderB;
CDigitalIn g_PushButtonA;
CDigitalIn g_PushButtonB;

const int ClockInPin = 2;
const int ResetInPin = 3;

const int EncoderAPinA = 4;
const int EncoderAPinB = 5;
const int PushButtonAPin = 6;

const int EncoderBPinA = 7;
const int EncoderBPinB = 8;
const int PushButtonBPin = 9;

const int ClockOutAPin = 10;
const int ClockOutBPin = 11;


struct SController
{
  SController(int MaxPatternLength)
   : m_MaxPatternLength(MaxPatternLength)
   , m_PatternLength()
   , m_PatternFills()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_Pattern = 0x1315;
    m_ActiveMask = 0xFFFF;
    m_CurrentStep = 0;
    m_Reset = false;
    m_PatternLength.Set(16);
    m_PatternFills.Set(7);
  }
  
  void Begin()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_CurrentStep = 0;
    m_Reset = false;

    m_PatternLength.SetInterval(1, m_MaxPatternLength);
    m_PatternLength.Set(m_MaxPatternLength);
    m_PatternFills.SetInterval(0, m_PatternLength.Get());
    m_PatternFills.Set(m_PatternLength/3-2);
    m_ActiveMask = euclidian::CalcActiveMask(m_PatternLength);
    m_Pattern = euclidian::CalcPattern(m_PatternLength, m_PatternFills);
  }

  void OnClockRise()
  {
    ++m_ChangeCounter;
    Advance();
  }

  void OnClockFall()
  {
    ++m_ChangeCounter;
    m_State = false;
  }

  void OnReset()
  {
    m_Reset = true;
  }

  void Advance()
  {
    if(m_Reset)
    {
      m_CurrentStep = 0;
      m_Reset = false;
    }
    else
    {
      ++m_CurrentStep;
      if(!(m_ActiveMask&(1<<m_CurrentStep)))
      {
        m_CurrentStep = 0;
      }
    }

    m_State = (m_Pattern & (1<<m_CurrentStep));
  }

  void UpdatePattern(int DeltaLength, int DeltaFills)
  {
    if(DeltaLength)
    {
      //Serial.println("Length changed");
      m_PatternLength.SetChange(DeltaLength);       
      m_PatternFills.SetInterval(0, m_PatternLength.Get());
    }
      
    if(DeltaFills)
    {
    //Serial.println("Fills changed");
      m_PatternFills.SetChange(DeltaFills);
    }

    if(DeltaFills || DeltaLength)
    {  
      m_ActiveMask = euclidian::CalcActiveMask(m_PatternLength);
      m_Pattern = euclidian::CalcPattern(m_PatternLength, m_PatternFills);
    }
  }

  const int m_MaxPatternLength;

  int m_ChangeCounter;
  
  bool m_State;
  
  uint32_t m_Pattern;
  uint32_t m_ActiveMask;
  int m_CurrentStep;
  bool m_Reset;

  CBoundedValue<int> m_PatternLength;
  CBoundedValue<int> m_PatternFills;
};

SController g_ControllerA(16);
int g_DebugChangeCounterA;

SController g_ControllerB(8);
int g_DebugChangeCounterB;

bool g_EditA;//true => editing A, false => editing B


void OnClockChange()
{
  if(digitalRead(ClockInPin)==HIGH)
  {
    g_ControllerA.OnClockRise();
    g_ControllerB.OnClockRise();
  }
  else
  {
    g_ControllerA.OnClockFall();
    g_ControllerB.OnClockFall();
  }
  
  //digitalWrite(13, g_ControllerA.m_State?HIGH:LOW);
  digitalWrite(ClockOutAPin, g_ControllerA.m_State?HIGH:LOW);
  digitalWrite(ClockOutBPin, g_ControllerB.m_State?HIGH:LOW);
}

void OnResetRise()
{
  g_ControllerA.OnReset();
  g_ControllerB.OnReset();
//  digitalWrite(13, g_ControllerA.m_State?HIGH:LOW);
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Uno euclidian rythm generator...");
  pinMode(ClockInPin, INPUT);
  pinMode(ResetInPin, INPUT_PULLUP);
  pinMode(ClockOutAPin, OUTPUT);
  pinMode(ClockOutBPin, OUTPUT);  
  //pinMode(13, OUTPUT);
  g_EditA = false;
  g_ControllerA.Begin();
  g_DebugChangeCounterA = 0;
  g_ControllerB.Begin();
  g_DebugChangeCounterB = 0;
  g_EncoderA.Begin(EncoderAPinA, EncoderAPinB);
  g_PushButtonA.Begin(PushButtonAPin);
  g_EncoderB.Begin(EncoderBPinA, EncoderBPinB);
  g_PushButtonB.Begin(PushButtonBPin);
  g_LedBar.begin(0x70);//default address
  g_LedBar.clear();
  g_LedBar.writeDisplay();

  // only 1 interrupt can be attached to a pin
  attachInterrupt(digitalPinToInterrupt(ClockInPin), OnClockChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ResetInPin), OnResetRise, RISING);
}

void ShowLeds(const SController& Controller, int Offset)
{
  uint32_t Pattern = Controller.m_Pattern;
  uint32_t ActiveMask = Controller.m_ActiveMask;
  for(int Led = 0; Led<Controller.m_MaxPatternLength; ++Led)
  {
    uint32_t LedMask = 1<<Led;
    if(ActiveMask & LedMask)
    { // active => filled or not
      if(Pattern & LedMask)
      {
        g_LedBar.setBar(Led+Offset, LED_RED);
      }
      else
      {
        g_LedBar.setBar(Led+Offset, LED_GREEN);
      }
    }
    else
    { // not active
      g_LedBar.setBar(Led+Offset, LED_OFF);
    }
  }

  g_LedBar.setBar(Controller.m_CurrentStep+Offset, LED_YELLOW);

  g_LedBar.writeDisplay();
}

void SerialDebug(const SController& Controller, int& DebugChangeCounter)
{
  if(Controller.m_ChangeCounter != DebugChangeCounter)
  {
    DebugChangeCounter = Controller.m_ChangeCounter;
    
    Serial.print(DebugChangeCounter);
    Serial.print(" : Step ");
    Serial.print(Controller.m_CurrentStep);
    Serial.print(" ");
    if(Controller.m_State)
    {
      Serial.println("On");
    }
    else
    {
      Serial.println("Off");
    }

    if(Controller.m_State)
    {
      Serial.print("L ");
      Serial.print(Controller.m_PatternLength);
      Serial.print(" F ");
      Serial.println(Controller.m_PatternFills);
    
      Serial.println(Controller.m_ActiveMask, BIN);
      Serial.println(Controller.m_Pattern, BIN);
    }
  }
}

void loop() 
{
  // the serial print below is for debug purposes:
  SerialDebug(g_ControllerA, g_DebugChangeCounterA);
  SerialDebug(g_ControllerB, g_DebugChangeCounterB);

  // read rotary encoders:
  g_EncoderA.Read();
  g_PushButtonA.Read();
  g_EncoderB.Read();
  g_PushButtonB.Read();

  if(g_PushButtonA.IsPressed())
  {
    g_EditA = true;// Edit A
  }
  else if(g_PushButtonB.IsPressed())
  {
    g_EditA = false;// Edit B
  }
  //else -> unchanged
  
  // change mask length
  // change pattern fills
  // change pattern offset = bitwise rotation of pattern
  if(g_EditA)
  {
    g_ControllerA.UpdatePattern(g_EncoderA.GetChange(), g_EncoderB.GetChange());
  }
  else
  {
    g_ControllerB.UpdatePattern(g_EncoderA.GetChange(), g_EncoderB.GetChange());
  }

  ShowLeds(g_ControllerA, 0);
  ShowLeds(g_ControllerB, 16);
      
  delay(1);//needed?
}
