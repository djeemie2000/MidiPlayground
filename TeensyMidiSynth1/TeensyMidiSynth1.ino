#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "MidiNoteFrequencies.h"
#include "IntSimpleOscillator.h"
#include "IntOperatorFactory.h"
#include "IntEnvelope.h"
#include "TimerOne.h"

#define SERIAL_USED Serial1

const int SamplingFrequency = 40000;
const int IntScale = 12;

const int NumOscillators = 16;

isl::CSimpleOscillator<IntScale> g_Oscillator[NumOscillators];
//int g_Envelope[NumOscillators];
int g_MidiNote[NumOscillators];
int g_ActivationCounter[NumOscillators];
isl::CIntegerAHREnvelope<int, 7> g_Envelope[NumOscillators];
int g_NoteOnCounter;

void WriteDac()
{
  mcp48_setOutput(CalcDacValue());
}

int CalcDacValue()
{
  //static int Val = 0;
  //Val = (Val+1)%4096;
  //return Val;

  int Val = 0;
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    Val +=  ( (g_Envelope[idx]() * g_Oscillator[idx]()) >> 7); 
  }
  Val /= NumOscillators;

  return 2048 + 7*Val/8;
}

void SpeedTest()
{
  SERIAL_USED.println("SpeedTest...");
  unsigned long Before = millis();
  int bs = 0;
  for(int idx = 0; idx<SamplingFrequency; ++idx)
  {
    bs += CalcDacValue();
    //WriteDac();
  }
  unsigned long After = millis();
  unsigned long Duration = After - Before;
  SERIAL_USED.print("Repeat x ");
  SERIAL_USED.print(SamplingFrequency);
  SERIAL_USED.print(" = ");
  SERIAL_USED.print(Duration);
  SERIAL_USED.println(" mSec");
}

void OnNoteOn(byte Channel, byte Note, byte Velocity)
{
  LogNoteOn(Channel, Note, Velocity);

  ++g_NoteOnCounter;
  bool Done = false;
  for(int idx = 0; !Done && idx<NumOscillators; ++idx)
  {
    if(g_ActivationCounter[idx]==-1)
    {
      g_Envelope[idx].NoteOn();// = 1;
      g_MidiNote[idx] = Note;
      g_ActivationCounter[idx] = g_NoteOnCounter;
      int FreqMilliHz = GetMidiNoteFrequencyMilliHz(g_MidiNote[idx]);
      g_Oscillator[idx].SetFrequency(FreqMilliHz);
      Done = true;
    }
  }
}

void LogNoteOn(byte Channel, byte Note, byte Velocity)
{
  SERIAL_USED.print("NoteOn : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" note ");
  SERIAL_USED.print(Note, DEC);
  SERIAL_USED.print(" vel ");
  SERIAL_USED.println(Velocity);
}

void OnNoteOff(byte Channel, byte Note, byte Velocity)
{
  LogNoteOff(Channel, Note, Velocity);
  // note off only for most recent note on
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    if(Note == g_MidiNote[idx])
    {
      g_Envelope[idx].NoteOff();// = 0;
      g_MidiNote[idx] = 0;
      g_ActivationCounter[idx] = -1;
    }
  }
}

void LogNoteOff(byte Channel, byte Note, byte Velocity)
{
  SERIAL_USED.print("NoteOff : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" note ");
  SERIAL_USED.print(Note, DEC);
  SERIAL_USED.print(" vel ");
  SERIAL_USED.println(Velocity);
}

void OnControlChange(byte Channel, byte Number, byte Value)
{
  LogControlChange(Channel, Number, Value);

  if(Number == 17)
  { // oscillator selection
    int SelectedOperator = Value*isl::COperatorFactory<IntScale>::NumOperators/128;
    for(int idx = 0; idx<NumOscillators; ++idx)
    {
      g_Oscillator[idx].SelectOperator(SelectedOperator);
    }
    SERIAL_USED.print("Selected operator ");
    SERIAL_USED.println(SelectedOperator);
  }
}

void LogControlChange(byte Channel, byte Number, byte Value)
{
  SERIAL_USED.print("CC : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" nr ");
  SERIAL_USED.print(Number, DEC);
  SERIAL_USED.print(" val ");
  SERIAL_USED.println(Value);
}

void OnPitchBend(byte Channel, int Bend)
{
  LogPitchBend(Channel, Bend);
}
  
void LogPitchBend(byte Channel, int Bend)
{
  SERIAL_USED.print("PitchBend : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" val ");
  SERIAL_USED.println(Bend);
}

void TestDacValue()
{
  SERIAL_USED.println("Test DacValue...");
  for(int idx = 0; idx<SamplingFrequency/10; ++idx)
  {
    SERIAL_USED.print(idx);
    SERIAL_USED.print(" : ");
    SERIAL_USED.println(CalcDacValue());
  }
  SERIAL_USED.println("Done");
}

void setup() 
{
  // put your setup code here, to run once:
  SERIAL_USED.begin(115200);
  delay(1000);

  SERIAL_USED.println("Teensy Midi Synth 1...");

  g_NoteOnCounter = 0;
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    g_Oscillator[idx].SetSamplingFrequency(SamplingFrequency);
    g_Oscillator[idx].SetFrequency(220 * 1000);
    g_Oscillator[idx].SelectOperator(6);
    g_Envelope[idx].SetAttack(SamplingFrequency, 100);
    g_Envelope[idx].SetRelease(SamplingFrequency, 900);
    //g_Envelope[idx] = 0;
    g_MidiNote[idx] = 0;
    g_ActivationCounter[idx] = -1;
  }

  mcp48_begin();
  mcp48_setOutput(0);
  delay(100);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchBend);

  //debug/diagnostics/tests
  SpeedTest();
  //TestDacValue();

  //
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  SERIAL_USED.print("Fs=");
  SERIAL_USED.print(SamplingFrequency);
  SERIAL_USED.print(" Period=");
  SERIAL_USED.print(PeriodMicroSeconds);
  SERIAL_USED.println(" uSec");
  
  SERIAL_USED.println("Starting...");
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(WriteDac);
}

void loop() 
{
  // put your main code here, to run repeatedly:
    usbMIDI.read();
}

