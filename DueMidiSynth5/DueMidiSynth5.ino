#include "DueTimer.h"
#include <SPI.h>

#include "MidiNoteFrequencies.h"
#include "MidiCC.h"
#include "DueMcpDac.h"
#include "IntOnePoleFilter.h"
#include "IntPhaseGenerator.h"
#include "IntNoise.h"
#include "IntOperators.h"
#include "IntEnvelope.h"
#include "WaveTableData.h"
#include "IntInterpolator.h"

// oscillator globals:
const int SamplingFrequency = 40000;//40000;

static const int IntegerResolution = 12;

const int LPFNumPoles = 8;
CIntegerMultiStageFilter<int, CIntegerOnePoleLowPassFilter<int, 8>, LPFNumPoles> g_LPFIntMulti;
CIntegerFeedbackOperator<int, 7> g_FeedbackOperator;//scale [0, 128] cfr midi
CIntegerPhaseGenerator<uint32_t, 16> g_PhaseGenerator;// interpolated phase scale = 2^16
CIntegerInterpolator<7, 16> g_Interpolation;// 128 sized wave table => 7 bit scale, use 16 bits index scale

static const int EnvelopeScale = 8;
CIntegerAHREnvelope<int, EnvelopeScale> g_AmplitudeEnvelope;

const int AttackReleaseTimes[] = { 1, 2, 3, 4, 5, 6, 7, 8, 
                                      9, 10, 11, 12, 13, 14, 15, 16,
                                      18, 20, 22, 24, 26, 28, 30, 32,
                                      36, 40, 44, 48, 52, 56, 60, 64,  
                                      72, 80, 88, 96, 104, 112, 120, 128,
                                      144, 160, 176, 192, 208, 224, 240, 256, 
                                      288, 320, 352, 394, 416, 448, 480, 512,
                                      576, 640, 704, 792, 832, 896, 960, 1024,
                                      1152, 1280, 1408, 1584, 1664, 1792, 1920, 2048,
                                      2304, 2560, 2816, 3168, 3328, 3584, 3840, 4096,
                                      4608 };

// debugging
unsigned long g_InteruptCounter;
unsigned long g_OverflowCounter;

// midi globals
const int LpfMidiCC = 1;//mod wheel!
const int LpfFeedbackMidiCC = 20;
const int AttackMidiCC = 23;
const int ReleaseMidiCC = 24;
const int WaveTableMidiCC = 17;
CMidiCC MidiCC;
int g_CurrAmplitude;
int g_WaveTableIndex;


// helper functions:
void myHandler()
{
  uint32_t DacValue = CalcDacValue();  //  limit on signed value, not here!
  mcp48_setOutput(0, GAIN_1, 0x01, DacValue);
  ++g_InteruptCounter;
}

uint32_t CalcDacValue()
{
  uint32_t PhaseScaled = g_PhaseGenerator();//saw up
  int WaveTableValue = g_Interpolation.GetInterpolatedValue(wt2::wav_res_waves + wt2::WaveTableOffset*g_WaveTableIndex, PhaseScaled) <<0;//first wt of first bank, [0,255] to [0,2040]

  int OscillatorValue = IntUnipolarToBipolar<IntegerResolution>(WaveTableValue);
  
  OscillatorValue = g_LPFIntMulti(OscillatorValue);

  OscillatorValue = (0<g_CurrAmplitude) ? OscillatorValue : 0; 
  // envelope -> signed!!
  //OscillatorValue = (OscillatorValue * g_AmplitudeEnvelope() )>> EnvelopeScale;
  
  // clamp here!!! signed!!!!
  if(2047<OscillatorValue)
  {
    OscillatorValue = 2048;
    ++g_OverflowCounter;
  }
  else if(OscillatorValue<-2048)
  {
    OscillatorValue = -2048;
    ++g_OverflowCounter;
  }
  
  return IntBipolarToUnsigned<IntegerResolution>(OscillatorValue);
}

