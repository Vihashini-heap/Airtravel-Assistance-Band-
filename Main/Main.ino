#include <Wire.h>

#define DRV_ADDR 0x5A

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(DRV_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(8, 9);
  Wire.setClock(100000);

  delay(3000);

  Serial.println("Triggering vibration...");

  writeReg(0x01, 0x00); // internal trigger
  writeReg(0x03, 0x01); // ERM
  writeReg(0x04, 47);   // effect
  writeReg(0x05,0);
  writeReg(0x0C, 1); 
  delay(50);   // GO
}

void loop() {}