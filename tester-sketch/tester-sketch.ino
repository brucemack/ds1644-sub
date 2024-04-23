/**
 * Copyright (C) 2024, Bruce MacKinnon KC1FSZ
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * FOR AMATEUR RADIO USE ONLY.
 * NOT FOR COMMERCIAL USE WITHOUT PERMISSION.
 */
#define PIN_DATA_OUT (2)
#define PIN_DATA_IN (3)
#define PIN_LATCH_IN (4)
#define PIN_SHIFT_LOAD (5)
#define PIN_CLOCK (6)
#define PIN_LATCH_OUT (7)
#define PIN_LED (13)

// TEMP: DUE TO HW BUG
#define PIN_MEM_OE (8)
#define PIN_MEM_WE (9)

/*
 * Shifts 32 bits into the control, address (2), and data registers.
 */
void writeRaw(uint32_t v) {
  // Shift each bit into the ring
  for (int i = 0; i < 32; i++) {  
    // Setup a data bit
    digitalWrite(PIN_DATA_IN, (v & 0x80000000) ? 1 : 0);
    // Shift clock
    digitalWrite(PIN_CLOCK, 1);
    digitalWrite(PIN_CLOCK, 0);
    // Rotate 
    v = v << 1;

    // TEMP (DUE TO HARDWARE BUG)
    // Latch out
    digitalWrite(PIN_LATCH_OUT, 1);
    digitalWrite(PIN_LATCH_OUT, 0);   
  }
  // Latch out
  //digitalWrite(PIN_LATCH_OUT, 1);
  //digitalWrite(PIN_LATCH_OUT, 0);   
}

uint8_t readRaw() {
  // Load parallel data into latch
  digitalWrite(PIN_LATCH_IN, 1);
  digitalWrite(PIN_LATCH_IN, 0);     
  // Transfer latch data to shift register
  digitalWrite(PIN_SHIFT_LOAD, 0);
  // Prepare to start shifting
  digitalWrite(PIN_SHIFT_LOAD, 1);
  uint8_t result = 0;
  // Shift each bit from the ring
  for (int i = 0; i < 8; i++) {
    // Rotate
    result = result << 1;  
    if (digitalRead(PIN_DATA_OUT) == HIGH)
      result |= 1;
    // Shift clock
    digitalWrite(PIN_CLOCK, 1);
    digitalWrite(PIN_CLOCK, 0);
  }
  return result;
}

void writeMem(uint16_t addr, uint8_t data) {
  // OE=0
  uint32_t ring = ((uint32_t)addr << 8) | ((uint32_t)data << 24);
  writeRaw(ring);
  // Strobe ~WE
  digitalWrite(PIN_MEM_WE, 0);
  digitalWrite(PIN_MEM_WE, 1);  
}

uint8_t readMem(uint16_t addr) {
  // OE=1 to prevent the Data Out register from interfering
  uint32_t ring = ((uint32_t)addr << 8) | (0b10000000);
  writeRaw(ring);
  // Enable memory output
  digitalWrite(PIN_MEM_OE, 0);
  delay(1);
  // Read back
  uint8_t r = readRaw();
  // Disable memory output
  digitalWrite(PIN_MEM_OE, 1);  
  return r;
}

void setup() {

  // Console
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_DATA_OUT, INPUT);
  pinMode(PIN_DATA_IN, OUTPUT);
  pinMode(PIN_LATCH_IN, OUTPUT);
  digitalWrite(PIN_LATCH_IN, 0);
  pinMode(PIN_SHIFT_LOAD, OUTPUT);
  digitalWrite(PIN_SHIFT_LOAD, 1);
  pinMode(PIN_CLOCK, OUTPUT);
  digitalWrite(PIN_CLOCK, 0);
  pinMode(PIN_LATCH_OUT, OUTPUT);
  digitalWrite(PIN_LATCH_OUT, 0);

  // TEMP: WILL BE MOVED TO CONTROL REGISTER
  pinMode(PIN_MEM_OE, OUTPUT);
  digitalWrite(PIN_MEM_OE, 1);
  pinMode(PIN_MEM_WE, OUTPUT);
  digitalWrite(PIN_MEM_WE, 1);
  // TEMP: WILL BE MOVED TO CONTROL REGISTER

  // Diag
  digitalWrite(PIN_LED, 1);
  delay(500);
  digitalWrite(PIN_LED, 0);
  delay(500);
  digitalWrite(PIN_LED, 1);

  Serial.println("Memory Tester V0");

  uint16_t space = 2048;
  Serial.print("Checking ");
  Serial.print(space);
  Serial.println(" locations ...");
  
  bool failed = false;
  for (uint16_t a = 0; a < space; a++) {
    writeMem(a, 0x55);
    uint8_t t0 = readMem(a);
    writeMem(a, 0xaa);
    uint8_t t1 = readMem(a);
    if (t0 != 0x55 || t1 != 0xaa) {
      Serial.print("Bad location: ");
      Serial.println(a);
      failed = true;
      break;  
    }
  }
  for (uint16_t a = 0; a < space; a++)
    writeMem(a, a);
  for (uint16_t a = 0; a < space; a++)
    if (readMem(a) != (a & 0xff)) {
      Serial.print("Bad location: ");
      Serial.println(a);
      failed = true;
      break;        
    } 
  if (!failed) {
    Serial.println("No faults detected");
  }
}

void loop() {
  /*  
  // Write a C0=1, disable OE
  writeRaw(0b110000000);
  // Read back
  uint32_t r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
  
  // Write a C0=0, disable OE
  writeRaw(0b010000000);
  // Read back
  r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
  */
  /*
  // Internal self-test
  // Show that we can latch data out and read it back
  // Write DO=0x55, enable OE
  writeRaw(0x00ff0000);
  // Read back
  uint32_t r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
  
  // Write DO=0xaa, enable OE
  writeRaw(0x00000000);
  // Read back
  r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
  */
}
