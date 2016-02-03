

/*

Reading multiple analog input through a 4051 mux/demux

*/

const int AnalogReadPin = A0;// common analog read pin

const int SelectPin0 = A1;//2;
const int SelectPin1 = A2;//3;
const int SelectPin2 = A3;//4;

void setup()
{
  pinMode(AnalogReadPin, INPUT);
  
  pinMode(SelectPin0, OUTPUT);
  pinMode(SelectPin1, OUTPUT);
  pinMode(SelectPin2, OUTPUT); 
  
  Serial.begin(115200);
  
  Serial.println("Starting....");
}

int AnalogRead(int Input)
{
  
  // select input
  digitalWrite(SelectPin0, (Input&0x01) ? HIGH : LOW);
  digitalWrite(SelectPin1, (Input&0x02) ? HIGH : LOW);
  digitalWrite(SelectPin2, (Input&0x04) ? HIGH : LOW);
  
  int Value = analogRead(AnalogReadPin);
  return Value;
}

void loop()
{
   Serial.println("Measuring...");
   
   for(int Input = 0; Input<4; ++Input)
   {
     int Value = AnalogRead(Input);
     Serial.print(Input);
     Serial.print(" : ");
     Serial.println(Value);
     
//     delay(100);
   }
  
  delay(1000);
}

