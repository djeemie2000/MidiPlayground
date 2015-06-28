
void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Starting...");

  analogWriteResolution(11);
}

void TestSquare()
{
  const int HalfPeriodmSec = 500;

  const int NumReads = 8;
  for(int Out = 0; Out<2048; Out+=64)
  {
    analogWrite(DAC1, Out);
    int Value = analogRead(A0);
    
    Serial.print("Test: Out=");
    Serial.print(Out);
    Serial.print(" Read=");
    Serial.print(Value);
    Serial.println();
    
    delay(HalfPeriodmSec);
  }
}


void loop() 
{
    TestSquare();
}
