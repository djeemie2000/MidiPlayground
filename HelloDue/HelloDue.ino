int Cntr;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Cntr = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Test ");
  Serial.println(Cntr++);
  Serial.println();

  delay(1000);

}
