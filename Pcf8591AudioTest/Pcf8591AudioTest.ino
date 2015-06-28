#define TWI_FREQ 400000L //????
#include "Wire.h"

const int Pcf8591Address = 0x48;


void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  Serial.begin(9600);
  Serial.println("Starting...");

  Wire.beginTransmission(Pcf8591Address);
  uint8_t Error = Wire.endTransmission(Pcf8591Address);
  if(Error==0)
  {
    Serial.println("Found pcf8591");
  }
  else
  {
    Serial.println("Could not find pcf8591");
  }
}

void TestSquare()
{
  // 100 Hz square/pulse wave => halfperiod = 5 mSec
  const int HalfPeriodmSec = 2;

  for(int Cntr = 0; Cntr<100; ++Cntr)
  {
    Wire.beginTransmission(Pcf8591Address);
    Wire.write(0x40);//AOut
    Wire.write(255);
    Wire.endTransmission();
    delay(HalfPeriodmSec);
    
    Wire.beginTransmission(Pcf8591Address);
    Wire.write(0x40);//AOut
    Wire.write(0);
    Wire.endTransmission();
    delay(HalfPeriodmSec);
  }

  Serial.println("TestSquare");
}

void TestTriangle()
{
  int before = millis();
  for(int Cntr = 0; Cntr<32; ++Cntr)
  {
  for (int i=0; i<256; i+=8)
   {
     Wire.beginTransmission(Pcf8591Address); // wake up PCF8591
     Wire.write(0x40); // control byte - turn on DAC (binary 1000000)
     Wire.write(i); // value to send to DAC
     Wire.endTransmission(); // end tranmission
     //delayMicroseconds(1);
   }
  
//   for (int i=255; i>=0; --i)
//   {
//     Wire.beginTransmission(Pcf8591Address); // wake up PCF8591
//     Wire.write(0x40); // control byte - turn on DAC (binary 1000000)
//     Wire.write(i); // value to send to DAC
//     Wire.endTransmission(); // end tranmission
//     delayMicroseconds(10);
//   }
  }

  int after = millis();
  Serial.print(after - before);
  Serial.println(" TestTriangle");
}

void loop() 
{
  for(int idx = 0; idx<5; ++idx)
  {
    TestSquare();
  }
  for(int idx = 0; idx<5; ++idx)
  {
    TestTriangle();//????
  }
}
