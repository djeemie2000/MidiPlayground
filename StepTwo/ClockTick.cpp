#include "ClockTick.h"

CClockTick::CClockTick()
 : m_Bpm(120)
 , m_SubDivision(128)
 , m_ReferenceMilliSeconds(0)
 , m_ReferenceClockTick(0)
 , m_CurrentClockTick(0)
{
}

void CClockTick::Begin(int Bpm, int SubDivision, unsigned long MilliSeconds)
{
    m_Bpm = Bpm;
    m_SubDivision = SubDivision;
    m_ReferenceMilliSeconds = MilliSeconds;
    m_ReferenceClockTick = 0;//????
}

void CClockTick::UpdateBpm(int Change)
{
    m_Bpm += Change;
    m_ReferenceClockTick = m_CurrentClockTick;
    m_ReferenceMilliSeconds = m_CurrentMilliSeconds;
}

void CClockTick::Update(unsigned long MilliSeconds)
{
    m_CurrentMilliSeconds = MilliSeconds;
    unsigned long ElapsedTime = MilliSeconds - m_ReferenceMilliSeconds;
    float Period = 60 * 1000.0f / (m_Bpm * m_SubDivision);
    unsigned long ElapsedClockTick = ElapsedTime / Period;
    m_CurrentClockTick = ElapsedClockTick + m_ReferenceClockTick;
}

unsigned long CClockTick::GetCurrentClockTick() const
{
    return m_CurrentClockTick;
}

int CClockTick::GetBpm() const
{
    return m_Bpm;
}
