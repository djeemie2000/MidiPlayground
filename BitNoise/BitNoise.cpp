#include "BitNoise.h"

CBitNoise::CBitNoise()
{
	reg = 0x55aa55aaL; //The seed for the bitstream. 	
}
int CBitNoise::Generate()
{
    unsigned char b31 = (reg & (1L << 31)) >> 31;
    unsigned char b29 = (reg & (1L << 29)) >> 29;
    unsigned char b25 = (reg & (1L << 25)) >> 25;
    unsigned char b24 = (reg & (1L << 24)) >> 24;
    unsigned char lobit = b31 ^ b29 ^ b25 ^ b24;
    reg = (reg << 1) | lobit;
    
    return reg & 1;
};

