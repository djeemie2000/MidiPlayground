
const int NumPins = 8;

const int AnalogInPin1 = A0;

void setup()
{
  for(int idx = 0; idx<NumPins; ++idx)
  {
    pinMode(AnalogInPin1+idx, INPUT);
  }
  
  Serial.begin(9600);
}


void loop()
{
  int Values[NumPins];
  for(int idx = 0; idx<NumPins; ++idx)
  {
    Values[idx] = analogRead(AnalogInPin1+idx);
  }  

  for(int idx = 0; idx<NumPins; ++idx)
  {
    Serial.print(Values[idx]);
    Serial.print(" ");
  }
  Serial.println();
 
 delay(500); 
}
