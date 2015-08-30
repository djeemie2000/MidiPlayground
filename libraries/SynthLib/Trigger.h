#ifndef TRIGGER_H
#define TRIGGER_H

template<class T>
class CTriggerIn
{
public:
    static const T TriggerThreshold = static_cast<T>(0.99);

    CTriggerIn(){}

    bool IsTriggerOn(T Trigger)
    {
        return TriggerThreshold<Trigger;
    }

    bool IsTriggerOff(T Trigger)
    {
        return Trigger<-TriggerThreshold;
    }
};

template<class T>
class CGateToTrigger
{
public:
    CGateToTrigger()
        : m_PrevGate(0)
    {}

    T operator()(T Gate)
    {
        // assume Gate is 0 (off) or 1 (on)
        // => Trigger is 0 (Gate unchanged), +1 (Gate off -> on) or -1 (Gate on -> off)
        T Trigger = Gate - m_PrevGate;
        m_PrevGate = Gate;
        return Trigger;
    }

private:
    T m_PrevGate;
};

#endif // TRIGGER_H
