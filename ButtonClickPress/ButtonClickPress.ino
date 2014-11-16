
const int RedLedPin = 12;
const int GreenLedPin = 11;
const int ButtonPin = 10;

const int RedLedPin2 = 9;
const int GreenLedPin2 = 8;
const int ButtonPin2 = 7;

bool g_RedLedActive = false;
bool g_GreenLedActive = false;
bool g_RedLed2Active = false;
bool g_GreenLed2Active = false;

class CButtonHandler
{
public:
  CButtonHandler()
  : m_HoldThreshold(500)
  , m_PrevButtonPressed(false)
  , m_Counter(0)
  , m_ButtonState(Neutral)
  {
  }

  void Update(bool ButtonPressed)
  {
    // set to 'default' state
    m_ButtonState = Neutral;

    if(m_PrevButtonPressed != ButtonPressed)
    {
      // change in state
      m_PrevButtonPressed = ButtonPressed;
      if(!m_PrevButtonPressed)
      { // pressed -> released
        if(m_Counter<m_HoldThreshold)
        {
          // 'short' press 
          m_ButtonState = Clicked;
        }
        m_Counter = 0;
      }        
    }
    else if(m_PrevButtonPressed)
    {
      // continuously pressed
      if(m_Counter<m_HoldThreshold)
      {
        // not (yet)a a'long' press
        ++m_Counter;
      }
      else if(m_Counter == m_HoldThreshold)
      {
        m_ButtonState = Hold;
        ++m_Counter;//prevent re-triggering hold again
      }
    }
  }

  bool ButtonClicked() const
  {
    return m_ButtonState == Clicked;
  }

  bool ButtonHold() const
  {
    return m_ButtonState == Hold;
  }
private:
  const int m_HoldThreshold;
  
  enum EButtonState
  {
    Neutral,
    Clicked,
    Hold
  };

  bool m_PrevButtonPressed;
  int m_Counter;
  EButtonState m_ButtonState;  
};

class CPeriodicTrigger
{
  public:
  CPeriodicTrigger()
  : m_Period(1)
  , m_Duration(1)
  , m_Counter(1)
  {
  }
  
  void Configure(int Period, int Duration)
  {
    m_Period = Period;
    m_Duration = Duration;
  }
  
  void Update()
  {
    if(1<m_Counter)
    {
      --m_Counter;
    }
    else
    {
      m_Counter = m_Period;
    }
  }
  
  bool TriggerOn() const
  {
    return (m_Counter == 1);
  }
  
  bool TriggerOff() const
  {
    int TriggerOff = m_Period - m_Duration;
    return (m_Counter == TriggerOff);
  }
  
  private:
    int m_Period;
    int m_Duration;
    int m_Counter;
};

CButtonHandler ButtonHandler1;
CButtonHandler ButtonHandler2;
CPeriodicTrigger StepTrigger;


void setup()
{
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  pinMode(ButtonPin, INPUT_PULLUP);

  pinMode(RedLedPin2, OUTPUT);
  pinMode(GreenLedPin2, OUTPUT);
  pinMode(ButtonPin2, INPUT_PULLUP);
  
  pinMode(13, OUTPUT);

  Serial.begin(9600); 
  
  StepTrigger.Configure(1000, 100);

  for(int Repeat = 0; Repeat<5; ++Repeat)
  {
    digitalWrite(RedLedPin, HIGH);
    digitalWrite(GreenLedPin, LOW);
    digitalWrite(RedLedPin2, HIGH);
    digitalWrite(GreenLedPin2, LOW);
    delay(100);

    digitalWrite(RedLedPin, LOW);
    digitalWrite(GreenLedPin, HIGH);
    digitalWrite(RedLedPin2, LOW);
    digitalWrite(GreenLedPin2, HIGH);
    delay(100);
  }

  digitalWrite(RedLedPin, g_RedLedActive);
  digitalWrite(GreenLedPin, g_GreenLedActive);
  digitalWrite(RedLedPin2, g_RedLed2Active);
  digitalWrite(GreenLedPin2, g_GreenLed2Active);
}

void loop()
{
  bool Button1Pressed = (digitalRead(ButtonPin)==LOW);
  bool Button2Pressed = (digitalRead(ButtonPin2)==LOW);

  ButtonHandler1.Update(Button1Pressed);
  ButtonHandler2.Update(Button2Pressed);
  StepTrigger.Update();

  if(ButtonHandler1.ButtonClicked())
  { // toggle red led
    g_RedLedActive = ! g_RedLedActive;
    analogWrite(RedLedPin, g_RedLedActive ? 128 : 0);
  }
  else if(ButtonHandler1.ButtonHold())
  {
    //toggle green led
    g_GreenLedActive = ! g_GreenLedActive;
    digitalWrite(GreenLedPin, g_GreenLedActive ? HIGH : LOW);  
  }

  if(ButtonHandler2.ButtonClicked())
  { // toggle red led
    g_RedLed2Active = ! g_RedLed2Active;
    analogWrite(RedLedPin2, g_RedLed2Active ? 128 : 0);
  }
  else if(ButtonHandler2.ButtonHold())
  {
    //toggle green led
    g_GreenLed2Active = ! g_GreenLed2Active;
    digitalWrite(GreenLedPin2, g_GreenLed2Active ? HIGH : LOW);  
  }
  
  if(StepTrigger.TriggerOn())
  {
    digitalWrite(13, HIGH);
  }
  if(StepTrigger.TriggerOff())
  {
    digitalWrite(13, LOW);
  }
  
  delay(1);//delay 1 millisecond
}

