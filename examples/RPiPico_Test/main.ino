/*
  @brief Test an RPi Pico with EEPROM set up for the T-RECS.
  @author Joel Dunham <joel.dunham@tangiblesthatteach.com>
  @date 2021/08/11

  Raspberry Pi Pico
  GPIO 14 = SDA
  GPIO 15 = SCL
  GND to GND
  3.3V to 3.3V
  
  Load this sketch
  Open output window at 115200 baud rate
*/

#include <Wire.h>

#define EEPROM_ADDR 0b1010000
#define EEPROM_SDA 14
#define EEPROM_SCL 15
#define EEPROM_SIZE 256  // For the RPi Pico
TwoWire __eepromWire(EEPROM_SDA, EEPROM_SCL);

uint64_t last_write = 0L;

#include "SparkFun_External_EEPROM.h" // Click here to get the library: http://librarymanager/All#SparkFun_External_EEPROM
ExternalEEPROM myMem;

void setup() {
  Serial.begin(115200);
  delay(10000);  // Time to open the serial monitor
  Serial.println("I2C EEPROM test for RPi Pico");

  pinMode(EEPROM_SDA, INPUT);
  pinMode(EEPROM_SCL, INPUT);
  if((digitalRead(EEPROM_SDA) == HIGH) && (digitalRead(EEPROM_SCL) == HIGH)) {
    Serial.println("SDA and SCL pulled high - the EEPROM board is attached");
    // Good to continue - pullup resistors in place, so something is attached
    // RP2040 (Raspberry Pi Pico) requires these calls
    __eepromWire.begin();
    __eepromWire.setClock(400000);
    Serial.println("EEPROM wire connection started");
    if(myMem.begin(EEPROM_ADDR, __eepromWire) == false) {
      Serial.println("Failed EEPROM init");
    } else {
      Serial.println("EEPROM initialized");
      myMem.setMemorySize(EEPROM_SIZE * 1000 / 8);  // In Bytes, not kilobits
      Serial.println("EEPROM size set");

      Serial.println("Running write test");
      float myValue3 = -7.35;
      myMem.put(20, myValue3); //(location, data)
      float myRead3;
      myMem.get(20, myRead3); //location to read, thing to put data into
      Serial.print("I read: ");
      Serial.println(myRead3);
    }
  } else {
    // The begin call will freeze - there is nothing connected and no EEPROM
    Serial.println("SDA and SCL are not pulled high - no EEPROM board");
  }  

  Serial.println("Test completed!");
}

void loop() {
  uint64_t current_time = millis();
  if(current_time > (last_write + 1000)) {
    Serial.println("Test completed...restart the board to run again");
    last_write = current_time;
  }
}
