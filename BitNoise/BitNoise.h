
#pragma once


class CBitNoise
{
public:
    CBitNoise();
    void Generate();
    inline int Get() 
    {     
      return m_Register & 1; 
    }

private:
    unsigned long int m_Register;
};

