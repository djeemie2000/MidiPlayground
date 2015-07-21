void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

}

void loop() {
  // put your main code here, to run repeatedly:
  for(int idx = 0; idx<256; ++idx)
  {
    Serial.println(idx);
    delay(250);
  }
}
