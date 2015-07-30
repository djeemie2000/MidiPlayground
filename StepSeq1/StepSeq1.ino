
const int LedPin = 13;
bool ClockPulse;
int CurrentStep;
const int NumSteps = 4;

const int SelectPin = 8;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(SelectPin, OUTPUT);
  pinMode(SelectPin+1, OUTPUT);
  pinMode(SelectPin+2, OUTPUT);

  pinMode(A0, INPUT);
  
  CurrentStep = 0;
  ClockPulse = false;
  // interrupt upon rising edge of digital input 2 (= interrupt number 0)
  attachInterrupt(0, OnClockPulse, RISING);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(ClockPulse)
  {
    AdvanceStep();

    ApplyStep();

    LogStep();

    ClockPulse = false;
  }
}

void AdvanceStep()
{
  CurrentStep = (CurrentStep+1)%NumSteps;
  Serial.print("Advance to step ");
  Serial.println(CurrentStep);  
}

void ApplyStep()
{
  // select channel on 4051
  int Value0 = CurrentStep & 0x01;
  int Value1 = CurrentStep>>1 & 0x01;
  int Value2 = CurrentStep>>2 & 0x01;
  digitalWrite(SelectPin, 0!=Value0 ? HIGH : LOW);
  digitalWrite(SelectPin+1, 0!=Value1 ? HIGH : LOW);
  digitalWrite(SelectPin+2, 0!=Value2 ? HIGH : LOW);

  /*Serial.print(Value0);
  Serial.print(" ");
  Serial.print(Value1);
  Serial.print(" ");
  Serial.print(Value2);
  Serial.println();*/
}

void LogStep()
{
  int Value = analogRead(A0);
  Serial.print("Value=");
  Serial.println(Value);
}

void OnClockPulse()
{
  ClockPulse = true;
  //avoid bouncing?
  delay(10);
}

