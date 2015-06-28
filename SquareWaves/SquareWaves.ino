/*
 Piezo
 
 This example shows how to run a Piezo Buzzer on pin 9
 using the analogWrite() function.
 
 It beeps 3 times fast at startup, waits a second then beeps continuously
 at a slower pace
 
 */

const int PwmPin = 53;//7;

void setup()  { 
  // declare pin PwmPin to be an output:
  pinMode(PwmPin, OUTPUT);
  //beep(50, 20);
  //beep(50, 20);
  //beep(50, 20);
  delay(1000);
} 

void loop()  
{ 
  for(int idx = 60; idx<101; idx+=20)
  {
    for(int PulseWidth = 16; PulseWidth<=240; PulseWidth += 16)
    {
      squareBeep(idx, 500, PulseWidth);
      squareBeep(idx/2, 500, PulseWidth);//octave below
    }
  }
}

void beep(unsigned char delayms, unsigned char pulsewidth)
{
  analogWrite(PwmPin, pulsewidth);      // Almost any value can be used except 0 and 255
                           // experiment to get the best tone
  delay(delayms);          // wait for a delayms ms
  analogWrite(PwmPin, 0);       // 0 turns it off
  delay(delayms);          // wait for a delayms ms   
}  

const int PulseWidthScale = 256;

void squareBeep(int FreqHz, int DurationMilliSeconds, int PulseWidth)//pw: scale of 256
{
  int PeriodMicroSeconds = 1000000/FreqHz;
  int DurationPeriods = DurationMilliSeconds*FreqHz/1000;
  for(int idx = 0; idx<DurationPeriods; ++idx)
  {
    digitalWrite(PwmPin, HIGH);
    delayMicroseconds(PeriodMicroSeconds*PulseWidth/(2*PulseWidthScale));
    digitalWrite(PwmPin, LOW);
    delayMicroseconds(PeriodMicroSeconds*(PulseWidthScale-PulseWidth)/(2*PulseWidthScale));
  }
}

