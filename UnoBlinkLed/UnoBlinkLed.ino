

void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);

  for(int Repeat = 0; Repeat<10; ++Repeat)
  {
    digitalWrite(13, HIGH);
    delay(100*(1+Repeat));
    digitalWrite(13, LOW);
    delay(100*(1+Repeat));
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(500);
  digitalWrite(13, LOW);
  delay(500);
}

