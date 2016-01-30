
static const int NumPwmLeds = 1;
static const int PwmLedPin[NumPwmLeds] = { 10 };

class CPwmLed
{
  public:
    CPwmLed()
     : m_Pin(-1)
     , m_Brightness(128)//???
     , m_Mode(OffMode)
    {}

    void Begin(int Pin)
    {
      m_Pin = Pin;
      pinMode(m_Pin, OUTPUT);
    }

    void SetBrightness(int Brightness)
    {
      m_Brightness = Brightness;
    }

    void On()
    {
      m_Mode = OnMode;
    }

    void Off()
    {
      m_Mode = OffMode;     
    }

    void BlinkNormal()
    {
      m_Mode = BlinkNormalMode;
    }

    void BlinkFast()
    {
      m_Mode = BlinkFastMode;
    }

    void BlinkSlow()
    {
      m_Mode = BlinkSlowMode;
    }

    void Update(unsigned long Millis)
    {
      int Brightness = 0;
//      if(m_Mode == OffMode)
//      {
//        Brightness =  0;
//      }
      if(m_Mode == OnMode)
      {
        Brightness = m_Brightness;
      }
      else if(m_Mode == BlinkNormalMode)
      {
        // use last 8 bits of millis => period of 256 mSec ~ 4 Hz
        Brightness = Millis & 0xFF;
      }
      else if(m_Mode == BlinkFastMode)
      {
        Brightness = (Millis<<1) & 0xFF;
      }
      else if(m_Mode == BlinkSlowMode)
      {
        Brightness = (Millis>1) & 0xFF;
      }

      analogWrite(m_Pin, m_Brightness);
    }

    
  private:
    static const int OffMode = 0;
    static const int OnMode = 1;
    static const int BlinkNormalMode = 2;
    static const int BlinkFastMode = 3;
    static const int BlinkSlowMode = 4;
  
     int m_Pin;
     int m_Brightness;
     int m_Mode;
};

CPwmLed g_Led[NumPwmLeds];

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("TeensyPwmLeds...");

  for(int Led = 0; Led<NumPwmLeds; ++Led)
  {
    g_Led[Led].Begin(PwmLedPin[Led]);
    g_Led[Led].SetBrightness(128);
  }
}

void RunLeds(int Led, int PeriodMilliSeconds)
{
  for(int Cntr = 0; Cntr<PeriodMilliSeconds; ++Cntr)
  {
    g_Led[Led].Update(millis());
    delay(1);
  }
}

void loop() 
{
  // on/off 
  for(int Led = 0; Led<NumPwmLeds; ++ Led)
  {
    Serial.println(Led);
    Serial.println("On");
    g_Led[Led].On();
    RunLeds(Led, 500);
    Serial.println("Off");
    g_Led[Led].Off();
    RunLeds(Led, 500);
  }
  
  // on for different brightnesses
  for(int Brightness = 32; Brightness<256; Brightness += 32)
  {
      for(int Led = 0; Led<NumPwmLeds; ++ Led)
      {
        Serial.println(Led);
        Serial.print("Brightness");
        Serial.println(Brightness);
        g_Led[Led].SetBrightness(Brightness);
        RunLeds(Led, 250);
      }
  }
  
  // blink
for(int Led = 0; Led<NumPwmLeds; ++ Led)
  {
    Serial.println(Led);
    Serial.println("BlinkNormal");
    g_Led[Led].BlinkNormal();
    RunLeds(Led, 2000);
    
    Serial.println("BlinkFast");
    g_Led[Led].BlinkFast();
    RunLeds(Led, 2000);

    Serial.println("BlinkSlow");
    g_Led[Led].BlinkSlow();
    RunLeds(Led, 2000);
  }

}
