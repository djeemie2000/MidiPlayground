
const int ClockInPin = 2;
const int ResetInPin = 3;

struct SController
{
  SController()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_Pattern = 0x1315;
    m_ActiveMask = 0xFFFF;
    m_CurrentStep = 0;
  }
  
  void Begin()
  {
    m_ChangeCounter = 0;
    m_State = false;
    m_CurrentStep = 0;
  }

  void OnClockRise()
  {
    ++m_ChangeCounter;
    Advance();
    //m_State = true;
  }

  void OnClockFall()
  {
    ++m_ChangeCounter;
    m_State = false;
  }

  void OnReset()
  {
    ++m_ChangeCounter;    
    m_CurrentStep = 0;
    m_State = (m_Pattern & (1<<m_CurrentStep));
  }

  void Advance()
  {
    ++m_CurrentStep;
    if(!(m_ActiveMask&(1<<m_CurrentStep)))
    {
      m_CurrentStep = 0;
    }

    m_State = (m_Pattern & (1<<m_CurrentStep));
  }

  int m_ChangeCounter;
  bool m_State;
  
  uint32_t m_Pattern;
  uint32_t m_ActiveMask;
  int m_CurrentStep;
};

SController g_Controller;

void OnClockChange()
{
  if(digitalRead(ClockInPin)==HIGH)
  {
    g_Controller.OnClockRise();
  }
  else
  {
    g_Controller.OnClockFall();
  }
  
  digitalWrite(13, g_Controller.m_State?HIGH:LOW);
}

void OnResetRise()
{
  g_Controller.OnReset();
  digitalWrite(13, g_Controller.m_State?HIGH:LOW);
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Uno interrupt in test...");
  pinMode(ClockInPin, INPUT);
  pinMode(13, OUTPUT);
  g_Controller.Begin();

  // only 1 interrupt can be attached to a pin
  attachInterrupt(digitalPinToInterrupt(ClockInPin), OnClockChange, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ResetInPin), OnResetRise, RISING);
}

void loop() 
{
  // put your main code here, to run repeatedly:
  static int ChangeCounter = 0;
  //while(true)
  //{
    if(g_Controller.m_ChangeCounter != ChangeCounter)
    {
      ChangeCounter = g_Controller.m_ChangeCounter;
      
      Serial.print(ChangeCounter);
      Serial.print(" : Step ");
      Serial.print(g_Controller.m_CurrentStep);
      Serial.print(" ");
      if(g_Controller.m_State)
      {
        Serial.println("On");
      }
      else
      {
        Serial.println("Off");
      }
    }
      delay(1);
  //}
}
