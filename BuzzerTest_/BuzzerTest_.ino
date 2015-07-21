
const int BuzzerPin = 7;

void setup()
{
  //pinMode(BuzzerPin, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  Serial.println("Sequence");
  const unsigned int BaseFrequency = 55;
  unsigned int Frequency = BaseFrequency;
  for(int Repeat = 0; Repeat<10; ++Repeat)
  {
    tone(BuzzerPin, Frequency);
    delay(100);
    Frequency = 1.42f*Frequency;
  }
}

