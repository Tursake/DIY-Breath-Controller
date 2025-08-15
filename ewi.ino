#include <Wire.h>
#include <Seeed_CY8C401XX.h>
#include "MIDIUSB.h"
#include "PitchToNote.h"
#include "RegisterToNote.h"
#include "NoteMapping.h"
#include "MIDIHelpers.h"
#include "Config.h"
#include "mpr121.h"

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

  pinMode(MPR121_irqpin_1, INPUT_PULLUP);
  pinMode(MPR121_irqpin_2, INPUT_PULLUP);
  pinMode(PSI_PIN, INPUT);

  Wire.begin();
  Serial.begin(9600);
  Serial.println("Setup complete");

  mpr121_setup(MPR121_1);
  mpr121_setup(MPR121_2);

}

void loop() {

  readTouchInputs();

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
int newNote = getNoteIndexForRegisters(touchStates);
bool validNote = (newNote != -1 && smoothedIntensity > BREATH_THRESHOLD);

if (currentNote == -1 && validNote) {
    noteOn(MIDI_CHANNEL - 1, notePitches[newNote], smoothedIntensity);
    MidiUSB.flush();
    currentNote = newNote;
    Serial.print("Note On: "); Serial.println(currentNote);

        Serial.print("newNote index: ");
    Serial.print(newNote);
    Serial.print("  MIDI pitch: ");
    Serial.println(notePitches[newNote]);
}
else if (currentNote != -1) {
    if (!validNote) {
        // Stop the current note
        noteOff(MIDI_CHANNEL - 1, notePitches[currentNote], 0);
        MidiUSB.flush();
        currentNote = -1;
        Serial.println("Note Off (no valid note)");
    } else if (newNote != currentNote) {
        // Change to a different note
        noteOff(MIDI_CHANNEL - 1, notePitches[currentNote], 0);
        noteOn(MIDI_CHANNEL - 1, notePitches[newNote], smoothedIntensity);
        MidiUSB.flush();
        currentNote = newNote;
        Serial.print("Note On: "); Serial.println(newNote);
    }
}


  delay(5);
}
