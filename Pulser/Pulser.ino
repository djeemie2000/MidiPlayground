/*!
 * Pulser:
 * CV + gate in -> pulse out, pulse length ~CV 
 */

const int NumPulses = 4;
const int CVInPin = A0;
const int GateInPin = 3;//??
const int PulseOutPin = 8;//??

int g_Gate[NumPulses];
int g_PulseCounter[NumPulses];
int g_PulseLength[NumPulses];


void setupFastAnalogRead()
{
  // bits [2:0] of ADCSRA register determine clock divider
  // by default it is 128 ~ 111
  // we can set it to 32 ~ 101
  // we can set it to 16 ~ 100
  // we can set it to 8  ~ 011
  ADCSRA &= (0xF8 | 0x04);
}

class CTimings
{
  public:
  CTimings() 
  : m_Counter(0)
  , m_Millis(0)
  {}

  void Tick(unsigned long Interval)
  {
    ++m_Counter;
    if(Interval<=m_Counter)
    {
      m_Counter = 0;
      unsigned long PrevMillis = m_Millis;
      m_Millis = millis();

      Serial.print(Interval);
      Serial.print(" x -> ");
      Serial.print(m_Millis-PrevMillis);
      Serial.println(" ms");
    }
  }

  private:
    unsigned long m_Counter;
    unsigned long m_Millis;
};

CTimings g_Timings;

void setup() 
{
  Serial.begin(115200);
  Serial.println("Pulser...");
  
  setupFastAnalogRead();

  for(int Pulse = 0; Pulse<NumPulses; ++Pulse)
  {
    pinMode(CVInPin+Pulse, INPUT);
    pinMode(GateInPin+Pulse, INPUT);
    pinMode(PulseOutPin+Pulse, OUTPUT);

    g_Gate[Pulse] = 0;
    g_PulseCounter[Pulse] = 0;
    g_PulseLength[Pulse] = 1;
  }
}

void loop() 
{
  // alternating (?):  read one pulse length CV 

  // read gate input
  // update pulse counter
  // Note: pulse counter == 0 <=> no pulse
  // write all pulse output
  for(int Pulse = 0; Pulse<NumPulses; ++Pulse)
  {
    g_PulseLength[Pulse] = 1+analogRead(CVInPin+Pulse)*2;// scaling TBD 
    
    int Gate = digitalRead(GateInPin+Pulse);//?? use fast digital in? read PORT??
    
    if(!g_Gate[Pulse] && Gate)
    { // (re)start pulse
      g_Gate[Pulse] = 1;
      g_PulseCounter[Pulse] = 1;
    }

    if(g_PulseCounter[Pulse])
    { // pulse is ongoing
      if(g_PulseLength[Pulse]<g_PulseCounter[Pulse])
      { // end pulse
        g_PulseCounter[Pulse] = 0;
      }
      else
      { // continue pulse
        ++g_PulseCounter[Pulse];
      }
    }
    
    //TODO use fast digital out! write PORT??
    digitalWrite(PulseOutPin+Pulse, g_PulseCounter[Pulse]);
  }
  
  delayMicroseconds(500);//TODO some global speed CV??

  // print timing every ... repeats
  g_Timings.Tick(10000);
}

