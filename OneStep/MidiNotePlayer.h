#ifndef MIDINOTEPLAYER_H
#define MIDINOTEPLAYER_H

#include <MidiSerial.h>

class CMidiNotePlayer
{
public:
    CMidiNotePlayer()
     : m_MidiChannel(1)
     , m_MidiNote(0x64)
     , m_Velocity(0x7F)
     , m_NoteOn(false)
     , m_MidiSerial()
    {
    }
    
    void Begin();

    void NoteOn(uint8_t MidiNote, uint8_t Velocity);
    void NoteOff();

private:
    uint8_t m_MidiChannel;
    uint8_t m_MidiNote;
    uint8_t m_Velocity;
    bool m_NoteOn;
    
    CMidiSerial m_MidiSerial;
};

#endif // MIDINOTEPLAYER_H

