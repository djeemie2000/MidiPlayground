#include "Stepping.h"
#include "Crop.h"

CStepping::CStepping()
 : m_StepSize(1)
 , m_StepIntervalBegin(0)
 , m_StepIntervalLength(MaxNumSteps)
{
}
int CStepping::GetStepSize() const
{
    return m_StepSize;
}

void CStepping::UpdateStepSize(int Change)
{
    m_StepSize = Crop(m_StepSize + Change, MinStepSize, MaxStepSize);
}

int CStepping::GetStepIntervalBegin() const
{
    return m_StepIntervalBegin;
}

void CStepping::UpdateStepIntervalBegin(int Change)
{
    m_StepIntervalBegin = Crop(m_StepIntervalBegin + Change, MinStepIntervalBegin, MaxStepIntervalBegin);
}

int CStepping::GetStepIntervalLength() const
{
    return m_StepIntervalLength;
}

void CStepping::UpdateStepIntervalLength(int Change)
{
    m_StepIntervalLength = Crop(m_StepIntervalLength + Change, MinStepIntervalLength, MaxStepIntervalLength);
}

int CStepping::Advance(int Step)
{
    int IntervalLength = m_StepIntervalLength<MaxNumSteps-m_StepIntervalBegin ? m_StepIntervalLength : MaxNumSteps-m_StepIntervalBegin;
    int AdvancedStep = m_StepIntervalBegin + (Step-m_StepIntervalBegin+m_StepSize)%IntervalLength;
    return AdvancedStep;
}
