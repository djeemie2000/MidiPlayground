#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"

const int AnalogInPin = A0;
const int AnalogInPin2 = A1;
const int DigitalOutPin0 = A2;
const int SlopeSelectInPin = 5;
const int SlopeSignInPin = 6;

static const int g_SamplingFrequency = 1<<13;//8192

class CController
{
  public:
    CController()
     : m_Phase(0)
//     , m_ScaledValue(0)
     , m_SelectedSlope(0)
     , m_SlopeSign(1)
    {
      m_PhaseIncrease[0] = 8;
      m_PhaseIncrease[1] = 8;  
    }

    void Begin()
    {
      pinMode(AnalogInPin, INPUT);
      pinMode(AnalogInPin2, INPUT);
      pinMode(DigitalOutPin0, OUTPUT);
      pinMode(DigitalOutPin0+1, OUTPUT);
      pinMode(DigitalOutPin0+2, OUTPUT);
      pinMode(DigitalOutPin0+3, OUTPUT);
      pinMode(SlopeSelectInPin, INPUT_PULLUP);
      pinMode(SlopeSignInPin, INPUT_PULLUP);     
      mcp48dac::Begin(1);
    }

    void Update()
    {
      // read pots for speed
      m_PhaseIncrease[0] = 8 + analogRead(AnalogInPin);
      m_PhaseIncrease[1] = 8 + analogRead(AnalogInPin2);
    }

    void OnTick()
    {      
      // update phase
      m_Phase += m_PhaseIncrease[m_SelectedSlope];

      // digital output updates: period, period/2, period*2
      uint32_t PeriodPin = (m_Phase >> (PhaseScale+1)) & 0x01;
      digitalWrite(DigitalOutPin0, !PeriodPin ? HIGH : LOW);

      
      uint32_t Periodx2Pin = (m_Phase >> (PhaseScale+2)) & 0x01;
      digitalWrite(DigitalOutPin0+1, Periodx2Pin ? HIGH : LOW);
      
      uint32_t PeriodDiv2Pin = (m_Phase >> (PhaseScale)) & 0x01;
      digitalWrite(DigitalOutPin0+2, !PeriodDiv2Pin ? HIGH : LOW);
      digitalWrite(DigitalOutPin0+3, PeriodDiv2Pin ? HIGH : LOW);

      // read digital inputs
      m_SelectedSlope = ( digitalRead(SlopeSelectInPin)==HIGH && digitalRead(SlopeSignInPin)==HIGH ) ? 0 : 1;
      //m_SlopeSign = digitalRead(SlopeSignInPin)==HIGH ? 1 : -1;
      
      // LFO (mcp) update
      unsigned int Value = ( m_Phase >> (PhaseScale-ValueScale) ) & ValueMask;
      if(PeriodDiv2Pin)// Value & (1<<(PhaseScale-1)))
      {
        Value = (1<<ValueScale)-1-Value;//~Value;
      }
      
      mcp48dac::SetOutput(Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);      
    }
   
  private:
    static const int ValueScale = 12;
    static const unsigned int ValueMask = (1<<ValueScale)-1;
    static const int PhaseScale = ValueScale + 8;
     
    static const int NumSlopes = 2;
    uint32_t m_Phase;
    int m_PhaseIncrease[NumSlopes];
    int m_SelectedSlope;
    int m_SlopeSign;
};

CController g_Controller;

void TestSpeed()
{
  Serial.println("Test speed...");
  unsigned long Before = millis();
  const long NumRepeats = g_SamplingFrequency;
  for(long Value = 0; Value<NumRepeats; ++Value)
  {
   //mcp48dac::SetOutput(Value%4096, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);
   g_Controller.OnTick();
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  Serial.print(NumRepeats);
  Serial.print(" x = ");
  Serial.print(Duration);
  Serial.println(" mSec");
  
}

void OnTick()
{
  g_Controller.OnTick();
}

void StartTimer()
{
  const unsigned long PeriodMicroSeconds = 1000000ul / g_SamplingFrequency;
  Serial.print("Fs=");
  Serial.print(g_SamplingFrequency);
  Serial.print(" Period=");
  Serial.print(PeriodMicroSeconds);
  Serial.println(" uSec");
  
  Serial.println("Starting..");
  Timer1.initialize(PeriodMicroSeconds);
  Timer1.attachInterrupt(OnTick);
}

void TestAccuracy_NewApi()
{
  Serial.println("Test accuracy new API...");
  for(int Value = 64; Value<4096; Value += 128)
  {
   mcp48dac::SetOutput(Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);
   mcp48dac::SetOutput(Value, mcp48dac::Channel_B, mcp48dac::Gain_x2, 0);   

   //mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_A, mcp48dac::Gain_x2, 1);
   //mcp48dac::SetOutput(4096-Value, mcp48dac::Channel_B, mcp48dac::Gain_x2, 1);   

   Serial.print("Out= ");
    Serial.println(Value);
    delay(500);
  }
}

void setup() 
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Uno LFO 1...");

  g_Controller.Begin();

  TestSpeed();
  
  delay(1000);
  //Serial.println("Starting");
  StartTimer();
}

void loop() 
{
//  TestSpeed();
//  TestAccuracy_NewApi();
  g_Controller.Update();
}

