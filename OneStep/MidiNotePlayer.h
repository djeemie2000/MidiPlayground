#ifndef MIDINOTEPLAYER_H
#define MIDINOTEPLAYER_H

//#include <cstdint>

class CMidiNotePlayer
{
public:
    CMidiNotePlayer()
     : m_MidiNote(0x64)
     , m_Velocity(0x7F)
    {}

    void NoteOn(uint8_t MidiNote, uint8_t Velocity);

    void NoteOff();

private:
    uint8_t m_MidiNote;
    uint8_t m_Velocity;
};


void CMidiNotePlayer::NoteOn(uint8_t MidiNote, uint8_t Velocity)
{
    m_MidiNote = MidiNote;
    m_Velocity = Velocity;
    // TODO
}

void CMidiNotePlayer::NoteOff()
{
    // use m_MidiNote, m_Velocity
    // TODO
}

#endif // MIDINOTEPLAYER_H
