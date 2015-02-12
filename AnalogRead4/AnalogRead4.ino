

const int AnalogInPin1 = A0;
const int AnalogInPin2 = A1;
const int AnalogInPin3 = A2;
const int AnalogInPin4 = A3;

void setup()
{
  pinMode(AnalogInPin1, INPUT);
  pinMode(AnalogInPin2, INPUT);
  pinMode(AnalogInPin3, INPUT);
  pinMode(AnalogInPin4, INPUT);
  
  Serial.begin(9600);
}


void loop()
{
 int Value1 = analogRead(AnalogInPin1);
 int Value2 = analogRead(AnalogInPin2);
 int Value3 = analogRead(AnalogInPin3);
 int Value4 = analogRead(AnalogInPin4);
 
 Serial.print(Value1);
 Serial.print(" ");
 Serial.print(Value2);
 Serial.print(" ");
 Serial.print(Value3);
 Serial.print(" ");
 Serial.print(Value4);
 Serial.println();
 
 delay(500); 
}
