#ifndef PERIODIC_H
#define PERIODIC_H

class CPeriodic
{
public:
    static const int NoAction = 0;
    static const int NoteOnAction = 1;
    static const int NoteOffAction = -1;

    CPeriodic();

    void UpdateTempo(int Change);
    
    int GetTempoBpm() const;

    int Update(unsigned long TimeStampMilliSeconds, int Duration);
    
    bool GetState() const;

private:
    int s_TempoBpm;
    unsigned long s_StartTimeStampMilliSeconds;
    bool s_State;
};

#endif // PERIODIC_H
