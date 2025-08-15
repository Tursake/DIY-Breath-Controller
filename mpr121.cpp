#include "mpr121.h"

// Define and initialize the variable
boolean touchStates[24] = {0};

void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

void readTouchInputs() {
  // Only read if either IRQ pin is LOW (active)
  if (digitalRead(MPR121_irqpin_1) == LOW || digitalRead(MPR121_irqpin_2) == LOW) {

    // --- Read MPR121 #1 ---
    Wire.requestFrom(MPR121_1, TOUCH_BYTES);
    while (Wire.available() < 2);  // Wait for 2 bytes
    uint16_t touched1 = Wire.read() | (Wire.read() << 8);

    // --- Read MPR121 #2 ---
    Wire.requestFrom(MPR121_2, TOUCH_BYTES);
    while (Wire.available() < 2);  // Wait for 2 bytes
    uint16_t touched2 = Wire.read() | (Wire.read() << 8);

    // --- Check touch states for both devices ---
    for (int device = 0; device < 2; device++) {
      uint16_t touched = (device == 0) ? touched1 : touched2;

      for (int i = 0; i < 12; i++) {
        int idx = i + device * 12; // index in touchStates array
        if (touched & (1 << i)) {
          /*if (!touchStates[idx]) {
            Serial.print("Device "); Serial.print(device + 1);
            Serial.print(" - pin "); Serial.print(i);
            Serial.println(" was just touched");
          }*/
          touchStates[idx] = 1;
        } else {
          /*if (touchStates[idx]) {
            Serial.print("Device "); Serial.print(device + 1);
            Serial.print(" - pin "); Serial.print(i);
            Serial.println(" is no longer being touched");
          }*/
          touchStates[idx] = 0;
        }
      }
    }
  }
}

void mpr121_setup(uint8_t device){

  set_register(device, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(device, MHD_R, 0x01);
  set_register(device, NHD_R, 0x01);
  set_register(device, NCL_R, 0x00);
  set_register(device, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(device, MHD_F, 0x01);
  set_register(device, NHD_F, 0x01);
  set_register(device, NCL_F, 0xFF);
  set_register(device, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(device, ELE0_T, TOU_THRESH);
  set_register(device, ELE0_R, REL_THRESH);
 
  set_register(device, ELE1_T, TOU_THRESH);
  set_register(device, ELE1_R, REL_THRESH);
  
  set_register(device, ELE2_T, TOU_THRESH);
  set_register(device, ELE2_R, REL_THRESH);
  
  set_register(device, ELE3_T, TOU_THRESH);
  set_register(device, ELE3_R, REL_THRESH);
  
  set_register(device, ELE4_T, TOU_THRESH);
  set_register(device, ELE4_R, REL_THRESH);
  
  set_register(device, ELE5_T, TOU_THRESH);
  set_register(device, ELE5_R, REL_THRESH);
  
  set_register(device, ELE6_T, TOU_THRESH);
  set_register(device, ELE6_R, REL_THRESH);
  
  set_register(device, ELE7_T, TOU_THRESH);
  set_register(device, ELE7_R, REL_THRESH);
  
  set_register(device, ELE8_T, TOU_THRESH);
  set_register(device, ELE8_R, REL_THRESH);
  
  set_register(device, ELE9_T, TOU_THRESH);
  set_register(device, ELE9_R, REL_THRESH);
  
  set_register(device, ELE10_T, TOU_THRESH);
  set_register(device, ELE10_R, REL_THRESH);
  
  set_register(device, ELE11_T, TOU_THRESH);
  set_register(device, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(device, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(device, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(device, ATO_CFG0, 0x0B);
  set_register(device, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(device, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(device, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(device, ELE_CFG, 0x0C);
  
}
