#include "DueTimer.h"
#include <SPI.h>

#include "MidiNoteFrequencies.h"
#include "MidiCC.h"

#include "OnePoleFilter.h"
#include "Noise.h"
#include "KarplusStrong.h"


// oscillator globals:
int OutPin = 53;

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

const int SamplingFrequency = 40000;

// osillator globals
float g_OscillatorFrequencyHz;
float g_ExiterLPFCutOff;
const int g_ExiterLPFNumPoles = 8;
CNoise<float> g_Exiter;
CMultiStageFilter<float, COnePoleLowPassFilter<float>, 8> g_ExiterLPF;
float g_KarplusStrongCutOff;
int g_KarplusStrongNumPoles;
float g_Trigger;
CKarplusStrong<float> g_KarplusStrong(SamplingFrequency, 10);//MinFreq


// midi globals
int CurrMidiNote;
int CurrAmplitude;
CMidiCC MidiCC;

// helper functions:
void myHandler()
{
  unsigned int DacValue = CalcDacValue();
  mcp48_setOutput(0, GAIN_1, 0x01, DacValue);
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency * 1000 / FrequencyMilliHz;
}

void mcp48_begin()
{
  SPI.begin(PIN_CS);//can be 4, 10 or ??
  SPI.setClockDivider(PIN_CS, 4);//DUE: default is 21 corresponding to 4MHz as in AVR
}

void mcp48_setOutput(unsigned int val)
{
  //assuming single channel, gain=2
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x10;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}

void mcp48_setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}

unsigned int BipolarToMcp(float Value)
{
  // conversion + clamping
  int DacValue = (1 + Value) * 2047; //[-1,1] -> [0,4096[
  DacValue = 0 < DacValue ? DacValue : 0;
  DacValue = DacValue < 4096 ? DacValue : 4096;
  return DacValue;
}

unsigned int CalcDacValue()
{
  //float Tmp = g_ExiterLPF(g_Exiter(), g_ExiterLPFCutOff);

//  float Tmp2 = 0.0f;
  float OscillatorValue = g_KarplusStrong(g_Trigger,
                                          g_OscillatorFrequencyHz,
                                          g_KarplusStrongCutOff,
                                          g_Exiter());//g_ExiterLPF(g_Exiter(), g_ExiterLPFCutOff));                                         
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


  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000 * 1000 / SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds);
}

void OnNoteOn()
{
  int OscillatorFrequencyMilliHz = GetMidiNoteFrequencyMilliHz(CurrMidiNote);
  g_OscillatorFrequencyHz = OscillatorFrequencyMilliHz * 0.001f;
  g_Trigger = 1.0f;
}

void OnNoteOff()
{

}

void ApplyOscillatorParameters()
{
  //TODO
  g_ExiterLPFCutOff = (1 + MidiCC.GetController(73)) / 128.0f;
  g_KarplusStrongCutOff = (1 + MidiCC.GetController(75)) / 128.0f;;
}

void loop()
{
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

