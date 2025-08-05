#ifndef NOTEMAPPING_H
#define NOTEMAPPING_H

#include <Arduino.h>
#include "PitchToNote.h"
#include "RegisterToNote.h"
#include "Config.h"

// Returns the note index for the given register states, or -1 if no match
static inline int getNoteIndex(uint8_t reg1, uint8_t reg2) {
  for (int i = 0; i < EWI_NOTERANGE; i++) {
    if (reg1 == register1Values[i] && reg2 == register2Values[i]) {
      return i;
    }
  }
  return -1;
}

// Returns the MIDI note number for given register values, or -1 if no match
static inline int getMIDINoteForRegisters(uint8_t reg1, uint8_t reg2) {
  int idx = getNoteIndex(reg1, reg2);
  if (idx == -1) return -1;
  return notePitches[idx];
}

#endif