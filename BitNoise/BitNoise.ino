#include "BitNoise.h"

const int AnalogInPin = A0;
const int NoiseOutPin = 3;
const int GatePin = 0;//wrt PORTB

const int NumAnalogInPins = 8;
int g_AnalogInValue[NumAnalogInPins];

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

// arduino nano: pins D2-D7 are PD2-PD7
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

// arduino nano: pins D8-D12 are PB0-PB4 
int DigitalInPortB(int Pin)
{
  // TODO support other ports
  return (PINB >> Pin) & 1;
}



CBitNoise g_BitNoise;

struct SNoiseGen
{  
  int s_Color;
  int s_Cntr;
  int s_NoisePin; 
  int s_CurrentBit;
  int s_GatePin;
  int s_Gate;

  void Begin(int NoisePin, int GatePin)
  {
    s_Color = 0;
    s_Cntr = 0;
    s_NoisePin = NoisePin;//-1;
    s_CurrentBit = 0;
    s_GatePin = GatePin;

    pinMode(s_NoisePin, OUTPUT);
    pinMode(s_GatePin+8, INPUT_PULLUP);//by default, gate is on 
    // +8 : PORTB offset is 8
  }
  
  void Update(int CurrentBit)
  {
    ++s_Cntr;
    if(s_Color<s_Cntr)
    {
      s_CurrentBit = CurrentBit;
      s_Cntr = 0;
    }

    // fast digital pin input read
    // Gate => current bit, no gate => low
    s_Gate = DigitalInPortB(s_GatePin);
    DigitalOutPortD(s_NoisePin, s_Gate ? s_CurrentBit : 0);
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

  // global color in
  for(int Pin = 0; Pin<NumAnalogInPins; ++Pin)
  {
    pinMode(AnalogInPin+Pin, INPUT);
    g_AnalogInValue[Pin] = 0;
  }
  
  // generators
  for(int Gen = 0; Gen<NumGenerators; ++Gen)
  {
    g_NoiseGen[Gen].Begin(NoiseOutPin+Gen, GatePin+Gen);
    g_NoiseGen[Gen].SetColor(0);
  }

//  TimingBitNoise();
//  TimingUpdate();
//  TimingDigitalWrite();
//  TimingAnalogRead();
}

void Debug(int GlobalColor)
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

    Serial.println("AnalogIn ");
    for(int Pin = 0; Pin<NumAnalogInPins; ++Pin)
    {
      Serial.println(g_AnalogInValue[Pin]);
    }

    Serial.print("Pitch ");
    Serial.println(GlobalColor);
    
    Serial.println("Grain ");
    for(int Gen = 0; Gen<NumGenerators; ++Gen)
    {
      Serial.println(g_NoiseGen[Gen].s_Color);
    }

    Serial.println("Gate ");
    for(int Gen = 0; Gen<NumGenerators; ++Gen)
    {
      Serial.println(g_NoiseGen[Gen].s_Gate);
    }
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
    Dummy += analogRead(AnalogInPin);
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

int CombineCV(int idx1, int idx2)
{
  int Combined = g_AnalogInValue[idx1]+g_AnalogInValue[idx2];
  return Combined<1024 ? Combined : 1023;
}

void loop() 
{
  int AnalogRead = 0;
  int GlobalColor = 0;
  while(true)
  {

    
    // do one analog read at a time:
    g_AnalogInValue[AnalogRead] = analogRead(AnalogInPin+AnalogRead);
    
    // global color, local color for each generator
    // high Vin => high pitch, low Vin => low pitch
    GlobalColor = 1023-CombineCV(0,5);

    // 'graininess' : low voltage => low gaininess = low color, high voltage => high graininess = high color
    // color/divider/octave: [0,16[
    g_NoiseGen[0].SetColor(CombineCV(1,6)>>6);
    g_NoiseGen[1].SetColor(CombineCV(2,6)>>6);
    g_NoiseGen[2].SetColor(CombineCV(3,7)>>6);
    g_NoiseGen[3].SetColor(CombineCV(4,7)>>6);
        
    ++AnalogRead;
    if(NumAnalogInPins<=AnalogRead)
    {
      AnalogRead = 0;
    }
    
    // generate next bit of noise
    g_BitNoise.Generate();
    int CurrentBit = g_BitNoise.Get();
    
    // update generators, which all have their own color
    g_NoiseGen[0].Update(CurrentBit);
    g_NoiseGen[1].Update(CurrentBit);
    g_NoiseGen[2].Update(CurrentBit);
    g_NoiseGen[3].Update(CurrentBit);
    
    delayMicroseconds(GlobalColor);
    
    //Debug(GlobalColor);
  }
}

