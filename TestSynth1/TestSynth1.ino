
// connect headphones : gnd to gnd , left and right to OutPin
// put a potmeter between gnd and OutPin to limit the amplitude

const int OutPin = A0;


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

template <class T>
class CPhaseAccumulator
{
public:
   CPhaseAccumulator()
   : m_Phase(-1)
   {}
   
   T operator()(T PhaseIncr)
   {
     m_Phase += PhaseIncr;
     if(1<m_Phase)
     {
       m_Phase -= 2;
     }
     return m_Phase;
   }
private:
    T m_Phase;
};

CPhaseAccumulator<float> PhaseAcc;
volatile float PhaseIncr;
float Phase;
int Value;
int WaveForm;
unsigned long InteruptCount = 0;

float SawUp(float Phase)
{
  return Phase;
}

float SawDown(float Phase)
{
  return -Phase;
}

float Pulse(float Phase)
{
  return Phase<0 ? -.99 : .99;
}

float PseudoSin(float Phase)
{
  return 2*(1+Phase)*(1-Phase)-1;
}

float CalcPseudoSin(float Phase)
{
  return 4*Phase*(1-Phase);
}

float FullPseudoSin(float Phase)
{
  return 0<Phase ? CalcPseudoSin(Phase) : -CalcPseudoSin(-Phase);
}

float Power(float Phase)
{
  float Power = Phase * Phase * Phase;
  return Power;
}

void setup()
{
  pinMode(OutPin, OUTPUT);
  WaveForm = 0;
  const float SamplingFrequency = 1760;
  float NoteFrequency = 80;
  PhaseIncr = 2*NoteFrequency/SamplingFrequency;
  beginTimer1(SamplingFrequency);
  Serial.begin(115200);
  Serial.print("PhaseIncr=");
  Serial.println(PhaseIncr);
}


void loop()
{

  delay(1000);
  WaveForm = (WaveForm+1)%5;
  unsigned long Cnt = InteruptCount;
  InteruptCount = 0;
  Serial.println(Cnt);

  //Serial.print(Phase);//????????????
  //Serial.print(" ");//????????????
  //Serial.println(Value);//????????????
  
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  ++InteruptCount;
  
  Phase = PhaseAcc(PhaseIncr);
  // SawUp
  float Out = 0;
  if(WaveForm==0)
  {
    Out = SawUp(Phase);
  }
  else if(WaveForm==1)
  {
    Out = SawDown(Phase);
  }
  else if(WaveForm==2)
  {
    Out = Pulse(Phase);
  }
  else if(WaveForm==3)
  {
    Out = PseudoSin(Phase);
  }
  else if(WaveForm==4)
  {
    Out = FullPseudoSin(Phase);
  }
  else if(WaveForm==5)
  {
    Out = Power(Phase);
  }
  
  Value = 512*(1+Out);//convert [-1,+1] to [0, 1024]
  Value = max(0, min(1023, Value));
  
  //Serial.print(Phase);//????????????
  //Serial.print(" ");//????????????
  //Serial.println(Value);//????????????
  
  analogWrite(OutPin, Value);
}

