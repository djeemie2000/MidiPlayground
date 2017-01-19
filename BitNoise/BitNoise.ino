#include "werkstatt.h"
#include "BitNoise.h"

const int AnalogInPin1 = A0;
const int NoiseOutPin = 4;

unsigned long g_DebugCounter;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x02);
}

//noise g_Noise(NoiseOutPin); 
CBitNoise g_BitNoise;

struct SNoiseGen
{  
  int s_Bit;
  int s_Color;
  int s_Cntr;
  int s_NoisePin;

  void Begin(int NoisePin)
  {
    s_Bit = 0;
    s_Color = 0;
    s_Cntr = 0;
    s_NoisePin = NoisePin;

    pinMode(s_NoisePin, OUTPUT);
  }
  
  void Update(int CurrentBit)
  {
    ++s_Cntr;
    if(s_Color<=s_Cntr)
    {
      s_Bit = CurrentBit;
      s_Cntr = 0;
    }

    digitalWrite(s_NoisePin, s_Bit?HIGH:LOW);
  }
  
  void SetColor(int Color)
  {
    s_Color = Color;
  }
};

const int NumGenerators = 4;
SNoiseGen g_NoiseGen[NumGenerators];
int g_CurrentRead;

void setup() 
{
  setupFastAnalogRead();

  Serial.begin(115200);
  Serial.println("BitNoise...");

//  pinMode(NoiseOutPin, OUTPUT);
  for(int Gen = 0; Gen<NumGenerators; ++Gen)
  {
    g_NoiseGen[Gen].Begin(NoiseOutPin+Gen);
    g_NoiseGen[Gen].SetColor(Gen+1);
  }
  g_CurrentRead = 0;
}

void Debug()
{
  // for debugging / timing purposes
  ++g_DebugCounter;
  unsigned long Interval = 1024*16;
  if(Interval<g_DebugCounter)
  {
    Serial.println();
    Serial.println(millis());
    Serial.println(g_DebugCounter);
    g_DebugCounter = 0;
  }
}

void loop() 
{
  // read one color at a time
  //g_NoiseGen[g_CurrentRead].SetColor(analogRead(AnalogInPin1+g_CurrentRead));
  //g_CurrentRead = (g_CurrentRead+1)%NumGenerators;
  //g_Noise.generate(Color1); 

  int Color = analogRead(AnalogInPin1+g_CurrentRead);

  // generate next bit
  int CurrentBit = g_BitNoise.Generate();

  // update generators, which all have their own color
  for(int Gen = 0; Gen<NumGenerators; ++Gen)
  {
    g_NoiseGen[Gen].Update(CurrentBit);
  }
    delayMicroseconds(Color);
//  ++g_Cntr;
//  if(g_Color<=g_Cntr)
//  {
//    g_Bit = CurrentBit;
//    g_Cntr = 0;
//  }
//  digitalWrite(NoiseOutPin, g_NoiseGen.s_Bit?HIGH:LOW);
  
  Debug();
}

