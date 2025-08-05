#ifndef MIDIHELPERS_H
#define MIDIHELPERS_H

#include <stdint.h>
#include "MIDIUSB.h"

// Send a Note On message
static inline void noteOn(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  midiEventPacket_t noteOnMsg = {0x09, (uint8_t)(0x90 | ((channel - 1) & 0x0F)), pitch, velocity};
  MidiUSB.sendMIDI(noteOnMsg);
}

// Send a Note Off message
static inline void noteOff(uint8_t channel, uint8_t pitch, uint8_t velocity) {
  midiEventPacket_t noteOffMsg = {0x08, (uint8_t)(0x80 | ((channel - 1) & 0x0F)), pitch, velocity};
  MidiUSB.sendMIDI(noteOffMsg);
}

// Send a Control Change message
static inline void controlChange(uint8_t channel, uint8_t control, uint8_t value) {
  midiEventPacket_t ccMsg = {0x0B, (uint8_t)(0xB0 | ((channel - 1) & 0x0F)), control, value};
  MidiUSB.sendMIDI(ccMsg);
}

// Send a Pitch Bend message (14-bit value: 0–16383, center is 8192)
static inline void pitchBend(uint8_t channel, int value) {
  value = constrain(value, 0, 16383); // Ensure it's within 14-bit bounds
  uint8_t lsb = value & 0x7F;
  uint8_t msb = (value >> 7) & 0x7F;
  midiEventPacket_t pbMsg = {0x0E, (uint8_t)(0xE0 | ((channel - 1) & 0x0F)), lsb, msb};
  MidiUSB.sendMIDI(pbMsg);
}

// Optional: helper to flush MIDI messages
static inline void sendAllPendingMIDI() {
  MidiUSB.flush();
}

#endif
