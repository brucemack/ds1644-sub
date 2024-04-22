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

/*
 * Shifts 32 bits into the control, address (2), and data registers.
 */
void writeRaw(uint32_t v) {
  // Go into shift mode
  digitalWrite(PIN_SHIFT_LOAD, 0);
  // Shift each bit into the ring
  for (int i = 0; i < 32; i++) {  
    // Setup a data bit
    digitalWrite(PIN_DATA_IN, (v & 0x80000000) ? 1 : 0);
    // Shift clock
    digitalWrite(PIN_CLOCK, 1);
    digitalWrite(PIN_CLOCK, 0);
    // Rotate 
    v = v << 1;
  }
  // Latch out
  digitalWrite(PIN_LATCH_OUT, 1);
  digitalWrite(PIN_LATCH_OUT, 0);   
}

uint32_t readRaw() {
  // Go into load mode
  digitalWrite(PIN_SHIFT_LOAD, 1);
  // Load parallel data into latch
  digitalWrite(PIN_LATCH_IN, 1);
  digitalWrite(PIN_LATCH_IN, 0);     
  // Transfer latch data to shift register
  digitalWrite(PIN_SHIFT_LOAD, 0);
  // Prepare to start shifting
  digitalWrite(PIN_SHIFT_LOAD, 1);
  uint32_t result = 0;
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

void setup() {

  // Console
  Serial.begin(9600);

  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_DATA_OUT, INPUT);
  pinMode(PIN_DATA_IN, OUTPUT);
  pinMode(PIN_LATCH_IN, OUTPUT);
  digitalWrite(PIN_LATCH_IN, 0);
  pinMode(PIN_SHIFT_LOAD, OUTPUT);
  digitalWrite(PIN_SHIFT_LOAD, 0);
  pinMode(PIN_CLOCK, OUTPUT);
  digitalWrite(PIN_CLOCK, 0);
  pinMode(PIN_LATCH_OUT, OUTPUT);
  digitalWrite(PIN_LATCH_OUT, 0);

  // Diag
  digitalWrite(PIN_LED, 1);
  delay(500);
  digitalWrite(PIN_LED, 0);
  delay(500);
  digitalWrite(PIN_LED, 1);
  delay(500);
  digitalWrite(PIN_LED, 0);

  Serial.println("Memory Tester V0");
}

void loop() {
  // Write a C0=1, disable OE
  writeRaw(0b10000001);
  // Read back
  uint32_t r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
  
  // Write a C0=0, disable OE
  writeRaw(0b10000000);
  // Read back
  r = readRaw();
  Serial.print(r);
  Serial.println();
  delay(1000);
}
