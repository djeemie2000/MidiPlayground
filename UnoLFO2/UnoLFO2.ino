#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "UintSkewedPhasor.h"

const int SpeedAInPin = A0;
const int SpeedBInPin = A1;
const int SkewAInPin = A2;
const int SkewBInPin = A3;

static const uint32_t g_SamplingFrequency = 1<<13;//8192

class CController
{
  public:
    CController()
     : m_PhasorA()
     , m_PhasorB()
    {
    }

    void Begin()
    {
      pinMode(SpeedAInPin, INPUT);
      pinMode(SpeedBInPin, INPUT);
      pinMode(SkewAInPin, INPUT);
      pinMode(SkewBInPin, INPUT);
      
      mcp48dac::Begin(1);
    }

    void Update()
    {
      // read pots for speed, skew
      int PhaseIncreaseA = 8 + 4*analogRead(SpeedAInPin);// TODO check scaling
      int SkewA = max(1, analogRead(SkewAInPin));
      m_PhasorA.SetPhaseIncrease(PhaseIncreaseA, SkewA);

      int PhaseIncreaseB = 8 + 4*analogRead(SpeedBInPin);// TODO check scaling
      int SkewB = max(1, analogRead(SkewBInPin));
      m_PhasorB.SetPhaseIncrease(PhaseIncreaseB, SkewB);      
    }

    void OnTick()
    {      
      // update phasors
      uint32_t LfoValueA = m_PhasorA.UpdateTriangle();
      uint32_t LfoValueB = m_PhasorB.UpdateTriangle();
      
      mcp48dac::SetOutput(LfoValueA, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);      
      mcp48dac::SetOutput(LfoValueB, mcp48dac::Channel_B, mcp48dac::Gain_x2, 0);    

      // TODO pulse out on digital outputs ~ skewed phase
    }
   
  private:
    static const int ValueScale = 12;
    static const int PhaseScale = 24;
     
    uisl::CSkewedPhasor<ValueScale, PhaseScale> m_PhasorA;
    uisl::CSkewedPhasor<ValueScale, PhaseScale> m_PhasorB;
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

void StartTimer(uint32_t SamplingFrequency)
{
  const unsigned long PeriodMicroSeconds = 1000000ul / SamplingFrequency;
  Serial.print("Fs=");
  Serial.print(SamplingFrequency);
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
  StartTimer(g_SamplingFrequency);
}

void loop() 
{
//  TestSpeed();
//  TestAccuracy_NewApi();
  g_Controller.Update();
}

