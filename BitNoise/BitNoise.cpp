#include "BitNoise.h"

CBitNoise::CBitNoise()
{
	reg = 0x55aa55aaL; //The seed for the bitstream. 	
}
int CBitNoise::Generate()
{
    b31 = (reg & (1L << 31)) >> 31;
    b29 = (reg & (1L << 29)) >> 29;
    b25 = (reg & (1L << 25)) >> 25;
    b24 = (reg & (1L << 24)) >> 24;
    lobit = b31 ^ b29 ^ b25 ^ b24;
    newr = (reg << 1) | lobit;
    reg = newr;

    return reg & 1;
};

