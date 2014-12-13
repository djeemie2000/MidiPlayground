//**************************************************************//
//  Name    : shiftOutCode, Hello World                                
//  Author  : Carlyn Maw,Tom Igoe, David A. Mellis 
//  Date    : 25 Oct, 2006    
//  Modified: 23 Mar 2010                                 
//  Version : 2.0                                             
//  Notes   : Code for using a 74HC595 Shift Register           //
//          : to count from 0 to 255                           
//****************************************************************

//Pin connected to ST_CP of 74HC595
int latchPin = 8;
//Pin connected to SH_CP of 74HC595
int clockPin = 12;
////Pin connected to DS of 74HC595
int dataPin = 11;

// pin used to measure button pressed
int buttonPin = 7;

int flashPin = 13;


void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(buttonPin, INPUT);
  
  pinMode(flashPin, OUTPUT);
}

void loop() {

  digitalWrite(flashPin, HIGH);
  
  // read button1
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 16);    
  digitalWrite(latchPin, HIGH);
  bool Button1Pressed = (HIGH==digitalRead(buttonPin));
    
  // read button2
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, 32);    
  digitalWrite(latchPin, HIGH);
  bool Button2Pressed = (HIGH==digitalRead(buttonPin));
  
  delay(100);
  digitalWrite(flashPin, LOW);
  
  //  for (int numberToDisplay = 0; numberToDisplay < 4; numberToDisplay++) {
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(latchPin, LOW);
    // shift out the bits:
    int numberToDisplay = 0;
    numberToDisplay += Button1Pressed ? 1 : 0;
    numberToDisplay += Button2Pressed ? 2 : 0;
    shiftOut(dataPin, clockPin, MSBFIRST, numberToDisplay);  
    //take the latch pin high so the LEDs will light up:
    digitalWrite(latchPin, HIGH);
    
    // pause before next button read:
    delay(2000);
//  }
}
