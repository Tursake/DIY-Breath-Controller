#ifndef EWI_CONFIG_H
#define EWI_CONFIG_H

#include <Arduino.h>

// ==== NOTE RANGE ====
constexpr uint8_t EWI_NOTERANGE = 18;

// ==== SHIFT REGISTER PINS ====
constexpr uint8_t CLOCK_ENABLE_PIN1 = 4;
constexpr uint8_t DATA_IN1          = 5;
constexpr uint8_t LOAD1             = 7;

constexpr uint8_t CLOCK_ENABLE_PIN2 = 8;
constexpr uint8_t DATA_IN2          = 9;
constexpr uint8_t LOAD2             = 11;

constexpr uint8_t CLOCK_IN          = 6; // Shared clock pin

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

#endif