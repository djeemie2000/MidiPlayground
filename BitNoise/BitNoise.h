
#pragma once


class CBitNoise
{
public:
    CBitNoise();
    int Generate();
private:
    unsigned long int reg;
    unsigned long int newr;
    unsigned char lobit;
    unsigned char b31, b29, b25, b24;
};

