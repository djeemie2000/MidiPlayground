#include <SPI.h>
#include "UnoMcpDac.h"

#include "IntDelayLine.h"

const int AnalogPinAudioIn = A0;
const int AnalogPinDelayIn = A1;
const int NumDacs = 1;

#define DELAY_LINE_8BIT 1
//Note: could make it byte instead of unsigned int => 8 bit but doubles the capacity

#ifdef DELAY_LINE_8BIT
const int DelayLineCapacity = 1024+256;
isl::CDelayLine<byte, DelayLineCapacity> g_DelayLine;
#else
const int DelayLineCapacity = 512+128;
isl::CDelayLine<unsigned int, DelayLineCapacity> g_DelayLine;
#endif

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
  
  pinMode(AnalogPinAudioIn, INPUT);
  pinMode(AnalogPinDelayIn, INPUT);
  setupFastAnalogRead();

  mcp48dac::Begin(NumDacs);  
  
  Serial.begin(115200);
  Serial.println("AudioDelay...");

  TestMcp();
}

void Debug()
{
  ++g_DebugCounter;
  if(g_DebugCounter>20000)
  {
    Serial.print(g_DebugCounter);
    Serial.print(" ");
    unsigned long PrevMillis = g_Millis;
    g_Millis = millis();
    unsigned long Elapsed = g_Millis - PrevMillis;
    Serial.println(Elapsed);

    g_DebugCounter = 0;
  }  
}

void loop()
{
#ifdef DELAY_LINE_8BIT
  //10 bits input 8 bits delay line
  byte Value = analogRead(AnalogPinAudioIn)>>2;  
  g_DelayLine.Write(Value);
  // 8 bits delay line 12 bits output
  mcp48dac::SetOutput(g_DelayLine.Read(DelayLineCapacity-2)<<4, mcp48dac::Channel_A, mcp48dac::Gain_x1);
#else
  //10 bits input 12 bits output
  unsigned int Value = analogRead(AnalogPinAudioIn)<<2;  
  g_DelayLine.Write(Value);
  mcp48dac::SetOutput(g_DelayLine.Read(DelayLineCapacity-2), mcp48dac::Channel_A, mcp48dac::Gain_x1);
#endif

  // read sleep time from 2nd AnalogIn 
  int SleepTime = analogRead(AnalogPinDelayIn);
  delayMicroseconds(SleepTime);

  // TODO read actual delay from analog in
  // TODO read sleep time and delay alternating -> g_
 
  // Debug();  
}

