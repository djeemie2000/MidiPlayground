void setup() {
  // put your setup code here, to run once:
  const int OutBaudRate = 115200;
  Serial.begin(115200);
  Serial.println("Listening...");
  const int InBaudRate = 31250;//19200;//9600;
  Serial3.begin(InBaudRate);
}

void loop() {
  // put your main code here, to run repeatedly:
  int Cntr = 0;
  while(true)
  {
    int Available = 0<Serial3.available();
    if(0<Available)
    {
      int Value = Serial3.read();
      Serial.print(Cntr++);
      Serial.print(":");
      Serial.print(Value, HEX);
      Serial.print(" (");
      Serial.print(Available, DEC);
      Serial.println(")");  
    }  
  }
}

