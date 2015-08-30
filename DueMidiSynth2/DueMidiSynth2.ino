#include "DueTimer.h"
#include <SPI.h>

#include "FrequencyMultipliers.h"
#include "MidiNoteFrequencies.h"
#include "MidiCC.h"

#include "PhaseAccumulator.h"
#include "PhaseStep.h"
#include "FullPseudoSin.h"



// oscillator globals:
int OutPin = 53;

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

const int SamplingFrequency = 40000;

// LFO globals:
CPhaseStep<float> LFOPhaseStep(SamplingFrequency);
CPhaseAccumulator<float> LFOPhaseAccumulator;
CFullPseudoSin<float> LFOOperator;

CPhaseStep<float> g_OscillatorPhaseStep(SamplingFrequency);
CPhaseAccumulator<float> g_OscillatorPhaseAccumulator;
CFullPseudoSin<float> g_OscillatorOperator;


// midi globals
int CurrMidiNote;
int CurrAmplitude;
CMidiCC MidiCC;
const int LFOFrequencyCC = 79;
const float DefaultLFOFrequency = 0.5f;

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

unsigned int CalcDacValue()
{
  // TODO envelope
  float OscillatorValue = g_OscillatorOperator(g_OscillatorPhaseAccumulator(g_OscillatorPhaseStep()));
  int Amplitude = 0<CurrAmplitude ? 1 : 0;
  OscillatorValue *= Amplitude;
  
  // conversion + clamping
  int DacValue = (1+OscillatorValue)*2047;//[-1,1] -> [0,4096[
  DacValue = 0<DacValue ? DacValue : 0;
  DacValue = DacValue<4096 ? DacValue : 4096;

   return DacValue;
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

  LFOPhaseStep.SetFrequency(DefaultLFOFrequency);
  MidiCC.SetController(LFOFrequencyCC, 64);

  g_OscillatorPhaseStep.SetFrequency(440.0f);

  mcp48_begin();

  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000 * 1000 / SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds);
}

void ApplyOscillatorParameters()
{
  //TODO logaritmic scaling
  float LFOFrequency = MidiCC.GetController(LFOFrequencyCC)*DefaultLFOFrequency/64;
  LFOPhaseStep.SetFrequency(LFOFrequency);
  
  int OscillatorFrequencyMilliHz = GetMidiNoteFrequencyMilliHz(CurrMidiNote);

  float OscillatorFrequencyHz = OscillatorFrequencyMilliHz * 0.001f;
  g_OscillatorPhaseStep.SetFrequency(OscillatorFrequencyHz);
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
      if(0<CurrAmplitude)
      {
        --CurrAmplitude;
      }
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
    }
    else
    {
      Change = false;
    }

    if (Change)
    {
      ApplyOscillatorParameters();
    }

  }
}

