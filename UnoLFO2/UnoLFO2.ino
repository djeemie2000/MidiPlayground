#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "UintSkewedPhasor.h"

const int SpeedAInPin = A0;
const int SkewAInPin = A1;
const int SpeedBInPin = A2;
const int SkewBInPin = A3;

static const uint32_t g_SamplingFrequency = 1<<14;//2*8192

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
      const int MinPhaseIncrease = 8;
      const int PhaseIncrMultiplier = 2;
      int PhaseIncreaseA = MinPhaseIncrease + PhaseIncrMultiplier*analogRead(SpeedAInPin);// TODO check scaling
      int SkewA = min(1008, max(16, analogRead(SkewAInPin)));
      m_PhasorA.SetPhaseIncrease(PhaseIncreaseA, SkewA);

      int PhaseIncreaseB = MinPhaseIncrease + PhaseIncrMultiplier*analogRead(SpeedBInPin);// TODO check scaling
      int SkewB = min(1008, max(16, analogRead(SkewBInPin)));
      m_PhasorB.SetPhaseIncrease(PhaseIncreaseB, SkewB); 

      return;
      Serial.print("Incr ");
      Serial.print(PhaseIncreaseA);
      Serial.print(" Skew ");
      Serial.print(SkewA);
      Serial.print(" Incr ");
      Serial.print(PhaseIncreaseB);
      Serial.print(" Skew ");
      Serial.print(SkewB);
      Serial.print(" ");
      Serial.println(m_TickCount); 

      delay(500);      
    }

    void DebugOnTick()
    {          
      // update phasors
      uint32_t PhaseA = m_PhasorA();
      uint32_t LfoValueA = uisl::CalcTriangle<uint32_t, ValueScale>(PhaseA);
      //uint32_t LfoValueB = m_PhasorB();//.UpdateTriangle();

      ++m_TickCount;

      //return;
      if(m_TickCount%32==0)
      {
        //Serial.print(m_TickCount);
        //Serial.print(" : ");
        //Serial.print(PhaseA & 0xFFF);
        //Serial.print(" ");
        Serial.print(LfoValueA);
        Serial.println();        
      }
    }

    void OnTick()
    {      
      // update phasors
      uint32_t LfoValueA = uisl::CalcTriangle<uint32_t, ValueScale>(m_PhasorA());
      uint32_t LfoValueB = uisl::CalcTriangle<uint32_t, ValueScale>(m_PhasorB());
      
      mcp48dac::SetOutput(LfoValueA, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);      
      mcp48dac::SetOutput(LfoValueB, mcp48dac::Channel_B, mcp48dac::Gain_x2, 0);    

      // TODO pulse out on digital outputs ~ skewed phase

      ++m_TickCount;
    }
   
  private:
    static const int ValueScale = 12;
    static const int PhaseScale = 20;//24;
     
    uisl::CSkewedPhasor<ValueScale, PhaseScale> m_PhasorA;
    uisl::CSkewedPhasor<ValueScale, PhaseScale> m_PhasorB;

    uint32_t m_TickCount;
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
  StartTimer(g_SamplingFrequency);
}

void loop() 
{
  g_Controller.Update();
//  for(uint32_t Repeat = 0; Repeat<g_SamplingFrequency/64; ++Repeat)
//  {
//    g_Controller.DebugOnTick();
//    delay(1);
//  }
}

