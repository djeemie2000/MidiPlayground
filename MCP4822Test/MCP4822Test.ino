// MCPDAC relies on SPI.
#include <SPI.h>
#include "MCPDAC.h"

const int ChipSelectPin = 10;
const int ADCInPin = A0;

void setup()
{
  Serial.begin(115200);
  Serial.println("MCP4822 test");
  
  // CS on pin 10
  // no LDAC pin (tie it to ground)
  MCPDAC.begin(ChipSelectPin);
  
  // Set the gain to "HIGH" mode - 0 to 4096mV.
  MCPDAC.setGain(CHANNEL_A,GAIN_HIGH);
  // Set the gain to "LOW" mode - 0 to ?
  MCPDAC.setGain(CHANNEL_B,GAIN_LOW);
    
  // Do not shut down channel A, but shut down channel B.
  MCPDAC.shutdown(CHANNEL_A,false);
  MCPDAC.shutdown(CHANNEL_B,false);

  pinMode(ADCInPin, INPUT);//needed???
  pinMode(ADCInPin+1, INPUT);//needed???

  Serial.println("Setup done");
}


void TestAccuracy()
{
  Serial.println("Test accuracy...");
  
  // write some values to the DAC, then read the value from the ADC
  const int MaxDACValue = 1<<12;// 12 bits resolution
  for(unsigned int DacValue = 0; DacValue<MaxDACValue; DacValue += 128)
  {
    MCPDAC.setVoltage(CHANNEL_A, DacValue);
    MCPDAC.setVoltage(CHANNEL_B, DacValue);
    //MCPDAC.update();
    
    //delay(1);//needed?
    int ADCValue = analogRead(ADCInPin);
    int ADCValueB = analogRead(ADCInPin+1);

    Serial.print("DAC out=");
    Serial.print(DacValue);
    Serial.print(" ADC in=");
    Serial.print(ADCValue);
    Serial.print(" ");
    Serial.print(ADCValueB);
    Serial.println();
    
    delay(250);
  }
}

void TestSpeed()
{
  Serial.println("Test speed...");
  
  // write some values to the DAC repeatedly
  // timing of the duration
  unsigned long Before = millis();
  
  const unsigned int MaxDACValue = 1<<12;// 12 bits resolution
  const int NumRepeats = 10;
  for(int Repeat = 0; Repeat<NumRepeats; ++Repeat)
  {
    for(int DacValue = 0; DacValue<MaxDACValue; ++DacValue)
    {
      MCPDAC.setVoltage(CHANNEL_A, DacValue);
    }
  }

  unsigned long After = millis();
  unsigned long Duration = After-Before;
  unsigned int Number = NumRepeats * MaxDACValue;

  Serial.print("DAC out x ");
  Serial.print(Number);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void loop()
{
  //TestAccuracy();

  TestSpeed();

  delay(2000);  
}
