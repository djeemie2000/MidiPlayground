#ifndef CLOCKTICK_H
#define CLOCKTICK_H

class CClockTick
{
public:
    CClockTick();

    void Begin(int Bpm, int SubDivision, unsigned long MilliSeconds);
    //TODO change Bpm
    void UpdateBpm(int Change);

    void Update(unsigned long MilliSeconds);
    unsigned long GetCurrentClockTick() const;
    int GetBpm() const;

private:
    int m_Bpm;
    int m_SubDivision;
    unsigned long m_ReferenceMilliSeconds;
    unsigned long m_ReferenceClockTick;
    unsigned long m_CurrentMilliSeconds;
    unsigned long m_CurrentClockTick;
};

#endif // CLOCKTICK_H
