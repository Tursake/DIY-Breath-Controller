#include "RegisterToNote.h"

const int noteButtonMap[EWI_BUTTONS] = {
  11,10,9,8,7,6,5,4,3,2,1,0,23
};


const byte notePitches[EWI_NOTERANGE] = {
  PitchToNote::B3b_,  // A#3
  PitchToNote::B3_,   // B3
  PitchToNote::C4_,   // C4
  PitchToNote::C4s_,  // C#4
  PitchToNote::D4_,   // D4
  PitchToNote::D4s_,  // D#4
  PitchToNote::E4_,   // E4
  PitchToNote::F4_,   // F4
  PitchToNote::F4s_,  // F#4
  PitchToNote::G4_,   // G4
  PitchToNote::G4s_,  // G#4
  PitchToNote::A4_,   // A4
  PitchToNote::B4b_,  // A#4
  PitchToNote::B4b_,  // A#4 (alt)
  PitchToNote::B4_,   // B4
  PitchToNote::C5_,   // C5
  PitchToNote::C5s_,  // C#5
  PitchToNote::D5_    // D5
};
