#include "DueTimer.h"
#include <SPI.h>

#include "MidiNoteFrequencies.h"
#include "MidiCC.h"
#include "DueMcpDac.h"
#include "OnePoleFilter.h"
#include "Noise.h"
#include "KarplusStrong.h"
#include "IntOnePoleFilter.h"
#include "IntPhaseGenerator.h"

// oscillator globals:
int OutPin = 53;

const int SamplingFrequency = 40000;

// osillator globals
float g_OscillatorFrequencyHz;
float g_ExiterLPFCutOff;
const int g_ExiterLPFNumPoles = 2;
CNoise<float> g_Exiter;
CMultiStageFilter<float, COnePoleLowPassFilter<float>, g_ExiterLPFNumPoles> g_ExiterLPF;
COnePoleLowPassFilter<int> g_LPFFloat;

float g_KarplusStrongCutOff;
int g_KarplusStrongNumPoles;
float g_Trigger;
CKarplusStrong<float> g_KarplusStrong(SamplingFrequency, 10);//MinFreq

//integer stuff
CNoise<int> g_ExiterInt;
CIntegerOnePoleLowPassFilter<int, 8> g_LPFInt(0);
CIntegerPhaseGenerator<12> g_PhaseInt(0);

// debugging
unsigned long g_InteruptCounter;

// midi globals
int CurrMidiNote;
int CurrAmplitude;
CMidiCC MidiCC;

// helper functions:
void myHandler()
{
  unsigned int DacValue = CalcLPFNoiseInt();//CalcDacValue();

  DacValue = 0 < DacValue ? DacValue : 0;
  DacValue = DacValue < 4096 ? DacValue : 4096;
  
  mcp48_setOutput(0, GAIN_1, 0x01, DacValue);
  ++g_InteruptCounter;
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency * 1000 / FrequencyMilliHz;
}

unsigned int BipolarToMcp(float Value)
{
  // conversion + clamping
  int DacValue = (1 + Value) * 2047; //[-1,1] -> [0,4094[
  DacValue = 0 < DacValue ? DacValue : 0;
  DacValue = DacValue < 4096 ? DacValue : 4096;
  return DacValue;
}

unsigned int CalcLPFNoiseFloat()
{
  float OscillatorValue = g_LPFFloat(g_Exiter());
  return BipolarToMcp(OscillatorValue);
}

int CalcLPFNoiseInt()
{
  int Noise = (g_Exiter.Rand()>>20);// [0, u32 max] to [0, 4096] to [-2048, 2048]
  Noise -= 2048;

  int Saw = g_PhaseInt();
  
  int OscillatorValue = g_LPFInt((Noise+Saw)/2);

  OscillatorValue = 0<CurrAmplitude ? OscillatorValue : 0;
  
  return 2048+OscillatorValue;//TODO function?
}

void TestLPFNoiseInt()
{
  while(true)
  {
    int DacValue = CalcLPFNoiseInt();
    Serial.println(DacValue);    
    if(DacValue<0 || 4096<=DacValue)
    {
      Serial.println("!!!!!!!!!!");
      delay(500);
    }
    delay(100);
  }
}


void LogSpeedTest(int Repeats, unsigned long Before, unsigned long After)
{
  Serial.print("Repeat x ");
  Serial.print(Repeats);
  Serial.print("  = ");
  unsigned long Duration = After-Before;
  Serial.print(Duration);
  Serial.println(" mSec");  
}

void TestCalcSpeed()
{
  Serial.println("Testing CalcLPFNoise()...");
  
//  {
//    g_LPFFloat.SetParameter(0.65f);
//    
//    unsigned long Before = millis();
//    unsigned int DacValue;
//    for(int Repeat = 0; Repeat<SamplingFrequency; ++Repeat)
//    {
//        DacValue += CalcLPFNoiseFloat();
//    }
//
//    unsigned long After = millis();
//    LogSpeedTest(SamplingFrequency, Before, After);  
//  }

  {
    g_LPFInt.SetParameter(168);

    unsigned long Before = millis();
    unsigned int DacValue;
    for(int Repeat = 0; Repeat<SamplingFrequency; ++Repeat)
    {
        DacValue += CalcLPFNoiseInt();
    }

    unsigned long After = millis();
    LogSpeedTest(SamplingFrequency, Before, After);  
  }
}

unsigned int CalcDacValue()
{
  float OscillatorValue = g_KarplusStrong(g_Trigger,
                                          g_OscillatorFrequencyHz,
                                          g_KarplusStrongCutOff,
                                          g_ExiterLPF(g_Exiter(), g_ExiterLPFCutOff));                                         
  g_Trigger = 0.0f;

  return BipolarToMcp(OscillatorValue);
}

