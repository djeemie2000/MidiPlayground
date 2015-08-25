#include "DueTimer.h"
#include "Oscillator.h"
#include "FrequencyMultipliers.h"
#include "MidiNoteFrequencies.h"
#include "MidiCC.h"

#include "PhaseAccumulator.h"
#include "PhaseStep.h"
#include "FullPseudoSin.h"

// oscillator globals:
int OutPin = 53;

const int SamplingFrequency = 40000;

const int NumOscillators = 4;
COscillator Oscillator[NumOscillators];

// LFO globals:
CPhaseStep<float> LFOPhaseStep(SamplingFrequency);
CPhaseAccumulator<float> LFOPhaseAccumulator;
CFullPseudoSin<float> LFOOperator;


// midi globals
int CurrMidiNote;
int CurrAmplitude;
CMidiCC MidiCC;
const int DetuneCC = 72;

// helper functions:
void myHandler()
{
  // update LFO
  float LFOValue = LFOOperator(LFOPhaseAccumulator(LFOPhaseStep()));
  int PulseWidthValue = 128+64*LFOValue;
  // update oscillators
  for (int idx = 0; idx < NumOscillators; ++idx)
  {
    Oscillator[idx].SetPulseWidth(PulseWidthValue);
    Oscillator[idx].Update();
  }
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency * 1000 / FrequencyMilliHz;
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

  const int DefaultOscillatorFrequencyMilliHz = GetMidiNoteFrequencyMilliHz(DefaultMidiNote);
  for (int idx = 0; idx < NumOscillators; ++idx)
  {
    int OscOutPin = OutPin - 2 * idx;
    Oscillator[idx].Begin(OscOutPin);
    Oscillator[idx].SetPeriod(CalcPeriod(DefaultOscillatorFrequencyMilliHz));
    Oscillator[idx].SetAmplitude(0);
    
    MidiCC.SetController(DetuneCC+idx, 64);//default to mid range
  }

  LFOPhaseStep.SetFrequency(0.5f);//

  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000 * 1000 / SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds);
}

void ApplyOscillatorParameters()
{
  int OscillatorFrequencyMilliHz = GetMidiNoteFrequencyMilliHz(CurrMidiNote);

  // debug info
  //  if (Repeat == 0)
  //  {
  //    Serial.print("global: ");
  //    Serial.print(" -> ");
  //    Serial.print(OscillatorFrequencyMilliHz);
  //    Serial.println();
  //  }

  for (int idx = 0; idx < NumOscillators; ++idx)
  {
    //
    int Amplitude = 0<CurrAmplitude ? 1 : 0;
    Oscillator[idx].SetAmplitude(Amplitude);
    //
    // scale midi cc [0,127], detune value[0, 1023]
    int DetuneValue = MidiCC.GetController(DetuneCC+idx)*8;
    int DetuneMult = FrequencyMultipliers_4x256_signed[DetuneValue];
    int DetunedOscillatorFrequencyMilliHz = OscillatorFrequencyMilliHz * DetuneMult / FrequencyMultiplierScale;
    //
    Oscillator[idx].SetPeriod(CalcPeriod(DetunedOscillatorFrequencyMilliHz));

    // debug info
    //    if (Repeat == 0)
    //    {
    //      Serial.print("detune ");
    //      Serial.print(idx);
    //      Serial.print(": ");
    //      Serial.print(DetuneValue);
    //      Serial.print(" -> ");
    //      Serial.print(DetuneMult);
    //      Serial.print(" -> ");
    //      Serial.print(DetunedOscillatorFrequencyMilliHz);
    //      Serial.println();
    //    }
  }
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

