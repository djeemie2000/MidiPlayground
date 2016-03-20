#ifndef ROTARYENCODER_H
#define ROTARYENCODER_H

class CRotaryEncoder
{
public:
    CRotaryEncoder();

    void Begin(int PinA, int PinB);
    void Read();
    int GetPosition() const;
    int GetChange() const;
    void Reset();//!< reset to position, change count, ...

    int GetChangeCount() const;
    unsigned int GetRotaryHistory() const;

private:
    int m_PinA;
    int m_PinB;
    unsigned int m_RotaryHistory;
    int m_Position;
    int m_Change;
    int m_ChangeCount;
};

#endif // ROTARYENCODER_H
