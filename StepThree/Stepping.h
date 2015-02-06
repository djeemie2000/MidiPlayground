#ifndef STEPPING_H
#define STEPPING_H

class CStepping
{
public:
    CStepping();

    int GetStepSize() const;
    void UpdateStepSize(int Change);

    int GetStepIntervalBegin() const;
    void UpdateStepIntervalBegin(int Change);

    int GetStepIntervalLength() const;
    void UpdateStepIntervalLength(int Change);

    int Advance(int Step);//!< returns advanced step

private:
    static const int MaxNumSteps = 8;
    static const int MinStepSize = 1;
    static const int MaxStepSize = MaxNumSteps;
    static const int MinStepIntervalBegin = 0;
    static const int MaxStepIntervalBegin = MaxNumSteps -1;
    static const int MinStepIntervalLength = 1;
    static const int MaxStepIntervalLength = MaxNumSteps;

    int m_StepSize;
    int m_StepIntervalBegin;
    int m_StepIntervalLength;
};

#endif // STEPPING_H
