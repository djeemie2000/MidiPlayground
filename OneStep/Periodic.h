#ifndef PERIODIC_H
#define PERIODIC_H

int BpmToMilliSeconds(int Bpm)
{
    // carefull : make sure calculation is in long and not in int!
    return 60L * 1000L / Bpm;
}


class CPeriodic
{
public:
    static const int NoAction = 0;
    static const int NoteOnAction = 1;
    static const int NoteOffAction = -1;

    CPeriodic()
        : s_TempoBpm(120)
        , s_StartTimeStampMilliSeconds(0)
        , s_State(false)
    {}

    void UpdateTempo(int Change)
    {
        s_TempoBpm += Change;
    }
    
    int GetTempoBpm() const
    {
      return s_TempoBpm;
    }

    int Update(unsigned long TimeStampMilliSeconds, int Duration)
    {
        int Action = NoAction;

        if(!s_State)
        {
            // currently off => check for note On
            unsigned long NewPeriodTimeStamp = s_StartTimeStampMilliSeconds + BpmToMilliSeconds(s_TempoBpm);
            if(NewPeriodTimeStamp<TimeStampMilliSeconds)
            {
                Action = NoteOnAction;
                s_State = true;
                s_StartTimeStampMilliSeconds = NewPeriodTimeStamp;//TimeStampMilliSeconds or use exact value???
            }
        }
        else
        {
            // currently on => check for note off
            int DurationMilliSeconds = Duration * BpmToMilliSeconds(s_TempoBpm) / 128;//maxDuration = 127 !!
            unsigned long NoteOffTimeStamp = s_StartTimeStampMilliSeconds+ DurationMilliSeconds;
            if(NoteOffTimeStamp<TimeStampMilliSeconds)
            {
                Action = NoteOffAction;
                s_State = false;
            }
        }

        return Action;
    }
    
    bool GetState() const
    {
      return s_State;
    }

private:
    int s_TempoBpm;
    unsigned long s_StartTimeStampMilliSeconds;
    bool s_State;

};


#endif // PERIODIC_H
