#include "BitNoise.h"

const int AnalogInPin1 = A0;
const int NoiseOutPin = 3;

unsigned int g_DebugCounter;
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

// pins 0-7 are on port D
void DigitalOutPortD(int Pin, int Value)
{
  // TODO support other ports
  if(Value)
  {
    PORTD |= (1<<Pin);
  }
  else
  {
    PORTD &= ~(1<<Pin);
  }
}


CBitNoise g_BitNoise;

struct SNoiseGen
{  
  int s_Color;
  int s_Cntr;
  int s_NoisePin; 

  void Begin(int NoisePin)
  {
    s_Color = 0;
    s_Cntr = 0;
    s_NoisePin = NoisePin-1;

    pinMode(s_NoisePin, OUTPUT);
  }
  
  void Update(int CurrentBit)
  {
    ++s_Cntr;
    if(s_Color<s_Cntr)
    {
      DigitalOutPortD(s_NoisePin, CurrentBit);
      s_Cntr = 0;
    }
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
    g_NoiseGen[Gen].SetColor(Gen);
  }

//  TimingBitNoise();
//  TimingUpdate();
//  TimingDigitalWrite();
//  TimingAnalogRead();
}

void Debug()
{
  // for debugging / timing purposes
  ++g_DebugCounter;
  unsigned long Interval = 10000;//1024*16;
  if(Interval<=g_DebugCounter)
  {
    unsigned long currMillis = millis();
    Serial.println();
    Serial.println(currMillis-g_Millis);
    Serial.println(g_DebugCounter);
    g_DebugCounter = 0;
    g_Millis = millis();
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
    g_BitNoise.Generate();
    Dummy += g_BitNoise.Get();
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
    //digitalWrite(NoiseOutPin, Repeat&1);
    DigitalOutPortD(3, Repeat&1);
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
  int AnalogRead = 0;
  int GlobalColor = 0;
  while(true)
  {
    // do one analog read at a time:
    // global color, local color for each generator
    if(AnalogRead == 0)
    {
      GlobalColor = analogRead(AnalogInPin1);
      //GlobalColor = 0;//debug!!!!
    }
    else
    {
      // color/divider/octave: [0,16[
      int LocalColor = analogRead(AnalogInPin1+AnalogRead)>>6;
      g_NoiseGen[AnalogRead-1].SetColor(LocalColor);
    }
    ++AnalogRead;
    if(NumGenerators<AnalogRead)
    {
      AnalogRead = 0;
    }
    
    // generate next bit
    g_BitNoise.Generate();
    int CurrentBit = g_BitNoise.Get();
    
    // update generators, which all have their own color
    g_NoiseGen[0].Update(CurrentBit);
    g_NoiseGen[1].Update(CurrentBit);
    g_NoiseGen[2].Update(CurrentBit);
    g_NoiseGen[3].Update(CurrentBit);
    
    delayMicroseconds(GlobalColor);
    
    //Debug();
  }
}

