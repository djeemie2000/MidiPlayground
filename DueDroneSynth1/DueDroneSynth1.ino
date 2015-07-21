#include "DueTimer.h"

int OutPin = 53;

const int SamplingFrequency = 20000;

const int FrequencyMultiplierScale = 1000;

const int FrequencyMultipliers[] = {
1000,
1011,
1022,
1033,
1044,
1056,
1067,
1079,
1091,
1102,
1114,
1127,
1139,
1151,
1164,
1176,
1189,
1202,
1215,
1228,
1242,
1255,
1269,
1283,
1297,
1311,
1325,
1340,
1354,
1369,
1384,
1399,
1414,
1430,
1445,
1461,
1477,
1493,
1509,
1526,
1542,
1559,
1576,
1593,
1610,
1628,
1646,
1664,
1682,
1700,
1719,
1737,
1756,
1775,
1795,
1814,
1834,
1854,
1874,
1895,
1915,
1936,
1957,
1978,
2000,
2022,
2044,
2066,
2089,
2111,
2134,
2158,
2181,
2205,
2229,
2253,
2278,
2302,
2327,
2353,
2378,
2404,
2430,
2457,
2484,
2511,
2538,
2566,
2594,
2622,
2650,
2679,
2709,
2738,
2768,
2798,
2828,
2859,
2890,
2922,
2954,
2986,
3018,
3051,
3084,
3118,
3152,
3186,
3221,
3256,
3292,
3327,
3364,
3400,
3437,
3475,
3513,
3551,
3589,
3629,
3668,
3708,
3748,
3789,
3830,
3872,
3914,
3957,
4000,
4044,
4088,
4132,
4177,
4223,
4269,
4315,
4362,
4410,
4458,
4506,
4555,
4605,
4655,
4706,
4757,
4809,
4861,
4914,
4967,
5022,
5076,
5131,
5187,
5244,
5301,
5359,
5417,
5476,
5536,
5596,
5657,
5718,
5781,
5844,
5907,
5972,
6037,
6102,
6169,
6236,
6304,
6373,
6442,
6512,
6583,
6655,
6727,
6800,
6874,
6949,
7025,
7102,
7179,
7257,
7336,
7416,
7497,
7578,
7661,
7744,
7829,
7914,
8000,
8087,
8175,
8264,
8354,
8445,
8537,
8630,
8724,
8819,
8915,
9012,
9110,
9210,
9310,
9411,
9514,
9617,
9722,
9828,
9935,
10043,
10152,
10263,
10375,
10488,
10602,
10717,
10834,
10952,
11071,
11192,
11314,
11437,
11561,
11687,
11815,
11943,
12073,
12205,
12338,
12472,
12608,
12745,
12884,
13024,
13166,
13309,
13454,
13601,
13749,
13899,
14050,
14203,
14358,
14514,
14672,
14832,
14993,
15157,
15322,
15488,
15657,
15828,
16000
};



class COscillator
{
  public:
    COscillator();

    void Begin(int Pin);
    void SetPeriod(int Period);
    void Update();
  private:
    int m_Pin;
    int m_Phase;
    int m_Period;
};

COscillator::COscillator()
 : m_Pin(-1)
 , m_Phase(0)
 , m_Period(1) 
{
}

void COscillator::Begin(int Pin)
{
    m_Pin = Pin;
    pinMode(m_Pin, OUTPUT);
}

void COscillator::SetPeriod(int Period)
{
  m_Period = Period;
}

void COscillator::Update()
{
  m_Phase++;
  m_Phase = m_Phase<m_Period ? m_Phase : 0;
  int OutputOn = m_Phase<m_Period/2 ? HIGH : LOW;
  digitalWrite(m_Pin, OutputOn); 
}

const int NumOscillators = 4;

COscillator Oscillator[NumOscillators];

void myHandler()
{
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    Oscillator[idx].Update();
  }
}

int CalcPeriod(int FrequencyMilliHz)
{
  return SamplingFrequency*1000/FrequencyMilliHz;
}

void setup()
{
  Serial.begin(115200);
  
  for(int idx = 0; idx<NumOscillators; ++idx)
  {
    Oscillator[idx].Begin(OutPin-2*idx);
    Oscillator[idx].SetPeriod(CalcPeriod(250000));
  }
  
  Timer3.attachInterrupt(myHandler);
  int SamplingPeriodMicroSeconds = 1000*1000/SamplingFrequency;
  Timer3.start(SamplingPeriodMicroSeconds); 
}

void loop()
{
  int FrequencyHz = 40; // base/lowest frequency
//  for(int DetuneMilliHz = 1; DetuneMilliHz <= 2000; DetuneMilliHz+= 50)
//  {
//    for(int idx = 0; idx<NumOscillators; ++idx)
//    {
//      Oscillator[idx].SetPeriod(CalcPeriod((1+idx/2)*FrequencyHz*1000+idx*DetuneMilliHz));
//    }
//    delay(1000);
//  }  
  for(int Repeat = 0; Repeat<500; ++Repeat)
  {
    for(int idx = 0; idx<NumOscillators; ++idx)
    {
      //
      int Value = (analogRead(A0+idx)/8)%256;
      int FreqMult = FrequencyMultipliers[Value];
      int OscillatorFrequencyMilliHz = FrequencyHz * FreqMult * 1000 / FrequencyMultiplierScale;
      //
      Oscillator[idx].SetPeriod(CalcPeriod(OscillatorFrequencyMilliHz));
      // debug info
      if(Repeat==0)
      {
        Serial.print(idx);
        Serial.print(" ");
        Serial.print(Value);
        Serial.print(" -> ");
        Serial.print(FreqMult);
        Serial.print(" -> ");
        Serial.print(OscillatorFrequencyMilliHz);
        Serial.println();
      }
        delay(1);//??
    }
  }    
}

