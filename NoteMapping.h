#ifndef NOTEMAPPING_H
#define NOTEMAPPING_H

#include <Arduino.h>
#include "PitchToNote.h"
#include "RegisterToNote.h"
#include "Config.h"

const boolean noteCombinations[EWI_NOTERANGE][EWI_BUTTONS] = {
  {1,0,1,1,0,1,0,1,1,1,0,1,1},  // A#
  {1,0,1,1,0,1,0,1,1,1,0,0,1},  // B
  {1,0,1,1,0,0,0,1,1,1,0,0,1},  // C
  {1,0,1,1,1,0,0,1,1,1,0,0,1},  // C#
  {1,0,1,1,0,0,0,1,1,1,0,0,0},  // D
  {1,0,1,1,0,0,0,1,1,1,1,0,0},  // D#
  {1,0,1,1,0,0,0,1,1,0,0,0,0},  // E
  {1,0,1,1,0,0,0,1,0,0,0,0,0},  // F
  {1,0,1,1,0,0,0,0,1,0,0,0,0},  // F#
  {1,0,1,1,0,0,0,0,0,0,0,0,0},  // G
  {1,0,1,1,1,0,0,0,0,0,0,0,0},  // G#
  {1,0,1,0,0,0,0,0,0,0,0,0,0},  // A
  {1,0,1,0,0,0,1,0,0,0,0,0,0},  // A#
  {1,1,0,0,0,0,0,0,0,0,0,0,0},  // A# alt
  {1,0,0,0,0,0,0,0,0,0,0,0,0},  // B
  {0,0,1,0,0,0,0,0,0,0,0,0,0},  // C
  {0,0,0,0,0,0,0,0,0,0,0,0,0},  // C#
  {0,0,0,0,1,0,0,0,0,0,0,0,0}   // D
};

// Returns the note index for the given register states, or -1 if no match
static inline int getNoteIndex(boolean* touchStates) {
  for (int i = 0; i < EWI_NOTERANGE; i++) {
    bool match = true;
    for (int j = 0; j < EWI_BUTTONS; j++) {
      if (touchStates[noteButtonMap[j]] != noteCombinations[i][j]) {
        match = false;
        break;
      }
    }
    if (match) return i; // index into notePitches
  }
  return -1;
}

// Returns the *index* into notePitches[] for given registers
static inline int getNoteIndexForRegisters(boolean* touchStates) {
  return getNoteIndex(touchStates); // just return index
}


// Returns true if this note has no buttons pressed (blow-only)
bool isBlowOnly(int noteIndex) {
    for (int i = 0; i < EWI_BUTTONS; i++) {
        if (noteCombinations[noteIndex][i]) return false;
    }
    return true;
}




#endif
