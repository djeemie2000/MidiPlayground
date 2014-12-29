/*

Performs timings on some arduino functions.

Results on Uno:

AnalogRead x 10000 = 1120 msec => 1.1 mSec

DigitalRead x 10000 = 50 msec => 5 uSec

DigitalWrite x 10000 = 53 msec => 5 uSec

*/

const int DigitalReadPin = 5;
const int DigitalWritePin = 6;
const int AnalogReadPin = A0;

void setup()
{
  pinMode(DigitalReadPin, INPUT_PULLUP);
  pinMode(DigitalWritePin, OUTPUT);
  pinMode(AnalogReadPin, OUTPUT);
  
  Serial.begin(9600);
}

void TimingDigitalRead()
{
  const int NumRepeats = 10000;
  unsigned long Before = millis();
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     bool Temp = (digitalRead(DigitalReadPin)==LOW); 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  
  Serial.print("DigitalRead x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}

void TimingDigitalWrite()
{
  const int NumRepeats = 10000;
  unsigned long Before = millis();
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     digitalWrite(DigitalReadPin, LOW); 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  
  Serial.print("DigitalWrite x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}

void TimingAnalogRead()
{
  const int NumRepeats = 10000;
  unsigned long Before = millis();
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     int Temp = analogRead(AnalogReadPin); 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  
  Serial.print("AnalogRead x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}

void loop()
{
  TimingDigitalRead();
  TimingDigitalWrite();
  TimingAnalogRead();
}

