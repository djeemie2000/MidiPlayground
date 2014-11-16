
const int RedLedPin = 12;
const int GreenLedPin = 11;
const int ButtonPin = 10;

bool g_ButtonPressed = false;
int g_ButtonCounter = 0;
bool g_RedLedActive = false;
bool g_GreenLedActive = false;

void setup()
{
  pinMode(RedLedPin, OUTPUT);
  pinMode(GreenLedPin, OUTPUT);
  pinMode(ButtonPin, INPUT_PULLUP);
 
  Serial.begin(9600); 
  
  for(int Repeat = 0; Repeat<5; ++Repeat)
  {
    digitalWrite(RedLedPin, HIGH);
    digitalWrite(GreenLedPin, LOW);
    delay(100);
  
    digitalWrite(RedLedPin, LOW);
    digitalWrite(GreenLedPin, HIGH);
    delay(100);
  }
  
  digitalWrite(RedLedPin, g_RedLedActive);
  digitalWrite(GreenLedPin, g_GreenLedActive);
}

void loop()
{
  bool ButtonPressed = (digitalRead(ButtonPin)==LOW);
  if(ButtonPressed != g_ButtonPressed)
  {
    // 
    g_ButtonPressed = ButtonPressed;
    g_ButtonCounter = 0;
    
    if(g_ButtonPressed)
    { // off -> on
      // => toggle red led
      g_RedLedActive = ! g_RedLedActive;
      digitalWrite(RedLedPin, g_RedLedActive ? 128 : LOW);
    }
  }
  else if(g_ButtonPressed)
  {
    if(g_ButtonCounter<1000)
    {
      ++g_ButtonCounter;
    }
    else if(g_ButtonCounter == 1000)
    { // long press
      // => toggle green led
      g_GreenLedActive = ! g_GreenLedActive;
      digitalWrite(GreenLedPin, g_GreenLedActive ? HIGH : LOW);  
      ++g_ButtonCounter;//prevent toggling again
    }
  }
  delay(1);//delay 1 millisecond
}
