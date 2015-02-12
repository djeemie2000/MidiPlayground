
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

CPhaseAccumulator<float> PhaseAcc1;
CPhaseAccumulator<float> PhaseAcc2;
float BasePhaseIncr;
volatile float PhaseIncr;
volatile float Detune;
//float Phase;
//int Value;
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
  WaveForm = 2;
  Detune = 0.5001;

  Serial.begin(115200);
//  Serial.print("PhaseIncr=");
//  Serial.println(PhaseIncr);

  const float SamplingFrequency = 1760;
  float NoteFrequency = 80;
  BasePhaseIncr = 2*NoteFrequency/SamplingFrequency;
  PhaseIncr = BasePhaseIncr;
  
  beginTimer1(SamplingFrequency);
}


void loop()
{
  for(int Repeat = 0; Repeat<3; ++Repeat)
  {
    delay(2000);
  //  if(WaveForm==2)
  //  {
  //    WaveForm = 4;
  //  }
  //  else {
  //    WaveForm = 2;
  //  }
    WaveForm = (WaveForm+1)%5;
    unsigned long Cnt = InteruptCount;
    InteruptCount = 0;
   // Serial.println(Cnt);
  }
  
  PhaseIncr *= 1.3;
  if(4*BasePhaseIncr<PhaseIncr)
  {
    PhaseIncr = BasePhaseIncr;//(1+0.25*WaveForm);
  }
 
  //Serial.print(Phase);//????????????
  //Serial.print(" ");//????????????
  //Serial.println(Value);//????????????
  
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
  ++InteruptCount;
  
  //PhaseIncr *= 0.997;
  float Phase1 = PhaseAcc1(PhaseIncr);
  float Phase2 = PhaseAcc2(Detune*PhaseIncr);
  
  // SawUp
  float Out = 0;
  if(WaveForm==0)
  {
    Out = SawUp(Phase1) + SawUp(Phase2);
  }
  else if(WaveForm==1)
  {
    Out = SawDown(Phase1) + SawDown(Phase2);
  }
  else if(WaveForm==2)
  {
    Out = Pulse(Phase1) + Pulse(Phase2);
  }
  else if(WaveForm==3)
  {
    Out = PseudoSin(Phase1) + PseudoSin(Phase2);
  }
  else if(WaveForm==4)
  {
    Out = FullPseudoSin(Phase1) + FullPseudoSin(Phase2);
  }
  else if(WaveForm==5)
  {
    Out = Power(Phase1) + Power(Phase2);
  }
  
  float Amplitude = 0.5f;
  int Value = 512*(1+Amplitude*Out);//convert [-1,+1] to [0, 1024]
  Value = max(0, min(1023, Value));// limit!
  
  analogWrite(OutPin, Value);
}

