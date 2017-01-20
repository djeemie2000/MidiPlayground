#include "werkstatt.h"
#include "BitNoise.h"

const int AnalogInPin1 = A0;
const int NoiseOutPin = 4;

unsigned long g_DebugCounter;
unsigned long g_Millis;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x02);
}

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

void setup() 
{
  setupFastAnalogRead();

  Serial.begin(115200);
  Serial.println("BitNoise...");

  for(int Gen = 0; Gen<NumGenerators; ++Gen)
  {
    g_NoiseGen[Gen].Begin(NoiseOutPin+Gen);
    g_NoiseGen[Gen].SetColor(Gen+1);
  }

  TimingBitNoise();
  TimingUpdate();
  TimingDigitalWrite();
  TimingAnalogRead();
}

void Debug()
{
  // for debugging / timing purposes
  ++g_DebugCounter;
  unsigned long Interval = 10000;//1024*16;
  if(Interval<g_DebugCounter)
  {
    unsigned long currMillis = millis();
    Serial.println();
    Serial.println(currMillis-g_Millis);
    Serial.println(g_DebugCounter);
    g_DebugCounter = 0;
    g_Millis = currMillis;
  }
}

void TimingBitNoise()
{
  Serial.println("TimingBitNoise");

  unsigned long Before = millis();
  
  const int NumRepeats = 10000;
  int Dummy = 0;
  for(int Repeat = 0; Repeat<10000; ++Repeat)
  {
    Dummy += g_BitNoise.Generate();
  }

  unsigned long After = millis();
  
  Serial.print("Repeat ");
  Serial.print(NumRepeats);
  Serial.print(" -> ");
  Serial.println(After-Before);
  Serial.println(Dummy);
}

void TimingDigitalWrite()
{
  Serial.println("TimingDigitalWrite");

  unsigned long Before = millis();
  
  const int NumRepeats = 10000;
  for(int Repeat = 0; Repeat<10000; ++Repeat)
  {
    digitalWrite(NoiseOutPin, Repeat&1);
  }

  unsigned long After = millis();
  
  Serial.print("Repeat ");
  Serial.print(NumRepeats);
  Serial.print(" -> ");
  Serial.println(After-Before);
}

void TimingAnalogRead()
{
  Serial.println("TimingAnalogRead");

  unsigned long Before = millis();
  
  const int NumRepeats = 10000;
  int Dummy = 0;
  for(int Repeat = 0; Repeat<10000; ++Repeat)
  {
    Dummy += analogRead(AnalogInPin1);
  }

  unsigned long After = millis();
  
  Serial.print("Repeat ");
  Serial.print(NumRepeats);
  Serial.print(" -> ");
  Serial.println(After-Before);
  Serial.println(Dummy);
}


void TimingUpdate()
{
  Serial.println("TimingUpdate");

  unsigned long Before = millis();
  
  const int NumRepeats = 10000;
  for(int Repeat = 0; Repeat<10000; ++Repeat)
  {
    g_NoiseGen[0].Update(Repeat);
    g_NoiseGen[1].Update(Repeat);
    g_NoiseGen[2].Update(Repeat);
    g_NoiseGen[3].Update(Repeat);
  }

  unsigned long After = millis();
  
  Serial.print("Repeat ");
  Serial.print(NumRepeats);
  Serial.print(" -> ");
  Serial.println(After-Before);
}


void loop() 
{
  int GlobalColor = analogRead(AnalogInPin1);
  GlobalColor = 0;//debug!!!!

  // generate next bit
  int CurrentBit = g_BitNoise.Generate();

  // update generators, which all have their own color
  for(int Gen = 0; Gen<NumGenerators; ++Gen)
  {
    g_NoiseGen[Gen].Update(CurrentBit);
  }
  delayMicroseconds(GlobalColor);
  
  Debug();
}

