#ifndef EWI_CONFIG_H
#define EWI_CONFIG_H

#include <Arduino.h>

// ==== NOTE RANGE ====
constexpr uint8_t EWI_NOTERANGE = 18;

// ==== BREATH SENSOR ====
constexpr uint8_t PSI_PIN           = 1;
constexpr int PSI_AMBIENT_RAW       = 370;
constexpr int PSI_MAX_RAW           = 420;
constexpr uint8_t BREATH_THRESHOLD  = 10;

// ==== MIDI SETTINGS ====
constexpr uint8_t MIDI_CHANNEL      = 1;
constexpr uint8_t MIDI_BREATH_CC    = 11;

// ==== TOUCH SENSOR ====
constexpr uint8_t TOUCH_SENSOR_ADDR = 0x08;

// ==== TOUCH BUTTONS ====
constexpr uint8_t EWI_BUTTONS = 13;
constexpr uint8_t MPR121_1 = 0x5A;
constexpr uint8_t MPR121_2 = 0x5B;
constexpr uint8_t TOUCH_BYTES = 2;
constexpr int MPR121_irqpin_1 = 8;
constexpr int MPR121_irqpin_2 = 7;

constexpr int portamentoPin = 14;
//Buttons 15-22 for octave

#endif
