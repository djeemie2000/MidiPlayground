

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

const int ButtonPin = 7; 

#define ENC_PORT PIND //PIND is the bank where pin 2 and 3 are on
const int EncoderPinA = 2;
const int EncoderPinB = 3;
const int InteruptPin = EncoderPinA;

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack
volatile uint32_t RotaryCode;
volatile int Position;
volatile int InterruptCount;

void setup() 
{
  RotaryCode = 0x00;
  Position = 0;
  InterruptCount = 0;
  
  pinMode(ButtonPin, INPUT_PULLUP);
  pinMode(EncoderPinA, INPUT_PULLUP);
  pinMode(EncoderPinB, INPUT_PULLUP);
  
  //interupt handling for int.0 = pin2
  attachInterrupt(0, OnInterupt, RISING);
  
  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);//backlight on
}

void loop() 
{  
  bool ButtonPressed = (LOW==digitalRead(ButtonPin));
  if(ButtonPressed)
  {
    Position = 0;
  }
  // display update
  lcd.home (); // set cursor to 0,0

  lcd.print("P ");
  lcd.print(Position);
  lcd.print("      ");
  
  lcd.setCursor(0,1);
  lcd.print(RotaryCode, HEX);
  
  lcd.print(" ");
  lcd.print(InterruptCount);
  lcd.print(" ");
}

void OnInterupt()
{
  //delayMicroseconds(1500);// wait for bounces to stabilize. choose a delay that is not too long, and not too small
  uint8_t CurrentRotaryCode = ( ENC_PORT >> 2 & 0x03  );    
  //if(CurrentRotaryCode != (RotaryCode & 0x000003))
  {
    if( CurrentRotaryCode == 0x03)// || CurrentRotaryCode == 0x00 )
    {
      ++Position;
    }
    else if(CurrentRotaryCode == 0x01)// || CurrentRotaryCode == 0x01)
    {
      --Position;
    }
    RotaryCode = (RotaryCode << 4) | CurrentRotaryCode;//allow hex based print of last 4 steps
  }
  
  ++InterruptCount;
}

