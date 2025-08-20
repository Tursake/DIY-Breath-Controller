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
float intensityLast = 0;
int currentNote = -1;

// Touch slider vars
int sliderValue = 0;
int centerPoint = -1;
int bendValue = 8192;
int bendValueLast = 8192;

float smoothedIntensity = 0;
int currentOctave = 0;
const byte PORTAMENTO_CC = 5;
bool portamentoLast = false;

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
    // --- Touch & Octave updates ---
    readTouchInputs();
    updateOctave(touchStates, currentOctave);

    static int lastOctave = currentOctave; // track previous octave
    bool portamentoHeld = touchStates[portamentoPin];

    // --- GLIDE SPEED via Potentiometer on A4 ---
    static int lastGlideCC = -1;
    int potVal = analogRead(A4);               // 0–1023
    float normalized = potVal / 1023.0;       // 0.0 – 1.0

    // Logarithmic-style scaling: small changes at low end are more noticeable
    float logScale = log10(1 + 9 * normalized); // log10(1..10) => 0..1
    byte glideCC = (byte)(logScale * 127);     // map to 0–127 MIDI

    if (glideCC != lastGlideCC) {             // only send if changed
        controlChange(MIDI_CHANNEL - 1, 65, glideCC); // CC65 = glide speed
        MidiUSB.flush();
        lastGlideCC = glideCC;
    }

    // --- PORTAMENTO BUTTON CC5 ---
    if (portamentoHeld != portamentoLast) {
        byte cc5Value = portamentoHeld ? 127 : 0; // full on/off
        controlChange(MIDI_CHANNEL - 1, 5, cc5Value);
        MidiUSB.flush();
        portamentoLast = portamentoHeld;

        Serial.print("Portamento CC5: ");
        Serial.println(cc5Value);
    }


    // --- BREATH SENSOR ---
    reading = analogRead(PSI_PIN);
    float rawIntensity = map(reading, PSI_AMBIENT_RAW, PSI_MAX_RAW, 0, 127);
    rawIntensity = constrain(rawIntensity, 0, 127);

    // Apply exponential smoothing
    const float alpha = 0.2;
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
        pitchBend(MIDI_CHANNEL - 1, bendValue);
        MidiUSB.flush();
        bendValueLast = bendValue;
    }

    // --- NOTE HANDLING ---
    int newNote = getNoteIndexForRegisters(touchStates);
    bool validNote = (newNote != -1 && smoothedIntensity > BREATH_THRESHOLD);

    // Handle octave changes in real time while holding a note
    if (currentNote != -1 && currentOctave != lastOctave) {
        noteOff(MIDI_CHANNEL - 1, notePitches[currentNote] + lastOctave * 12, 0);
        noteOn(MIDI_CHANNEL - 1, notePitches[currentNote] + currentOctave * 12, smoothedIntensity);
        MidiUSB.flush();

        Serial.print("Octave changed! Note On: ");
        Serial.print(currentNote);
        Serial.print(" | Octave: "); Serial.println(currentOctave);

        lastOctave = currentOctave;
    }

    if (currentNote == -1 && validNote) {
        noteOn(MIDI_CHANNEL - 1, notePitches[newNote] + currentOctave * 12, smoothedIntensity);
        MidiUSB.flush();
        currentNote = newNote;
        lastOctave = currentOctave;

        Serial.print("Note On: "); Serial.print(currentNote);
        Serial.print(" | Octave: "); Serial.println(currentOctave);
    } 
    else if (currentNote != -1) {
        if (!validNote) {
            noteOff(MIDI_CHANNEL - 1, notePitches[currentNote] + currentOctave * 12, 0);
            MidiUSB.flush();
            currentNote = -1;
            Serial.println("Note Off (no valid note)");
        } 
        else if (newNote != currentNote) {
            noteOff(MIDI_CHANNEL - 1, notePitches[currentNote] + currentOctave * 12, 0);
            noteOn(MIDI_CHANNEL - 1, notePitches[newNote] + currentOctave * 12, smoothedIntensity);
            MidiUSB.flush();
            currentNote = newNote;
            lastOctave = currentOctave;

            Serial.print("Note On: "); Serial.print(newNote);
            Serial.print(" | Octave: "); Serial.println(currentOctave);
        }
    }

    delay(5);
}

