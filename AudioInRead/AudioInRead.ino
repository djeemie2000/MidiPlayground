
const int AnalogInPin1 = A0;

void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x04);
}


void setup()
{
  pinMode(AnalogInPin1, INPUT);
  
  Serial.begin(115200);

  setupFastAnalogRead();
}


void loop()
{
  const int Scale = 990;
  const int Offset = 512;
  int Value = analogRead(AnalogInPin1);  

  Serial.print(Scale);
  Serial.print(" ");
  Serial.print(Offset);
  Serial.print(" ");
  Serial.println(Value);
}
