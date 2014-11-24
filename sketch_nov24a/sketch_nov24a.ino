/*
 MIDI Note player
 
 This sketch shows how to use the serial transmit pin (pin 1) to send MIDI Note data.
 If this circuit is connected to a MIDI synth, it will play 
 the Notes F#-0 (0x1E) to F#-5 (0x5A) in sequence.
 
 
 The circuit:
 * digital in 1 connected to MIDI jack pin 5
 * MIDI jack pin 2 connected to ground
 * MIDI jack pin 4 connected to +5V through 220-ohm resistor
 Attach a MIDI cable to the jack, then to a MIDI synth, and play music.
 
 created 13 Jun 2006
 modified 13 Aug 2012
 by Tom Igoe 
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/Midi
 
 */

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include "MidiSerial.h"

LiquidCrystal_I2C	lcd(0x27,2,1,0,4,5,6,7); // 0x27 is the I2C bus address for an unmodified backpack
CMidiSerial MidiSerial;

int Cntr;

void setup() 
{
  //  Set MIDI baud rate:
  //  Serial.begin(31250);
  MidiSerial.Begin(31250);

  // activate LCD module
  lcd.begin (16,2); // for 16 x 2 LCD module
  lcd.setBacklightPin(3,POSITIVE);
  lcd.setBacklight(HIGH);//backlight on
  
  Cntr = 0;
}

void loop() 
{
  ++Cntr;
  
  // tempo
  long TempoBpm = 120;
  long TempoPeriod = 60L * 1000L / TempoBpm;
  
  // step
  int Channel = 1;
  int Note = 50;
  int Velocity = 80;
  int Duration = 96;
  
  const int MaxDuration = 128;
  int NoteOnDuration = Duration * TempoPeriod / MaxDuration;
  int NoteOffDuration = (MaxDuration-Duration) * TempoPeriod / MaxDuration;

  // note on
  MidiSerial.NoteOn(Channel, Note, Velocity);

  // display update
  lcd.home (); // set cursor to 0,0
  lcd.print("N");  
  lcd.print(Note);
  lcd.print(" V");
  lcd.print(Velocity);
  lcd.print(" D");
  lcd.print(Duration);
  lcd.setCursor(0,1);
  lcd.print("Bpm ");
  lcd.print(TempoBpm);
  lcd.print("# ");
  lcd.print(Cntr);
//  lcd.print(" ");
//  lcd.print(NoteOnDuration);

  delay(NoteOnDuration);

  // note off
  MidiSerial.NoteOff(Channel, Note, Velocity);   
  delay(NoteOffDuration);

}

