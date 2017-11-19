#include <SPI.h>
#include "UnoMcpDac.h"

#include "IntDelayLine.h"

const int AnalogPinAudioIn = A0;
const int AnalogPinSleepIn = A1;
const int NumDacs = 1;

#define DELAY_LINE_8BIT 1
//Note: could make it byte instead of unsigned int => 8 bit but doubles the capacity

const int DelayLineCapacity = 1024+512;
isl::CDelayLine<byte, DelayLineCapacity> g_DelayLine;

unsigned int g_DelayTime;

//debug
unsigned int g_DebugCounter;
unsigned long g_Millis;


void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x04);
}

void TestMcp()
{
  Serial.println("Testing DAC...");
  for(int Repeat = 0; Repeat<2; ++Repeat)
  {
    mcp48dac::SetOutput(4095, mcp48dac::Channel_A, mcp48dac::Gain_x1);// _x2 ???
    delay(500);
    mcp48dac::SetOutput(2048, mcp48dac::Channel_A, mcp48dac::Gain_x1);// _x2 ???
    delay(500);
    mcp48dac::SetOutput(0, mcp48dac::Channel_A, mcp48dac::Gain_x1);// _x2 ???
    delay(500);
  }
  Serial.println("Done!");
}

void setup()
{
  g_DebugCounter = 0;
  g_Millis = 0;

  g_DelayTime = 0;
  
  pinMode(AnalogPinAudioIn, INPUT);
  pinMode(AnalogPinSleepIn, INPUT);
  setupFastAnalogRead();

  mcp48dac::Begin(NumDacs);  
  
  Serial.begin(115200);
  Serial.println("AudioDelay...");

  //TestMcp();
}

void Debug()
{
  ++g_DebugCounter;
  if(g_DebugCounter>=DelayLineCapacity)
  {
    Serial.print(g_DebugCounter);
    Serial.print(" ");
    unsigned long PrevMillis = g_Millis;
    g_Millis = millis();
    unsigned long Elapsed = g_Millis - PrevMillis;
    Serial.println(Elapsed);

    float FreqHz = float(DelayLineCapacity)/Elapsed;
    Serial.println(FreqHz);

    g_DebugCounter = 0;
  }  
}

void loop()
{
  // [0,1024[ -> [0,1024+512[ delay time
  unsigned int DelayTime = 3*analogRead(AnalogPinSleepIn)/2;
  
  // smooth delay time
  g_DelayTime = (7*g_DelayTime + DelayTime)>>3;
  unsigned int SmoothDelayTime = g_DelayTime;
  //unsigned int SmoothDelayTime = g_DelayTime(DelayTime);

  // 10 bits input -> 8 bits delay line
  unsigned int AudioIn = analogRead(AnalogPinAudioIn);
  byte Value = AudioIn>>2;   
  g_DelayLine.Write(Value);
  
  // 8 bits delay line -> 12 bits output
  unsigned int AudioOut = g_DelayLine.Read(SmoothDelayTime)<<4; 

  // write audio out to DAC
  mcp48dac::SetOutput(AudioOut, mcp48dac::Channel_A, mcp48dac::Gain_x1);

  // short sleep to obtain the desired sampling frequency
  const int SleepTime = 128;//96;//???????????????????
  delayMicroseconds(SleepTime);
 
  //Debug();  
}

