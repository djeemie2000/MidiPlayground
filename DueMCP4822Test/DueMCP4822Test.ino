#include <SPI.h>

const int PIN_CS = 10;
const int GAIN_1 = 0x1;
const int GAIN_2 = 0x0;

void setup()
{
  Serial.begin(115200);
  Serial.println("DUE MCP4822 test 2...");

  mcp48_begin();
}

void mcp48_begin()
{
  SPI.begin(PIN_CS);//can be 4, 10 or ??
  SPI.setClockDivider(PIN_CS, 4);//DUE: default is 21 corresponding to 4MHz as in AVR  
}

void setOutput(unsigned int val)
{
  //assuming single channel, gain=2
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | 0x10;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}

void setOutput(byte channel, byte gain, byte shutdown, unsigned int val)
{
  byte lowByte = val & 0xff;
  byte highByte = ((val >> 8) & 0xff) | channel << 7 | gain << 5 | shutdown << 4;

  SPI.transfer(PIN_CS, highByte, SPI_CONTINUE);
  SPI.transfer(PIN_CS, lowByte);
}

void TestAccuracy()
{
  //high-res saw wave
  for (unsigned int DacValue = 0; DacValue < 4096; DacValue += 32)
  {
    setOutput(0, GAIN_1, 1, DacValue);
    //setOutput(DacValue);
    //setOutput(1, 0, 0, DacValue);//???

    //delay(100);
    int ValueA = analogRead(A0);
    int ValueB = analogRead(A1);
    Serial.print("DAC out=");
    Serial.print(DacValue);
    Serial.print(" ADC in=");
    Serial.print(ValueA);
    Serial.print(" ");
    Serial.print(ValueB);
    Serial.println();

    delay(250);
  }
}

void TestSpeed()
{
  unsigned long Before = millis();

  //high-res saw wave
  for (int Repeat = 0; Repeat < 10; ++Repeat)
  {
    for (unsigned int DacValue = 0; DacValue < 4096; DacValue += 1)
    {
      setOutput(0, GAIN_1, 1, DacValue);
      //setOutput(DacValue);
      //setOutput(1, 0, 0, DacValue);//???
    }
  }

  unsigned long After = millis();
  unsigned long Duration = After - Before;
  unsigned int Number = 4096 * 10;

  Serial.print("DAC out x ");
  Serial.print(Number);
  Serial.print(" = ");
  Serial.print(Duration);
  Serial.print(" mSec");
  Serial.println();
}

void TestSaw()
{
  Serial.println("Saw wave out...");
  //high-res saw wave
  while (true)
  {
    for(int Increase = 2; Increase < 16; Increase += 2)
    {
      int NumRepeats = 30 * Increase;
      for(int Repeat = 0; Repeat < NumRepeats; Repeat++)
      {
        for (unsigned int DacValue = 0; DacValue < 4096; DacValue += Increase)
        {
          setOutput(0, GAIN_1, 1, DacValue);
          //setOutput(DacValue);
          //setOutput(1, 0, 0, DacValue);//???
        }
      }
    }
  }
}

void TestSquare()
{
  Serial.println("Saw wave out...");
  //high-res saw wave
  while (true)
  {
    for(int Increase = 2; Increase < 16; Increase += 2)
    {
      int NumRepeats = 30 * Increase;
      for(int Repeat = 0; Repeat < NumRepeats; Repeat++)
      {
        for (unsigned int DacValue = 0; DacValue < 4096; DacValue += Increase)
        {
          setOutput(0, GAIN_1, 1, DacValue<2048?0:4095);
          //setOutput(DacValue);
          //setOutput(1, 0, 0, DacValue);//???
        }
      }
    }
  }
}


void loop()
{
  //  TestAccuracy();
  //  TestSpeed();
  //  delay(500);
  TestSaw();
  //TestSquare();
}

