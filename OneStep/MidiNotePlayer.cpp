#include "MidiNotePlayer.h"

void CMidiNotePlayer::Begin()
{
  m_MidiSerial.Begin(32150);
}

void CMidiNotePlayer::NoteOn(uint8_t MidiNote, uint8_t Velocity)
{
    m_MidiNote = MidiNote;
    m_Velocity = Velocity;
    m_NoteOn = true;
    // ?? check if !m_NoteOn ?

    m_MidiSerial.NoteOn(m_MidiChannel, MidiNote, Velocity);
}

void CMidiNotePlayer::NoteOff()
{
    // use m_MidiNote, m_Velocity
    if(m_NoteOn)
    {
      m_MidiSerial.NoteOff(m_MidiChannel, m_MidiNote, m_Velocity);
      m_NoteOn = false;
    }
}
