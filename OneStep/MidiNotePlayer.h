#ifndef MIDINOTEPLAYER_H
#define MIDINOTEPLAYER_H

//#include <cstdint>
#include <MidiSerial.h>

class CMidiNotePlayer
{
public:
    CMidiNotePlayer()
     : m_MidiNote(0x64)
     , m_Velocity(0x7F)
     , m_MidiSerial()
    {
    }
    
    void Begin();

    void NoteOn(uint8_t MidiNote, uint8_t Velocity);
    void NoteOff();

private:
    uint8_t m_MidiNote;
    uint8_t m_Velocity;
    
    CMidiSerial m_MidiSerial;
};

void CMidiNotePlayer::Begin()
{
  m_MidiSerial.Begin(32150);
}

void CMidiNotePlayer::NoteOn(uint8_t MidiNote, uint8_t Velocity)
{
    m_MidiNote = MidiNote;
    m_Velocity = Velocity;
    // TODO
    m_MidiSerial.NoteOn(1, MidiNote, Velocity);
}

void CMidiNotePlayer::NoteOff()
{
    // use m_MidiNote, m_Velocity
    // TODO
    m_MidiSerial.NoteOff(1, m_MidiNote, m_Velocity);
}

#endif // MIDINOTEPLAYER_H

