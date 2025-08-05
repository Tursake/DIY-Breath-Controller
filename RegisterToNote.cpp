#include "RegisterToNote.h"

const byte register1Values[EWI_NOTERANGE] = {
  72, 72, 78, 70, 78, 78, 78, 78, 79, 79, 71, 95, 93, 63, 127, 222, 255, 247
};

const byte register2Values[EWI_NOTERANGE] = {
  16, 48, 48, 48, 112, 96, 120, 122, 120, 122, 122, 122, 122, 122, 122, 122, 122, 122
};


const byte notePitches[EWI_NOTERANGE] = {
  PitchToNote::B3b_,  // A#
  PitchToNote::B3_,   // B
  PitchToNote::C4_,   // C
  PitchToNote::C4s_,  // C# (alias for D4b_)
  PitchToNote::D4_,   // D
  PitchToNote::D4s_,  // D# (alias for E4b_)
  PitchToNote::E4_,   // E
  PitchToNote::F4_,   // F
  PitchToNote::F4s_,  // F#
  PitchToNote::G4_,   // G
  PitchToNote::G4s_,  // G#
  PitchToNote::A4_,   // A
  PitchToNote::B4b_,  // A# (alt)
  PitchToNote::B4b_,  // A# (alt 2)
  PitchToNote::B4_,   // B (alt)
  PitchToNote::C5_,   // C (alt)
  PitchToNote::C5s_,  // C# (alt) (alias for D5b_)
  PitchToNote::D5_,   // D (alt)
};