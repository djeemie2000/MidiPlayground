#ifndef MIDINOTEDISPLAY_H
#define MIDINOTEDISPLAY_H

#include <cstdint>

char MidiNoteToNote(uint8_t MidiNote)
{
    static const char Notes[] = { 'C', 'c', 'D', 'd', 'E', 'F', 'f', 'G', 'g', 'A', 'a', 'B' };
    return Notes[MidiNote%12];
}

int MidiNoteToOctave(uint8_t MidiNote)
{
     return MidiNote/12;
}

class CMidiNoteDisplay
{
public:
    CMidiNoteDisplay()
    {}

    void Update(uint8_t MidiNote, uint8_t Velocity, uint8_t Duration);
};


void CMidiNoteDisplay::Update(uint8_t MidiNote, uint8_t Velocity, uint8_t Duration)
{
    //TODO
}

#endif // MIDINOTEDISPLAY_H
