
#include <Wire.h>

int cntr = 0;

void setup()
{
  // use first 8 gpio's as input with pullup enabled
  Serial.begin(115200);

  Wire.begin();
  // set I/O pins to outputs
  Wire.beginTransmission(0x20);
  Wire.write(0x00); // IODIRA register
  Wire.write(0x00); // set all of port A to outputs
  Wire.endTransmission();
  Wire.beginTransmission(0x20);
  Wire.write(0x01); // IODIRB register
  Wire.write(0x00); // set all of port B to outputs
  Wire.endTransmission();
}

void loop()
{
  unsigned long Before = millis();
  uint16_t ReadValues = 0;
  for(int Repeat = 0; Repeat<1000; ++Repeat)
  {
     Wire.beginTransmission(0x20);
    Wire.write(0x12); // GPIOA
    Wire.write(Repeat%256); // port A
    Wire.endTransmission();
    delay(10);
  }
  unsigned long After = millis();
  Serial.print(cntr++);
  Serial.print("  ");
  Serial.print(After-Before-1000*10);
  Serial.print("  ");
  Serial.println(ReadValues, BIN);
  delay(500);
}


