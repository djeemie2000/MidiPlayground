#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "MidiNoteFrequencies.h"
#include "IntKarplusStrong.h"
#include "TimerOne.h"

#define SERIAL_USED Serial1

const int SamplingFrequency = 40000;
const int IntScale = 12;

const int NumOscillators = 8;

isl::CKarplusStrong<int, IntScale, SamplingFrequency/30, NumOscillators> g_Oscillator;
int g_Damp;
int g_Excite;
int g_NoteOnCounter;

void WriteDac()
{
  int Val = CalcDacValue();
  //mcp48_setOutput(CalcDacValue());
  mcp48_setOutput(0, GAIN_1, 1, Val);
  mcp48_setOutput(1, GAIN_1, 1, Val);  
}

int CalcDacValue()
{
  //static int Val = 0;
  //Val = (Val+1)%4096;
  //return Val;

  int Val = g_Oscillator(); 
 
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
  }
  unsigned long After = millis();
  unsigned long Duration = After - Before;
  SERIAL_USED.print("Repeat x ");
  SERIAL_USED.print(SamplingFrequency);
  SERIAL_USED.print(" = ");
  SERIAL_USED.print(Duration);
  SERIAL_USED.println(" mSec");
}

void SpeedTestWriteDac()
{
  SERIAL_USED.println("SpeedTest write Dac...");
  unsigned long Before = millis();
  for(int idx = 0; idx<SamplingFrequency; ++idx)
  {
    WriteDac();
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
  int FreqMilliHz = GetMidiNoteFrequencyMilliHz(Note);
  //int Excitation = Velocity<<5; // 27 to 2^12
  g_Oscillator.Excite(g_Excite, FreqMilliHz, g_Damp);
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
  // note off is ignored
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
  { // damp 
    g_Damp = Value<<5;//2^7 to 2^12
    SERIAL_USED.print("Selected Damp ");
    SERIAL_USED.println(g_Damp);
  }
  else if(Number == 18)
  { // damp 
    g_Excite = Value<<5;//2^7 to 2^12
    SERIAL_USED.print("Selected Damp ");
    SERIAL_USED.println(g_Damp);
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

  SERIAL_USED.println("Teensy Midi Synth 2...");

  g_NoteOnCounter = 0;
  g_Damp = 3072;//0.75*4096
  g_Excite = 2048;//0.5*4096
  g_Oscillator.SetSamplingFrequency(SamplingFrequency);

  mcp48_begin();
  mcp48_setOutput(0);
  delay(100);

  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleControlChange(OnControlChange);
  usbMIDI.setHandlePitchChange(OnPitchBend);

  //debug/diagnostics/tests
  SpeedTest();
  SpeedTestWriteDac();
  //TestDacValue();
  //return;

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

