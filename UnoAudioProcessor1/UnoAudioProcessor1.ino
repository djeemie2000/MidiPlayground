#include <SPI.h>
#include "UnoMcpDac.h"
#include "TimerOne.h"
#include "IntWaveFolder.h"

const int AudioInPin = A0;
const int FoldInPin = A1;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x05);
}

static const uint32_t g_SamplingFrequency = 1<<14;//2*8192

class CController
{
  public:
    CController()
    {
    }

    void Begin()
    {      
      pinMode(AudioInPin, INPUT);
      pinMode(FoldInPin, INPUT);
      setupFastAnalogRead();
      mcp48dac::Begin(1);
    }

    void Update()
    {
      int Fold = 32;//analogRead(FoldInPin)>>FoldRescale;// to ... bits fold
      m_WaveFolder.SetFold(Fold);

      Serial.print(Fold);
      Serial.print(" ");
      Serial.println(m_TickCount);
      delay(500);

    }

    void OnTick()
    {      
      // update phasors
      int InValue = analogRead(AudioInPin)-512;

      int ProcessedValue = InValue;
      //int ProcessedValue = m_WaveFolder(InValue);
      //int ProcessedValue = 0<InValue ? 512-InValue : -512-InValue;
      
      int OutValue = (ProcessedValue<<Rescale)+2048;
      //mcp48dac::SetOutput(OutValue, mcp48dac::Channel_A, mcp48dac::Gain_x2, 0);   
      mcp48dac::SetOutput(OutValue, mcp48dac::Channel_B, mcp48dac::Gain_x2, 0);    

      ++m_TickCount;
    }
   
  private:
    static const int DacScale = 12;
    static const int InScale = 10;
    static const int Rescale = DacScale-InScale;
    static const int FoldScale = 6;
    static const int FoldRescale = InScale-FoldScale;
     
    uint32_t m_TickCount;
    isl::CWaveFolder<int, DacScale-2, FoldScale> m_WaveFolder; 
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
  Serial.println("Uno Audio processor 1...");

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

