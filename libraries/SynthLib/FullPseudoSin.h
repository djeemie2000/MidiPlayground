#ifndef FULLPSEUDOSIN_H
#define FULLPSEUDOSIN_H

template<class T>
class CFullPseudoSin
{
public:
    CFullPseudoSin(){}
    T operator()(const T& Phase) const
    {
        return 0<Phase ? CalcPseudoSin(Phase) : -CalcPseudoSin(-Phase);
    }
private:
    T CalcPseudoSin(const T& Phase) const
    {
        return 4*Phase*(1-Phase);
    }
};

#endif // FULLPSEUDOSIN_H
