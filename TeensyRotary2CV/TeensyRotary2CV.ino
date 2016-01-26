#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "RotaryEncoder.h"

const int Rotary1PinA = 2;
const int Rotary1PinB = Rotary1PinA + 1;

static const int NumEncoders = 2;
CRotaryEncoder Encoder[NumEncoders];

void Test()
{
  Serial1.println("Testing...");

  for(int Value = 0; Value<4096; Value += 1024)
  {
    Serial1.print("Value = ");
    Serial1.println(Value);
    mcp48_setOutput(0, GAIN_1, 1, Value);
    mcp48_setOutput(1, GAIN_1, 1, Value);

    delay(1000);
  }
}

void setup() 
{
  // put your setup code here, to run once:
  Serial1.begin(115200);
  delay(1000);
  Serial1.println("Teensy Midi2CV...");

  Serial1.println("Configuring...");
  for(int idx = 0; idx<NumEncoders; ++idx)
  {
    Encoder[idx].Begin(Rotary1PinA+idx*2, Rotary1PinB+idx*2);
  }
  
  mcp48_begin();  
  mcp48_setOutput(0, GAIN_1, 1, 2048);
  mcp48_setOutput(1, GAIN_1, 1, 2048); 
  delay(1000);

  Serial1.println("Start");
}

void loop() 
{
  //while(true)
  {
    Test();
  }


  int PrevPosition[NumEncoders] = {0};
  int PrevValue[NumEncoders] = {2048};
  
  while(true)
  {
    for(int idx = 0; idx<NumEncoders; ++idx)
    {
      // encoder position has no boundaries, but value must be in [0,4096[
      Encoder[idx].Read();
      int Position = Encoder[idx].GetPosition();

      if(PrevPosition[idx]!=Position)
      {
          //Serial1.print(idx);
          //Serial1.println(Position);
      
        int Value = PrevValue[idx] + 16*(Position - PrevPosition[idx]);
        if(Value<0)
        { 
          Value=0;
        }
        else if(4095<Value)
        {
          Value = 4095;
        }
        if(PrevValue[idx]!=Value)
        {
          mcp48_setOutput(idx, GAIN_1, 1, Value);//TODO support idx>1
          Serial1.print(idx);
          Serial1.print(" : Value = ");
          Serial1.println(Value);
          PrevValue[idx] = Value;        
        }

        PrevPosition[idx] = Position;
      } 
    }
    //Serial1.println();
    delay(1);//needed?????? 
  }
}

