/* This software is based on the openQCM firmware for Arduino Micro http://openqcm.com/software
*/
// Include libraries
// frequency counting
#include <FreqCount.h>
// EERPOM library
#include <EEPROM.h>
#include <Wire.h>
// include PROGMEM library (for storing fixed values inthe flash memory of Arduino)
// Useful for the expected data from the actual drug (once tested)
#include <avr/pgmspace.h>
// GLOBAL VARIABLES
// fixed "gate interval" time for counting cycles (ms)
#define GATE   500
// total time of the test (ms)
#define UPPER_TIME 30000
// total number of reads to consider
#define LEN UPPER_TIME / GATE
// current address in EEPROM series
#define ADDRESS_SERIES 0
// current address in EEPROM first number
#define ADDRESS_NUMBERFIRST 1
// current address in EEPROM second number
#define ADDRESS_NUMBERSECOND 2
// initialise array for storing frequency reads
long freqs[LEN] = {};
// initial index for array
int index = 0;
void setup() {
  Serial.begin(115200);
  // init the frequency counter
  FreqCount.begin(GATE);
}
void loop() {
  // read the openQCM serial number at the connection
  if (Serial.available()) {
    int val = Serial.parseInt();
    if (val == 1) {
      byte valueSeries = EEPROM.read(ADDRESS_SERIES);
      byte valueNumberFirst = EEPROM.read(ADDRESS_NUMBERFIRST);
      byte valueNumberSecond = EEPROM.read(ADDRESS_NUMBERSECOND);
      Serial.print("SERIALNUMBER");
      Serial.print(valueSeries, DEC);
      Serial.print(valueNumberFirst, DEC);
      Serial.print(valueNumberSecond, DEC);
      Serial.write(255);
    }
  }
  // read quartz crystal microbalance frequency and temperature
  if (FreqCount.available())
  {
    freqs[index] = FreqCount.read();
    Serial.print(freqs[index]);
    Serial.print('\n');
    index++;
  }
}
