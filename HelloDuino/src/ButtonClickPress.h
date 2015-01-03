#pragma once

#include "ButtonHandler.h"
#include "PeriodicTrigger.h"

const int RedLedPin = 12;
const int GreenLedPin = 11;
const int ButtonPin = 10;

const int RedLedPin2 = 9;
const int GreenLedPin2 = 8;
const int ButtonPin2 = 7;

bool g_RedLedActive = false;
bool g_GreenLedActive = false;
bool g_RedLed2Active = false;
bool g_GreenLed2Active = false;



CButtonHandler ButtonHandler1;
CButtonHandler ButtonHandler2;
CPeriodicTrigger StepTrigger;


void setup()
{
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  pinMode(ButtonPin, INPUT_PULLUP);

  pinMode(RedLedPin2, OUTPUT);
  pinMode(GreenLedPin2, OUTPUT);
  pinMode(ButtonPin2, INPUT_PULLUP);

  pinMode(13, OUTPUT);

  Serial.begin(9600); 

  StepTrigger.Configure(1000, 100);

  for(int Repeat = 0; Repeat<5; ++Repeat)
  {
    digitalWrite(RedLedPin, HIGH);
    digitalWrite(GreenLedPin, LOW);
    digitalWrite(RedLedPin2, HIGH);
    digitalWrite(GreenLedPin2, LOW);
    delay(100);

    digitalWrite(RedLedPin, LOW);
    digitalWrite(GreenLedPin, HIGH);
    digitalWrite(RedLedPin2, LOW);
    digitalWrite(GreenLedPin2, HIGH);
    delay(100);
  }

  digitalWrite(RedLedPin, g_RedLedActive);
  digitalWrite(GreenLedPin, g_GreenLedActive);
  digitalWrite(RedLedPin2, g_RedLed2Active);
  digitalWrite(GreenLedPin2, g_GreenLed2Active);
}

void loop()
{
  bool Button1Pressed = (digitalRead(ButtonPin)==LOW);
  bool Button2Pressed = (digitalRead(ButtonPin2)==LOW);

  ButtonHandler1.Update(Button1Pressed);
  ButtonHandler2.Update(Button2Pressed);
  StepTrigger.Update();

  if(ButtonHandler1.ButtonClicked())
  { // toggle red led
    g_RedLedActive = !g_RedLedActive;
  }
  else if(ButtonHandler1.ButtonHold())
  {
    //toggle green led
    g_GreenLedActive = !g_GreenLedActive;
  }

  if(ButtonHandler2.ButtonClicked())
  { // toggle red led
    g_RedLed2Active = !g_RedLed2Active;
  }
  else if(ButtonHandler2.ButtonHold())
  {
    //toggle green led
    g_GreenLed2Active = !g_GreenLed2Active;
  }

  int RedLedValue = g_RedLedActive ? 128 : 0;
  analogWrite(RedLedPin, RedLedValue);
  digitalWrite(GreenLedPin, g_GreenLedActive ? HIGH : LOW);  

  int RedLedValue2 = g_RedLed2Active ? 128 : 0;
  analogWrite(RedLedPin2, RedLedValue2);
  digitalWrite(GreenLedPin2, g_GreenLed2Active ? HIGH : LOW);  

  if(StepTrigger.TriggerOn())
  {
    digitalWrite(13, HIGH);
  }
  else if(StepTrigger.TriggerOff())
  {
    digitalWrite(13, LOW);
  }

  delay(1);//delay 1 millisecond
}



