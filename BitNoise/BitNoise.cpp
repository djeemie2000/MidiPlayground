#include "BitNoise.h"

CBitNoise::CBitNoise()
{
	m_Register = 0x55aa55aaL; //The seed for the bitstream. 	
}

void CBitNoise::Generate()
{
    unsigned char b31 = (m_Register & (1L << 31)) >> 31;
    unsigned char b29 = (m_Register & (1L << 29)) >> 29;
    unsigned char b25 = (m_Register & (1L << 25)) >> 25;
    unsigned char b24 = (m_Register & (1L << 24)) >> 24;
    unsigned char lobit = b31 ^ b29 ^ b25 ^ b24;
    
    m_Register = (m_Register << 1) | lobit;
 };

