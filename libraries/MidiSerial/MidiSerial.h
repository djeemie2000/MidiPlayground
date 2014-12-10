#ifndef MIDISERIAL_H
#define MIDISERIAL_H

#include <Arduino.h>

class CMidiSerial
{
public:
  CMidiSerial(){}
  
  void Begin(int Baudrate)
  {
    Serial.begin(Baudrate);
  }
  void ControlChange(byte Channel, byte Controller, byte Value)
  {
    byte Operation = 0xB0 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Controller & 0x7F);
    Serial.write(Value & 0x7F);
  }
  void NoteOn(byte Channel, byte Note, byte Velocity)
  {
    byte Operation = 0x90 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Note & 0x7F);
    Serial.write(Velocity & 0x7F);    
  }
  void NoteOff(byte Channel, byte Note, byte Velocity)
  {
    byte Operation = 0x80 | (Channel & 0x0F);
    Serial.write(Operation);
    Serial.write(Note & 0x7F);
    Serial.write(Velocity & 0x7F);    
  }
  //TODO pitchbend
};

#endif //MIDISERIAL_H
