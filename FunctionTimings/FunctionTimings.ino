/*

Performs timings on some arduino functions.

Results on Uno:

AnalogRead x 10000 = 1120 msec => 0.11 mSec

DigitalRead x 10000 = 50 msec => 5 uSec

DigitalWrite x 10000 = 53 msec => 5 uSec

Serial.print(int) x 100 = 12 mSec @115200 Baud => 0.12 mSec
Serial.print(int) x 100 = 163 mSec @9600 Baud => 1.63 mSec

*/

const int DigitalReadPin = 5;
const int DigitalWritePin = 6;
const int AnalogReadPin = A0;

void setup()
{
  pinMode(DigitalReadPin, INPUT_PULLUP);
  pinMode(DigitalWritePin, OUTPUT);
  pinMode(AnalogReadPin, OUTPUT);
  
  Serial.begin(115200);
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

void TimingSerial()
{
  const int NumRepeats = 100;
  unsigned long Before = millis();
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     Serial.print(Repeat); 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  
  Serial.println();
  Serial.print("Serial print 1 int x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}


void loop()
{
  TimingSerial();
  TimingDigitalRead();
  TimingDigitalWrite();
  TimingAnalogRead();
}

