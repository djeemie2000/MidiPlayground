
#include "MIDI.h"//serial midi on Serial1 
//#include "usb_api.h"

// use this to switch between serial1 and usb midi
#ifdef usbMIDI
#define MIDI_USED usbMIDI
#define SERIAL_USED Serial1
#else
#define MIDI_USED MIDI
#define SERIAL_USED Serial
#endif

void OnNoteOn(byte Channel, byte Note, byte Velocity)
{
  SERIAL_USED.print("NoteOn : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" note ");
  SERIAL_USED.print(Note, HEX);
  SERIAL_USED.print(" vel ");
  SERIAL_USED.println(Velocity);
}

void OnNoteOff(byte Channel, byte Note, byte Velocity)
{
  SERIAL_USED.print("NoteOff : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" note ");
  SERIAL_USED.print(Note, HEX);
  SERIAL_USED.print(" vel ");
  SERIAL_USED.println(Velocity);
}

void OnControlChange(byte Channel, byte Number, byte Value)
{
  SERIAL_USED.print("CC : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" nr ");
  SERIAL_USED.print(Number, HEX);
  SERIAL_USED.print(" val ");
  SERIAL_USED.println(Value);
}

void OnPitchBend(byte Channel, int Bend)
{
  SERIAL_USED.print("PitchBend : ch ");
  SERIAL_USED.print(Channel, DEC);
  SERIAL_USED.print(" val ");
  SERIAL_USED.println(Bend);
}

void TestSerial1In()
{
  #ifndef usbMIDI

  Serial.println("Testing serial 1 in...");
  while(true)
  {
    if(0<Serial1.available())
    {
      int Byte = Serial1.read();
      Serial.print(Byte, HEX);
      Serial.println(' ');
    }
  }
  
  #endif
}

void setup() {
  // put your setup code here, to run once:
  SERIAL_USED.begin(115200);
  delay(1000);//?
  
  SERIAL_USED.println("Teensy Midi In test...");

  #ifndef usbMIDI
  MIDI_USED.begin(MIDI_CHANNEL_OMNI, 115200);//for now, use midi-ox or similar
  #endif

  //TestSerial1In();
  
  MIDI_USED.setHandleNoteOn(OnNoteOn);
  MIDI_USED.setHandleNoteOff(OnNoteOff);
  MIDI_USED.setHandleControlChange(OnControlChange);
  #ifndef usbMIDI
  MIDI_USED.setHandlePitchBend(OnPitchBend);
  #endif
  #ifdef usbMIDI
  MIDI_USED.setHandlePitchChange(OnPitchBend);
  #endif
  
  pinMode(13, OUTPUT);
  SERIAL_USED.println("Starting..");
}

void loop() {
  // put your main code here, to run repeatedly:
  for(int Repeat = 0; Repeat<100000; ++Repeat)
  {
    MIDI_USED.read();
  }
  //SERIAL_USED.println("Midi read 100000x");
}
