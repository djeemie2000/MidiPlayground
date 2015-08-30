const int Pin = 7;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Pin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int Value = digitalRead(Pin);
  unsigned char WriteChar = 0<Value ? 0xFF : 0x00;
  Serial.write(WriteChar);
  delay(250);
}
