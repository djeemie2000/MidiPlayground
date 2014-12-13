//shift register experiment
//coded by zxlee
//iamzxlee.wordpress.com
 
  
#define enable 2
#define load 3
#define clock 4
#define data 5
 
void setup()
{
pinMode(enable,OUTPUT);
pinMode(load,OUTPUT);
pinMode(clock,OUTPUT);
pinMode(data,INPUT);
digitalWrite(load,HIGH);
digitalWrite(enable,HIGH);
Serial.begin(9600);
}
 
void loop()
{
  // pulse 5 microsec on PL
  digitalWrite(load,LOW);
  delayMicroseconds(5);
  digitalWrite(load,HIGH);
  delayMicroseconds(5);
 
  digitalWrite(clock,HIGH);
  digitalWrite(enable,LOW);
  
  byte incoming=shiftIn(data,clock,MSBFIRST);
  
  digitalWrite(enable,HIGH);
 
  for(int i=7;i>=0;i--)
  {
    if(bitRead(incoming,i)==1)
    {
      Serial.print("1");
    }
    else
    {
      Serial.print("0");
    }
  }
  Serial.println();
 
 
   delay(250);
}