void TestDacValue()
{
  while(true)
  {
    int DacValue = CalcDacValue();
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
  Serial.println("Testing speed...");

  {
    g_LPFIntMulti.SetParameter(168);
    g_LPFIntMulti.SetStages(LPFNumPoles);

    unsigned long Before = millis();
    unsigned int DacValue;
    for(int Repeat = 0; Repeat<SamplingFrequency; ++Repeat)
    {
        DacValue += CalcDacValue();//CalcLPFNoiseInt();
    }

    unsigned long After = millis();
    LogSpeedTest(SamplingFrequency, Before, After);
  }

  {
    unsigned long Before = millis();
    unsigned int DacValue = 1<<(IntegerResolution-1);
    for(int Repeat = 0; Repeat<SamplingFrequency; ++Repeat)
    {
      mcp48_setOutput(2048, GAIN_1, 0x01, DacValue);
    }

    unsigned long After = millis();
    LogSpeedTest(SamplingFrequency, Before, After);
  }

}

void setup()
{
  // use Serial as debug output
  Serial.begin(115200);
  Serial.println("MidiSynth with wavetables...");

  Serial.print(wt2::NumWaveTables);
  Serial.print(" wave tables in ");
  Serial.print(wt2::NumWaveTableBanks);
  Serial.print(" banks");
  Serial.println();

  Serial.print("Sampling frequency is ");
  Serial.println(SamplingFrequency);

  // use serial1 as midi input
  // for now, use serial usb cable => baudrate is not 31250
  Serial1.begin(115200);

  analogReadResolution(10);

  const int DefaultMidiNote = 45;// note A1 = 110 Hz

  g_CurrAmplitude = 0;

  mcp48_begin();

  MidiCC.SetController(LpfMidiCC, 64);
  g_LPFIntMulti.SetParameter(MidiCC.GetController(LpfMidiCC)*2);
  g_LPFIntMulti.SetStages(LPFNumPoles);

  g_WaveTableIndex = 16;
  MidiCC.SetController(WaveTableMidiCC, g_WaveTableIndex%wt2::WaveTableBankSize);
  MidiCC.SetController(WaveTableMidiCC+1, g_WaveTableIndex/wt2::WaveTableBankSize);

  int FreqMilliHz = GetMidiNoteFrequencyMilliHz(DefaultMidiNote);
  g_PhaseGenerator.SetFrequency(SamplingFrequency, FreqMilliHz);

  g_AmplitudeEnvelope.SetAttack(SamplingFrequency, AttackReleaseTimes[1]);
  g_AmplitudeEnvelope.SetRelease(SamplingFrequency, AttackReleaseTimes[1]);

  // always show current processing speed
  TestCalcSpeed();
  //TestEnvelope();

  Serial.println("Starting...");

  g_InteruptCounter = 0;
  g_OverflowCounter = 0;
  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000 * 1000 / SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds);
}

void OnNoteOn(int MidiNote, int Velocity)
{
  ++g_CurrAmplitude;
  int FreqMilliHz = GetMidiNoteFrequencyMilliHz(MidiNote);
  g_PhaseGenerator.SetFrequency(SamplingFrequency, FreqMilliHz);
  // sync for first note, continue for following notes (avoids clicks in audio)
  if(g_CurrAmplitude==1)
  {
    g_PhaseGenerator.Sync();
  }
  g_AmplitudeEnvelope.NoteOn();
}

void OnNoteOff()
{
  if (0 < g_CurrAmplitude)
  {
    --g_CurrAmplitude;
  }
  if(0==g_CurrAmplitude)
  {
      g_AmplitudeEnvelope.NoteOff();
  }
}

void OnController(int Controller, int Value)
{
  int LPFCutOff = (1 + MidiCC.GetController(LpfMidiCC))*2;
  g_LPFIntMulti.SetParameter(LPFCutOff);

  g_FeedbackOperator.SetFeedback(MidiCC.GetController(LpfFeedbackMidiCC)*2);

  g_AmplitudeEnvelope.SetAttack(SamplingFrequency, AttackReleaseTimes[MidiCC.GetController(AttackMidiCC)*160/128]);
  g_AmplitudeEnvelope.SetRelease(SamplingFrequency, AttackReleaseTimes[MidiCC.GetController(ReleaseMidiCC)*160/128]);

  int WaveTableBank = MidiCC.GetController(WaveTableMidiCC+1)/16;//[0,127] to [0,15]
  int WaveTable = MidiCC.GetController(WaveTableMidiCC)*wt2::WaveTableBankSize/128;//[0, 127] to [0, 7]
  g_WaveTableIndex = WaveTableBank*wt2::WaveTableBankSize+WaveTable;// [ 0, 127] to ??

  // debug:
  Serial.print("Bank ");
  Serial.print(WaveTableBank);
  Serial.print(" table ");
  Serial.print(WaveTable);
  Serial.print(" -> ");
  Serial.print(g_WaveTableIndex);
  Serial.println();
}

void loop()
{
  if(SamplingFrequency*2<=g_InteruptCounter)
  {
    Serial.print(g_InteruptCounter);
    Serial.print(" O ");
    Serial.print(g_OverflowCounter); 
    Serial.print(" t=");
    Serial.println(millis());
    g_InteruptCounter = 0;
    g_OverflowCounter = 0;
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

    if ((RawMidiInBuffer[0]&0xF0) == 0x80)
    { //note off
      OnNoteOff();
      // debug:
      Serial.print("Note Off ");
      Serial.print(RawMidiInBuffer[1]);//note
      Serial.print(" (");
      Serial.print(RawMidiInBuffer[2]);//vel
      Serial.println(")");
    }
    else if ((RawMidiInBuffer[0]&0xF0) == 0x90)
    { // note on
      int MidiNote = RawMidiInBuffer[1];
      int Velocity = RawMidiInBuffer[2];
      OnNoteOn(MidiNote, Velocity);
      // debug:
      Serial.print("Note On ");
      Serial.print(RawMidiInBuffer[1]);//note
      Serial.print(" (");
      Serial.print(RawMidiInBuffer[2]);//vel
      Serial.println(")");

    }
    else if ((RawMidiInBuffer[0]&0xF0) == 0xB0)
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
      OnController(CC, Value);
    }
  }
}
