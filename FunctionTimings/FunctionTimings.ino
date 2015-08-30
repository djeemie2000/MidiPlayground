/*

Performs timings on some arduino functions.

Results on Uno:

AnalogRead x 10000 = 1120 msec => 0.11 mSec

AnalogWrite x 10000 = 120 msec => 0.012 mSec

DigitalRead x 10000 = 50 msec => 5 uSec

DigitalWrite x 10000 = 53 msec => 5 uSec

Serial.print(int) x 100 = 12 mSec @115200 Baud => 0.12 mSec
Serial.print(int) x 100 = 163 mSec @9600 Baud => 1.63 mSec

*/

const int DigitalReadPin = 5;
const int DigitalWritePin = 6;
const int AnalogReadPin = A0;
const int AnalogWritePin = A1;

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
  pinMode(DigitalReadPin, INPUT_PULLUP);
  pinMode(DigitalWritePin, OUTPUT);
  pinMode(AnalogReadPin, INPUT);
  pinMode(AnalogWritePin, OUTPUT);
 
  //setupFastAnalogRead();
  
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

void TimingAnalogWrite()
{
  const int NumRepeats = 10000;
  unsigned long Before = millis();
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     analogWrite(AnalogWritePin, Repeat%1024); 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;
  
  Serial.print("AnalogWrite x ");
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

void TimingFloatOperations()
{
  const int NumRepeats = 100000;
  unsigned long Before = millis();
  float Tmp = 1.0f;
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     Tmp = (Tmp + 1)*1.5f; 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;

  Serial.print(Tmp);
  
  Serial.print("Float mult/add x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}

void TimingFloatConversions()
{
  const int NumRepeats = 100000;
  unsigned long Before = millis();
  float Tmp = 3.789f;
  int Tmp2 = 0;
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
     Tmp2 += Tmp*2048;
     Tmp2 += 2048; 
  }
  unsigned long After = millis();
  unsigned long Duration = After-Before;

  Serial.print(Tmp2);
  
  Serial.print("Float conversion x ");
  Serial.print(NumRepeats);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.println(" msec");
}

void loop()
{
  //TimingSerial();
  //TimingDigitalRead();
  //TimingDigitalWrite();
  //TimingAnalogRead();
  //TimingAnalogWrite();
  TimingFloatOperations();
  TimingFloatConversions();
}

