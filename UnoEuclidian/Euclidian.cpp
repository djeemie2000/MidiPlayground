#include "Euclidian.h"
#include "EuclidianPatterns.h"

namespace euclidian
{

uint32_t CalcActiveMask(int Length)
{
  if(Length<1)
  {    
    Length = 1;
  }
  else if(euclidian::MaxPatternLength<Length)
  {
    Length = MaxPatternLength;
  }
  
  return (1<<Length)-1;
}

uint32_t CalcPattern(int Length, int Fills)
{
  if(Length<1)
  {    
    Length = 1;
  }
  else if(euclidian::MaxPatternLength<Length)
  {
    Length = MaxPatternLength;
  }

  if(Fills<0)
  {    
    Fills = 0;
  }
  else if(Length<Fills)
  {
    Fills = Length;
  }

  int Index = 0;
  for(int m = 1; m<Length; ++m)
  {
    // there are m+1 n out of m patterns
    Index += m+1;
  }
  Index += Fills;  

  // invert the pattern from the LUT
  uint32_t Pattern = euclidian::EuclidianPatterns[Index];
  uint32_t InvertedPattern = 0;

  for(int Bit = 0; Bit<Length; ++Bit)
  {
    uint32_t BitMask = 1<<(Length-Bit-1);
    if(Pattern & BitMask)
    {
      InvertedPattern |= (1<<Bit);
    }
  }
  
  return InvertedPattern;
}

}

