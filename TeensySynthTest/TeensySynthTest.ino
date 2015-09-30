#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "MidiNoteFrequencies.h"
#include "IntSimpleOscillator.h"
#include "IntOperatorFactory.h"
#include "TimerOne.h"

#define SERIAL_USED Serial1

const int SamplingFrequency = 40000;
const int IntScale = 12;

isl::CSimpleOscillator<IntScale> g_Oscillator;
int g_Envelope;
int g_MidiNote;

void WriteDac()
{
  mcp48_setOutput(CalcDacValue());
}

int CalcDacValue()
{
  //static int Val = 0;
  //Val = (Val+1)%4096;
  //return Val;

  return 2048 + (7*g_Oscillator()>>3);
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

  int FreqMilliHz = GetMidiNoteFrequencyMilliHz(Note);
  g_Oscillator.SetFrequency(FreqMilliHz);
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
    g_Oscillator.SelectOperator(SelectedOperator);

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

void TestSaw()
{
  for (int Value = 0; Value < 4096; Value += 4)
  {
    mcp48_setOutput(Value);
  }
}

void setup() 
{
  // put your setup code here, to run once:
  SERIAL_USED.begin(115200);
  delay(1000);

  SERIAL_USED.println("Teensy mcp4822 synth test...");

  g_Oscillator.SetSamplingFrequency(SamplingFrequency);
  g_Oscillator.SetFrequency(220 * 1000);
  g_Oscillator.SelectOperator(6);


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

void loop() {
  // put your main code here, to run repeatedly:
  //TestSpeed();
  //TestAccuracy();
  //delay(2000);
  //TestSaw();
  //TestSawInterrupt();
  usbMIDI.read();
}

