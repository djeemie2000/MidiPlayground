
const int NumPins = 1;

const int AnalogInPin1 = A0;

void setup()
{
  for(int idx = 0; idx<NumPins; ++idx)
  {
    pinMode(AnalogInPin1+idx, INPUT);
  }
  
  Serial.begin(115200);
}


void loop()
{
  int Values[NumPins];
  for(int idx = 0; idx<NumPins; ++idx)
  {
    Values[idx] = analogRead(AnalogInPin1+idx);
  }

  const long Resolution = 1024;
  const long MaxVoltageMilliVolts = 5000;//4550;//why not 5000???;
  const long NotesPerOctave = 12;

  for(int idx = 0; idx<NumPins; ++idx)
  {
    int MilliVolts = Values[idx]*MaxVoltageMilliVolts/Resolution;
    int NoteOneVoltPerOctave = (MilliVolts*NotesPerOctave+500)/1000;
    Serial.print(Values[idx]);
    Serial.print(" -> ");
    Serial.print(MilliVolts);
    Serial.print(" mV -> note ");
    Serial.print(NoteOneVoltPerOctave);
    Serial.print(" ");
    Serial.print(NoteOneVoltPerOctave/NotesPerOctave);
    Serial.print(" ");
    Serial.print(NoteOneVoltPerOctave%NotesPerOctave);  
  }
  Serial.println();
 
 delay(500); 
}
