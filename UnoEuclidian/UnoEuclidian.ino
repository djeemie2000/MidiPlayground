#include <Wire.h>
#include "Adafruit_LEDBackpack.h"

const int  g_NumLeds = 24;
Adafruit_24bargraph g_LedBar;


const int ClockInPin = 2;
const int ResetInPin = 3;

struct SController
{
  SController()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_Pattern = 0x1315;
    m_ActiveMask = 0xFFFF;
    m_CurrentStep = 0;
    m_Reset = false;
  }
  
  void Begin()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_CurrentStep = 0;
    m_Reset = false;
  }

  void OnClockRise()
  {
    ++m_ChangeCounter;
    Advance();
    //m_State = true;
  }

  void OnClockFall()
  {
    ++m_ChangeCounter;
    m_State = false;
  }

  void OnReset()
  {
    m_Reset = true;
    
    //++m_ChangeCounter;    
    //m_CurrentStep = 0;
    //m_State = (m_Pattern & (1<<m_CurrentStep));
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

  int m_ChangeCounter;
  bool m_State;
  
  uint32_t m_Pattern;
  uint32_t m_ActiveMask;
  int m_CurrentStep;
  bool m_Reset;
};

SController g_Controller;

void OnClockChange()
{
  if(digitalRead(ClockInPin)==HIGH)
  {
    g_Controller.OnClockRise();
  }
  else
  {
    g_Controller.OnClockFall();
  }
  
  digitalWrite(13, g_Controller.m_State?HIGH:LOW);
}

void OnResetRise()
{
  g_Controller.OnReset();
  digitalWrite(13, g_Controller.m_State?HIGH:LOW);
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Uno euclidian rythm generator...");
  pinMode(ClockInPin, INPUT);
  pinMode(13, OUTPUT);
  g_Controller.Begin();
  g_LedBar.begin(0x70);//default address
  g_LedBar.clear();
  g_LedBar.writeDisplay();

  // only 1 interrupt can be attached to a pin
  attachInterrupt(digitalPinToInterrupt(ClockInPin), OnClockChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ResetInPin), OnResetRise, RISING);
}

void ShowLeds()
{
  uint32_t Pattern = g_Controller.m_Pattern;
  uint32_t ActiveMask = g_Controller.m_ActiveMask;
  for(int Led = 0; Led<g_NumLeds; ++Led)
  {
    uint32_t LedMask = 1<<Led;
    if(ActiveMask & LedMask)
    { // active => filled or not
      if(Pattern & LedMask)
      {
        g_LedBar.setBar(Led, LED_RED);
      }
      else
      {
        g_LedBar.setBar(Led, LED_GREEN);
      }
    }
    else
    { // not active
      g_LedBar.setBar(Led, LED_OFF);
    }
  }

  g_LedBar.setBar(g_Controller.m_CurrentStep, LED_YELLOW);

  g_LedBar.writeDisplay();
}

void SerialDebug()
{
  static int ChangeCounter = 0;
  if(g_Controller.m_ChangeCounter != ChangeCounter)
  {
    ChangeCounter = g_Controller.m_ChangeCounter;
    
    Serial.print(ChangeCounter);
    Serial.print(" : Step ");
    Serial.print(g_Controller.m_CurrentStep);
    Serial.print(" ");
    if(g_Controller.m_State)
    {
      Serial.println("On");
    }
    else
    {
      Serial.println("Off");
    }
  }
}

void loop() 
{
  // the serial print below is for debug purposes:
  SerialDebug();

  // TODO read rotary encoders:
  // change mask length
  // change pattern fills
  // change pattern offset = bitwise rotation of pattern

  ShowLeds();
      
  delay(1);//needed?
}
