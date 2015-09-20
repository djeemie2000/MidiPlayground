
int DACValue = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  delay(1000);//??
  
  Serial.println("Teensy 123");

  pinMode(6, OUTPUT);
  pinMode(13, OUTPUT);

  analogReadResolution(12);
  for(int Pin = A0; Pin<=A9; ++Pin)
  {
    pinMode(Pin, INPUT);
  }

  analogWriteResolution(12);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  delay(500);
  Serial.println("HelloWorld");
  Serial1.println("Hello serial 1!");
  
  //blink led
  digitalWrite(13, HIGH);
  delay(200);
  digitalWrite(13, LOW);
  delay(200);

  Serial1.println("blink D out 6");
  //blink digital out
  digitalWrite(6, HIGH);
  delay(200);
  digitalWrite(6, LOW);
  delay(200);

  DACValue = (DACValue+32)%4096;
  analogWrite(A14, DACValue);
  Serial1.print("DAC out = ");
  Serial1.println(DACValue);

  for(int Pin = A0; Pin<=A9; ++Pin)
  {
    int Value = analogRead(Pin);
    Serial1.print("Analog  ");
    Serial1.print(Pin);
    Serial1.print(" is ");
    Serial1.println(Value);
  }
}
