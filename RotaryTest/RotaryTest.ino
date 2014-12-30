

const int PinA = 2;
const int PinB = 3;

unsigned int RotaryValue;
int ChangeCount;
int Position;

void setup()
{
  pinMode(PinA, INPUT);
  pinMode(PinB, INPUT);
  
  RotaryValue = 0;
  ChangeCount = 0;
  Position = 0;
  
  Serial.begin(115200);
}

void PrintState()
{  
  Serial.print((RotaryValue/4)%4);
  Serial.print(" ");
  Serial.print(RotaryValue%4);
  Serial.print(" #");
  Serial.print(ChangeCount);
  Serial.print(" -> ");
  Serial.println(Position);
}

void loop()
{
  //
  unsigned int NewRotaryValue = 2*digitalRead(PinA) + digitalRead(PinB);
  unsigned int PrevRotaryValue = RotaryValue%4;
  if(NewRotaryValue != PrevRotaryValue)
  {
    // change
    ++ChangeCount;

    if(!NewRotaryValue)
    {
      // new pos => increase or decrease?
      if(PrevRotaryValue == 2)
      {
        ++Position;
      }      
      else if(PrevRotaryValue == 1)
      {
        --Position;
      }
      else if(PrevRotaryValue == 3)
      {
        // skipped one
        unsigned int PrevPrevRotaryValue = (RotaryValue/4)%4;
        if(PrevPrevRotaryValue == 1)
        {
          ++Position;
        }      
        else if(PrevPrevRotaryValue == 2)
        {
          --Position;
        }
      }      
    }

    RotaryValue = 4*RotaryValue + NewRotaryValue;    
    
    if(!NewRotaryValue)
    {
      PrintState();
    }
  }
  
  delay(5);//will this prevent proper operation?
  // 10 mSec will sometimes cause skips
  // 5 mSec or below will give good performance
}

