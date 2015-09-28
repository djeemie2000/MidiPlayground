#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "TimerOne.h"
#include "IntSimpleOscillator.h"
#include "IntOperatorFactory.h"
#include "MidiNoteFrequencies.h"

#define SERIAL_USED Serial1

const int SamplingFrequency = 40000;
const int IntScale = 12;
isl::CSimpleOscillator<IntScale> g_Oscillator;

void TestSpeed()
{
  Serial1.println("Test speed...");
  unsigned long Before = millis();
  const int NumRepeats = SamplingFrequency;
  for (int Value = 0; Value < NumRepeats; ++Value)
  {
    mcp48_setOutput(Value % 4096);
  }
  unsigned long After = millis();
  unsigned long Duration = After - Before;
  Serial1.print(NumRepeats);
  Serial1.print(" x = ");
  Serial1.print(Duration);
  Serial1.println(" mSec");

}

void TestSaw()
{
  for (int Value = 0; Value < 4096; Value += 4)
  {
    mcp48_setOutput(Value);
  }
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

void WriteDac()
{
  //static int DacValue = 0;
  //DacValue = (DacValue + 16)%4096;
  //mcp48_setOutput(DacValue);
  mcp48_setOutput(2048 + (7*g_Oscillator()>>3));
}

//void TestSawInterrupt()
//{
//  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
//  SERIAL_USED.print("Fs=");
//  SERIAL_USED.print(SamplingFrequency);
//  SERIAL_USED.print(" Period=");
//  SERIAL_USED.print(PeriodMicroSeconds);
//  SERIAL_USED.println(" uSec");
//
//  SERIAL_USED.println("Starting...");
//  Timer1.initialize(PeriodMicroSeconds);
//  Timer1.attachInterrupt(WriteDac);
//  //delay(1000);
//  while (true)
//  {
//    for (int idx = 0; idx < 400 * 1000; ++idx)
//    {
//      usbMIDI.read();
//      //delay(1);
//    }
//    //static int Sel = 0;
//    //Sel = (Sel+1)%8;
//    //g_Oscillator.SelectOperator(Sel);
//  }
//  Timer1.detachInterrupt();
//  delay(1000);
//}

void setup() {
  // put your setup code here, to run once:
  SERIAL_USED.begin(115200);
  delay(1000);
  SERIAL_USED.println("Teensy mcp4822 synth test...");

  analogReadResolution(12);

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

