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

const int ButtonPin = 7; 
const int RotaryPin1 = 14;
const int RotaryPin2 = 15;
#define ENC_PORT PINC //PINC is the bank where pin 14 ad 15 are the first two pins

const int Size = 1024;
uint8_t RotaryCodes[Size];
int Cnt;

void setup() 
{
  Cnt = 0;
  
  pinMode(ButtonPin, INPUT_PULLUP);
  pinMode(RotaryPin1, INPUT_PULLUP);
  pinMode(RotaryPin2, INPUT_PULLUP);
  
  Serial.begin(9600);  
}

void loop() 
{  
   Serial.print("Grabbing...");
   delay(500);
   Serial.print("1 ");
   delay(500);
   Serial.print("2 ");
   delay(500);
   Serial.print("3 ");
   delay(500);
   Serial.println("Go!");
  
    unsigned long Before = millis();
    int idx = 0; 
    RotaryCodes[0] = 0xFF;
    while(idx<Size)
    {
      // only log transitions
      uint8_t CurrentRotaryCode = ( ENC_PORT & 0x03 );  //add current state. mask is 0x00000011 because using ports 14 and 15 on PORTC bus
      if(RotaryCodes[idx]!=CurrentRotaryCode)
      {
        ++idx;
        RotaryCodes[idx] = CurrentRotaryCode;
      }
      //delay(1);//????????     
    }
    unsigned long After = millis();
    int Duration = After - Before;
    
    // now log to serial
    Serial.println(Before);
    Serial.println(After);
    Serial.print("Duration of 1024 transitions: ");
    Serial.println(Duration);
    
    for(int idx = 0; idx<Size; ++idx)
    {
      Serial.print(RotaryCodes[idx], HEX);
      Serial.print(" ");
    }
    Serial.println();
  
    delay(500);
}

