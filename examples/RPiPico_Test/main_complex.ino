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
#include <Utilities/utils.h>

#define EEPROM_ADDR 0b1010000
#define EEPROM_SDA 14
#define EEPROM_SCL 15
#define EEPROM_SIZE 256  // For the RPi Pico
TwoWire __eepromWire(EEPROM_SDA, EEPROM_SCL);

uint64_t last_write = 0L;
uint64_t last_blink = 0L;
float counter = 1.0;
bool ledLOW = false;

const uint8_t CONFIGURED_VALUE{23};  // If the EEPROM is configured, set this so we know it's configured
const uint8_t numParams = 5;
const uint8_t numSensors = 1;
dataStoreDef mydatastore;
parameterDataStoreDef paramDatas[numParams];
sensorDataStoreDef sensorDatas[numSensors];
float eepromdata[5]{0.0, 0.0, 0.0, 0.0, 0.0};
const uint8_t TYPE_OFFSET_TEST = 0;  // For system ID data

bool initialized = false;
bool hasEEPROM = true;

void setup() {
  mydatastore.isConfigured = 0;

  Serial.begin(115200);
  delay(10000);  // Time to open the serial monitor
  Serial.println("I2C EEPROM test for RPi Pico");
}

void loop() {
  if(!initialized) {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(EEPROM_SDA, INPUT);
    pinMode(EEPROM_SCL, INPUT);
    if((digitalRead(EEPROM_SDA) == HIGH) && (digitalRead(EEPROM_SCL) == HIGH)) {
      Serial.println("SDA and SCL pulled high - the EEPROM board is attached");
      // Good to continue - pullup resistors in place, so something is attached
      // RP2040 (Raspberry Pi Pico) requires these calls
      __eepromWire.begin();
      __eepromWire.setClock(400000);
      Serial.println("EEPROM wire connection started");
      if(EEPROM.begin(EEPROM_ADDR, __eepromWire) == false) {
        Serial.println("Failed EEPROM init");
        hasEEPROM = false;
      } else {
        Serial.println("EEPROM initialized");
        EEPROM.setMemorySize(EEPROM_SIZE * 1000 / 8);  // In Bytes, not kilobits
        Serial.println("EEPROM size set");
      }
    } else {
      // The begin call will freeze - there is nothing connected and no EEPROM
      Serial.println("SDA and SCL are not pulled high - no EEPROM board");
      hasEEPROM = false;
    }

    if(hasEEPROM) {
      // Get current info
      EEPROM.get(0, mydatastore);
      if(mydatastore.isConfigured != CONFIGURED_VALUE) {
        // Write the initial versions
        mydatastore.isConfigured = CONFIGURED_VALUE;
        EEPROM.put(0, mydatastore); //(location, data)
        for(uint8_t paramIdx = 0; paramIdx < numParams; paramIdx++) {
          EEPROM.put(sizeof(dataStoreDef) + (paramIdx * sizeof(parameterDataStoreDef)), paramDatas[paramIdx]); //(location, data)
        }
        for(uint8_t senIdx = 0; senIdx < numParams; senIdx++) {
          EEPROM.put(sizeof(dataStoreDef) + (numParams * sizeof(parameterDataStoreDef)) +
            (senIdx * sizeof(sensorDataStoreDef)), sensorDatas[senIdx]); //(location, data)
        }
      } else {
        for(uint8_t parameterId = 0; parameterId < numParams; parameterId++) {
          EEPROM.get(sizeof(dataStoreDef) + (parameterId * sizeof(parameterDataStoreDef)), paramDatas[parameterId]);
        }
        // Read saved sensor values
        for(uint8_t sensorId = 0; sensorId < numSensors; sensorId++) {
          EEPROM.get(sizeof(dataStoreDef) + (numParams * sizeof(parameterDataStoreDef)) +
            (sensorId * sizeof(sensorDataStoreDef)), sensorDatas[sensorId]);
        }
      }
    }

    initialized = true;
    Serial.println("Test started");
  } else if (hasEEPROM) {
    uint64_t current_time = millis();
    if(current_time > (last_write + 250)) {
      Serial.println("Running EEPROM test");
      last_write = current_time;

      Serial.println("Running read test");

      uint16_t typeOffset = 0;
      if(TYPE_OFFSET_TEST == 0) {
          // For system ID
          typeOffset = sizeof(dataStoreDef) + (numParams * sizeof(parameterDataStoreDef)) +
              (MAX_SENSOR_VALUES * sizeof(sensorDataStoreDef));
      }
      uint8_t *bufPtr = (uint8_t*)eepromdata;
      for(uint16_t msg_idx = 0; msg_idx < 20; msg_idx++) {
        uint8_t readVal = EEPROM.read(msg_idx + typeOffset + 0);
        bufPtr[msg_idx] = readVal;
      }
      Serial.print("I read: ");
      for(uint8_t floatIdx = 0; floatIdx < 5; floatIdx++) {
        Serial.print(eepromdata[floatIdx]);
        Serial.print(", ");
      }
      Serial.println(".");
    }

    if(current_time > (last_blink + 1000)) {
      ledLOW = !ledLOW;
      digitalWrite(LED_BUILTIN, (ledLOW) ? LOW : HIGH);
      last_blink = current_time;
    }
  }
}
