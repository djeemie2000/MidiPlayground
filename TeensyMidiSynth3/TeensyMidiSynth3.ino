#include <SPI.h>
#include "spi4teensy3.h"
#include "TeensyMcpDac.h"
#include "MidiNoteFrequencies.h"
#include "IntKarplusStrong.h"
#include "IntFeedbackDelay.h"
#include "TimerOne.h"

#define SERIAL_USED Serial1

const int LedPin = 3;
int g_LedCounter;

//Note: reducing the sampling frequency to save RAM is certainly an option!
const int SamplingFrequency = 30000;
const int IntScale = 16;
const int DacScale = 12;

const int NumOscillators = 8;
const int MinFrequencyHz = 35;

isl::CKarplusStrong<int, IntScale, SamplingFrequency/MinFrequencyHz, NumOscillators> g_Oscillator;
int g_Damp;
int g_Excite;
int g_AttackMilliSeconds;
int g_NoteOnCounter;

const int NumCombFilters = 2;
const int DelayCapacity = SamplingFrequency/40;// min freq is 50 Hz
isl::CFeedbackDelay<int, DelayCapacity> g_CombFilterLeft[NumCombFilters];
isl::CFeedbackDelay<int, DelayCapacity> g_CombFilterRight[NumCombFilters];

int g_ValLeft;
int g_ValRight;

void WriteDac()
{
  CalcDacValue();
  mcp48_setOutput(0, GAIN_1, 1, g_ValLeft);
  mcp48_setOutput(1, GAIN_1, 1, g_ValRight);  
}

void ClampAndScale(int& Val)
{
  Val = 2048 + 7*Val/16;

  if(Val<0)
  {
    Val = 0;
  }
  else if(4095<Val)
  {
    Val = 4095;
  }
}

void CalcDacValue()
{
  //static int Val = 0;
  //Val = (Val+1)%4096;
  //return Val;

  int Val = g_Oscillator(); 

  g_ValLeft = 0;
  g_ValRight = 0;

  for(int idx = 0; idx<NumCombFilters; ++idx)
  {
    g_ValLeft += g_CombFilterLeft[idx](Val);
    g_ValRight += g_CombFilterRight[idx](Val);
  }

  g_ValLeft = (g_ValLeft>>(IntScale-DacScale))/(1+NumCombFilters);
  g_ValRight = (g_ValRight>>(IntScale-DacScale))/(1+NumCombFilters);
  
  ClampAndScale(g_ValLeft);
  ClampAndScale(g_ValRight);
}

void SpeedTest()
{
  SERIAL_USED.println("SpeedTest...");
  unsigned long Before = millis();
  int bs = 0;
  for(int idx = 0; idx<SamplingFrequency; ++idx)
  {
    //bs += 
    CalcDacValue();
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
  g_Oscillator.Excite(g_Excite, FreqMilliHz, g_Damp, g_AttackMilliSeconds);

  g_LedCounter = 100;
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
    SERIAL_USED.print("Selected Excite ");
    SERIAL_USED.println(g_Excite);
  }
  else if(Number == 19)
  { // attack 
    g_AttackMilliSeconds = 1+Value<<3;//2^7 to 2^10 (1024)
    SERIAL_USED.print("Selected Attack ");
    SERIAL_USED.println(g_AttackMilliSeconds);
  }
  else if(Number == 21)
  {
    for(int idx = 0; idx<NumCombFilters; ++idx)
    {
      g_CombFilterLeft[idx].SetWetDry(Value, 7);
      g_CombFilterRight[idx].SetWetDry(Value, 7);
    }
    SERIAL_USED.print("Selected Wet/Dry Left/Right ");
    SERIAL_USED.println(Value);    
  }  
  else if(Number == 22)
  {
    int Delay = Value*DelayCapacity>>7;
    for(int idx = 0; idx<NumCombFilters; ++idx)
    {
      g_CombFilterLeft[idx].SetDelay(Delay*(1+idx));//(11+idx)/11);//TODO
    }
    SERIAL_USED.print("Selected Delay Left ");
    SERIAL_USED.println(Delay);
  }
  else if(Number == 23)
  {
    int Delay = Value*DelayCapacity>>7;
    for(int idx = 0; idx<NumCombFilters; ++idx)
    {
      g_CombFilterRight[idx].SetDelay(Delay*(1+idx));//(11+idx)/11);//TODO
    }
    SERIAL_USED.print("Selected Delay Right ");
    SERIAL_USED.println(Delay);    
  }
  else if(Number == 24)
  {
    for(int idx = 0; idx<NumCombFilters; ++idx)
    {
      g_CombFilterLeft[idx].SetFeedback(Value, 7);
      g_CombFilterRight[idx].SetFeedback(Value, 7);
    }
    SERIAL_USED.print("Selected Feedback Left/Right ");
    SERIAL_USED.println(Value);    
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
    CalcDacValue();
    SERIAL_USED.print(idx);
    SERIAL_USED.print(" : ");
    SERIAL_USED.print(g_ValLeft);
    SERIAL_USED.print(",");
    SERIAL_USED.print(g_ValRight);    
  }
  SERIAL_USED.println("Done");
}

void setup() 
{
  // put your setup code here, to run once:
  SERIAL_USED.begin(115200);
  delay(1000);

  pinMode(LedPin, OUTPUT);
  g_LedCounter = 0;
  digitalWrite(LedPin, HIGH);

  SERIAL_USED.println("Teensy Midi Synth 2...");

  g_NoteOnCounter = 0;
  g_Damp = 3072;//0.75*4096
  g_Excite = 2048;//0.5*4096
  g_AttackMilliSeconds = 0;
  g_Oscillator.SetSamplingFrequency(SamplingFrequency);

  for(int idx = 0; idx<NumCombFilters; ++idx)
  {
    g_CombFilterLeft[idx].SetDelay(SamplingFrequency*11/1000);
    g_CombFilterRight[idx].SetDelay(SamplingFrequency*13/1000);
    g_CombFilterLeft[idx].SetFeedback(125, 7);
    g_CombFilterRight[idx].SetFeedback(125, 7);
    g_CombFilterLeft[idx].SetWetDry(64, 7);
    g_CombFilterRight[idx].SetWetDry(64, 7);
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

  --g_LedCounter;
  digitalWrite(LedPin, 0<g_LedCounter ? HIGH : LOW);
    
  delay(1);
}

