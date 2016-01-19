#include <Wire.h>
#include "Adafruit_LEDBackpack.h"

Adafruit_24bargraph bar;

void setup() {
  Serial.begin(115200);
  Serial.println("HT16K33 Bi-Color Bargraph test");
  
  bar.begin(0x70);  // pass in the address

  for (uint8_t b=0; b<24; b++ ){
    if ((b % 3) == 0)  bar.setBar(b, LED_RED);
    if ((b % 3) == 1)  bar.setBar(b, LED_YELLOW);
    if ((b % 3) == 2)  bar.setBar(b, LED_GREEN);
  }
  bar.writeDisplay();
  delay(2000);
}


void loop() 
{
 for (uint8_t b=0; b<24; b++) 
 {
   bar.setBar(b, LED_RED);
   bar.writeDisplay();
   delay(50);
   bar.setBar(b, LED_OFF);
   bar.writeDisplay();
 }
  
  for (uint8_t b=0; b<24; b++) 
  {
   bar.setBar(b, LED_GREEN);
   bar.writeDisplay();
   delay(50);
   bar.setBar(b, LED_OFF);
   bar.writeDisplay();
 }

 for (uint8_t b=0; b<24; b++) 
 {
   bar.setBar(23-b, LED_YELLOW);
   bar.writeDisplay();
   delay(50);
   bar.setBar(23-b, LED_OFF);
   bar.writeDisplay();
 }
 
 return;
 
 for (uint8_t b=0; b<24; b++ ){
    if ((b % 3) == 0)  bar.setBar(b, LED_RED);
    if ((b % 3) == 1)  bar.setBar(b, LED_YELLOW);
    if ((b % 3) == 2)  bar.setBar(b, LED_GREEN);
  }
  bar.writeDisplay();
}