void setup()
{
  // use Serial as debug output
  Serial.begin(115200);
  Serial.println("MidiSynth...");

  // use serial1 as midi input
  // for now, use serial usb cable => baudrate is not 31250
  Serial1.begin(115200);

  analogReadResolution(10);

  const int DefaultMidiNote = 45;// note A1 = 110 Hz

  CurrMidiNote = DefaultMidiNote;
  CurrAmplitude = 0;

  g_OscillatorFrequencyHz = 220.0f;
  g_KarplusStrongCutOff = 0.8f;
  g_ExiterLPFCutOff = 0.65f;
  g_ExiterLPF.SetStages(2);
  g_ExiterLPF.SetParameter(g_ExiterLPFCutOff);
  mcp48_begin();

  MidiCC.SetController(73, g_KarplusStrongCutOff*128-1);
  MidiCC.SetController(75, g_ExiterLPFCutOff*128-1);

  g_LPFInt.SetParameter(MidiCC.GetController(73));
  int FreqMilliHz = GetMidiNoteFrequencyMilliHz(CurrMidiNote);
  g_PhaseInt.SetFrequency(SamplingFrequency, FreqMilliHz);
  Serial.print("Freq= ");
  Serial.print(FreqMilliHz);
  Serial.print(" Phase= ");
  Serial.println(g_PhaseInt.GetPhaseStep());

  g_InteruptCounter = 0;
  
  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000 * 1000 / SamplingFrequency;
  //Timer3.start(SamplingPeriodMicroSeconds);
}

void OnNoteOn()
{
  int OscillatorFrequencyMilliHz = GetMidiNoteFrequencyMilliHz(CurrMidiNote);
  g_OscillatorFrequencyHz = OscillatorFrequencyMilliHz * 0.001f;
  g_Trigger = 1.0f;
  g_PhaseInt.SetFrequency(SamplingFrequency, OscillatorFrequencyMilliHz);
  g_PhaseInt.Sync();
}

void OnNoteOff()
{

}

void ApplyOscillatorParameters()
{
  //TODO
  g_ExiterLPFCutOff = (1 + MidiCC.GetController(73)) / 128.0f;
  g_KarplusStrongCutOff = (1 + MidiCC.GetController(75)) / 128.0f;
  g_LPFFloat.SetParameter(g_ExiterLPFCutOff);
  g_LPFInt.SetParameter(2*(1+MidiCC.GetController(73)));

  Serial.print("LPF ");
  Serial.println(g_ExiterLPFCutOff);
}

void loop()
{
  while(true)
  {
    TestCalcSpeed();
  }
  //TestLPFNoiseInt();
  
  //Serial.print("L");
  if(SamplingFrequency*2<=g_InteruptCounter)
  {
    Serial.print(g_InteruptCounter);
    Serial.print(" t=");
    Serial.println(millis());
    g_InteruptCounter = 0;
  }
  // read midi in -> read something or read nothing
  // if note on/off, change curr amplitide, note
  // apply curr only when changed

  // read midi in
  // => simplest approach:
  // note on => curr note, ampl = 1, vel ignored
  // note off => ampl = 0, note and vel ignored
  // cc => store value if CC[]
  // other => ignored
  const int RawMidiInSize = 3;
  if (RawMidiInSize <= Serial1.available())
  {
    byte RawMidiInBuffer[RawMidiInSize];
    Serial1.readBytes(RawMidiInBuffer, RawMidiInSize);

    Serial.print("Recieved 0x");
    Serial.print(RawMidiInBuffer[0], HEX);
    Serial.print(" 0x");
    Serial.print(RawMidiInBuffer[1], HEX);
    Serial.print(" 0x");
    Serial.print(RawMidiInBuffer[2], HEX);
    Serial.println();

    bool Change = true;
    if (RawMidiInBuffer[0] == 0x80)
    { //note off
      if (0 < CurrAmplitude)
      {
        --CurrAmplitude;
      }
      OnNoteOff();
      // debug:
      Serial.print("Note Off ");
      Serial.print(RawMidiInBuffer[1]);//note
      Serial.print(" (");
      Serial.print(RawMidiInBuffer[2]);//vel
      Serial.println(")");
    }
    else if (RawMidiInBuffer[0] == 0x90)
    { // note on
      ++CurrAmplitude;
      CurrMidiNote = RawMidiInBuffer[1];
      // ignore velocity
      OnNoteOn();
      // debug:
      Serial.print("Note On ");
      Serial.print(RawMidiInBuffer[1]);//note
      Serial.print(" (");
      Serial.print(RawMidiInBuffer[2]);//vel
      Serial.println(")");

    }
    else if (RawMidiInBuffer[0] == 0xB0)
    {
      int CC = RawMidiInBuffer[1];
      int Value = RawMidiInBuffer[2];
      MidiCC.SetController(CC, Value);
      // debug:
      Serial.print("CC ");
      Serial.print(CC);//controller
      Serial.print(" (");
      Serial.print(Value);//value
      Serial.println(")");
      //
      ApplyOscillatorParameters();
    }
    else
    {
      Change = false;
    }
  }
}

