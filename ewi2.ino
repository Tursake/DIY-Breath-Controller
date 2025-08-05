#include <Wire.h>
#include <Seeed_CY8C401XX.h>
#include "MIDIUSB.h"
#include "PitchToNote.h"
#include "RegisterToNote.h"
#include "NoteMapping.h"
#include "MIDIHelpers.h"
#include "Config.h"

// Shared state
float reading;
float intensity = 0;
float intensityLast = 0;
int currentNote = -1;

// Touch slider vars
int sliderValue = 0;
int centerPoint = -1;
int bendValue = 8192;
int bendValueLast = 8192;

float smoothedIntensity = 0;

//DEBUG VALUES
byte lastIncoming1 = 0;
byte lastIncoming2 = 0;
//

void setup() {
  Wire.begin();
  Serial.begin(9600);
  Serial.println("Setup complete");

  pinMode(PSI_PIN, INPUT);

  pinMode(LOAD1, OUTPUT);
  pinMode(CLOCK_ENABLE_PIN1, OUTPUT);
  pinMode(DATA_IN1, INPUT);

  pinMode(LOAD2, OUTPUT);
  pinMode(CLOCK_ENABLE_PIN2, OUTPUT);
  pinMode(DATA_IN2, INPUT);

  pinMode(CLOCK_IN, OUTPUT);
}

void loop() {
  // --- SHIFT REGISTER INPUT ---
  pinPulse();
  digitalWrite(CLOCK_IN, HIGH);

  digitalWrite(CLOCK_ENABLE_PIN1, LOW);
  byte incoming1 = shiftIn(DATA_IN1, CLOCK_IN, LSBFIRST);
  digitalWrite(CLOCK_ENABLE_PIN1, HIGH);

  digitalWrite(CLOCK_ENABLE_PIN2, LOW);
  byte incoming2 = shiftIn(DATA_IN2, CLOCK_IN, LSBFIRST);
  digitalWrite(CLOCK_ENABLE_PIN2, HIGH);

  // --- BREATH SENSOR ---
  reading = analogRead(PSI_PIN);
  float rawIntensity = map(reading, PSI_AMBIENT_RAW, PSI_MAX_RAW, 0, 127);
  rawIntensity = constrain(rawIntensity, 0, 127);

  // Apply exponential smoothing
  const float alpha = 0.2; // Adjust between 0.1 (smooth) and 0.3 (fast)
  smoothedIntensity = alpha * rawIntensity + (1.0 - alpha) * smoothedIntensity;

  if (currentNote != -1 &&
      smoothedIntensity > BREATH_THRESHOLD &&
      abs((int)smoothedIntensity - (int)intensityLast) >= 1) {
    
    controlChange(MIDI_CHANNEL - 1, MIDI_BREATH_CC, (byte)smoothedIntensity);
    MidiUSB.flush();
    intensityLast = smoothedIntensity;
  }


  // --- TOUCH SLIDER PITCH BEND ---
  Wire.requestFrom((uint8_t)TOUCH_SENSOR_ADDR, (uint8_t)4);
  if (Wire.available() >= 2) {
    Wire.read(); // Skip button byte
    sliderValue = Wire.read(); // Get slider (0–97)
  }

  if (sliderValue < 1) {
    centerPoint = -1;
    bendValue = 8192;
  } else {
    if (centerPoint == -1) centerPoint = sliderValue;
    int offset = sliderValue - centerPoint;
    bendValue = constrain(8192 + offset * 64, 0, 16383);
  }

  if (currentNote != -1 && bendValue != bendValueLast) {
    Serial.print("Sending Pitch Bend: ");
    Serial.println(bendValue);
    pitchBend(MIDI_CHANNEL - 1, bendValue);
    MidiUSB.flush();
    bendValueLast = bendValue;
  }

  // --- NOTE HANDLING ---
  int newNote = getNoteIndex(incoming1, incoming2);
  bool validNote = (newNote != -1) && (smoothedIntensity > BREATH_THRESHOLD);

  if (incoming1 != lastIncoming1 || incoming2 != lastIncoming2) {
    Serial.print("Incoming1: "); Serial.print(incoming1); Serial.print(" ("); Serial.print(incoming1, BIN); Serial.print(") ");
    Serial.print("Incoming2: "); Serial.print(incoming2); Serial.print(" ("); Serial.print(incoming2, BIN); Serial.print(")\n");

    int idx = getNoteIndex(incoming1, incoming2);
    Serial.print("Index match: "); Serial.println(idx);

    lastIncoming1 = incoming1;
    lastIncoming2 = incoming2;
  }

  if (validNote) {
    if (newNote != currentNote) {
      if (currentNote != -1) {
        Serial.print("Note Off: "); Serial.println(notePitches[currentNote]);
        noteOff(MIDI_CHANNEL - 1, notePitches[currentNote], 0);
        MidiUSB.flush();
      }
      Serial.print("Note On: "); Serial.println(notePitches[newNote]);
      noteOn(MIDI_CHANNEL - 1, notePitches[newNote], 64);
      MidiUSB.flush();
      currentNote = newNote;
    }
  } else if (currentNote != -1) {
    Serial.print("Note Off (no valid note): ");
    Serial.println(notePitches[currentNote]);
    noteOff(MIDI_CHANNEL - 1, notePitches[currentNote], 0);
    MidiUSB.flush();
    currentNote = -1;
  }

  delay(5);
}

// Shift register load pulse
void pinPulse() {
  digitalWrite(LOAD1, LOW);
  digitalWrite(LOAD2, LOW);
  delayMicroseconds(5);
  digitalWrite(LOAD1, HIGH);
  digitalWrite(LOAD2, HIGH);
  delayMicroseconds(5);
}
