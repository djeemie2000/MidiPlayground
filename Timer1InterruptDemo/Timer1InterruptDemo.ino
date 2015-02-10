
volatile unsigned long Counter;

void beginTimer1(float FrequencyHz)
{
  // initialize timer1 
  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;

  const int PreScaler = 256;
  float Temp = FrequencyHz*PreScaler;//256 prescaler
  const unsigned long ClockFrequency = 16000000;
  unsigned long PeriodClockCycles = ClockFrequency / Temp;

  OCR1A = PeriodClockCycles; // compare match register 16MHz/256/xHz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  ++Counter;  
}

void setup()
{
  Counter = 0;
  Serial.begin(9600);
  
  float Freq = 1250.0f;
  beginTimer1(Freq);
}

void loop()
{
  Serial.print("Time= ");
  Serial.print(millis());
  Serial.print(" Counter= ");
  Serial.println(Counter);
  delay(500);
}

