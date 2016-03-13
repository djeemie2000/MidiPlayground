
int g_ChangeCounter;
bool g_State;

void OnChange()
{
  ++g_ChangeCounter;
  g_State = digitalRead(2)==HIGH;
  
  digitalWrite(13, g_State?HIGH:LOW);
}

void OnRise()
{
  ++g_ChangeCounter;
  g_State = true;
}

void OnFall()
{
  ++g_ChangeCounter;
  g_State = false;
}

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Uno interrupt in test...");
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);
  g_ChangeCounter = 0;
  g_State = false;

  attachInterrupt(digitalPinToInterrupt(2), OnChange, CHANGE);
  // only 1 interrupt can be attached to a pin
  //attachInterrupt(digitalPinToInterrupt(2), OnRise, RISING);
  //attachInterrupt(digitalPinToInterrupt(2), OnFall, FALLING);

}

void loop() 
{
  // put your main code here, to run repeatedly:
  int ChangeCounter = 0;
  while(true)
  {
    if(g_ChangeCounter != ChangeCounter)
    {
      ChangeCounter = g_ChangeCounter;

      //digitalWrite(13, g_State?HIGH:LOW);
      
      Serial.print(ChangeCounter);
      Serial.print(" ");
      if(g_State)
      {
        Serial.println("On");
      }
      else
      {
        Serial.println("Off");
      }
    }
      delay(1);
    
//    if(digitalRead(2)==HIGH)
//    {
//      Serial.println("StateOn");
//    }
//    else
//    {
//      Serial.println("StateOff");      
//    }
  }
}
