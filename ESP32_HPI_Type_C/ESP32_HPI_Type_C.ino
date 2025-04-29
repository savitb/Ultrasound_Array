#include <Wire.h>

#define I2C_SDA 21
#define I2C_SCL 22

// CYPD3177 HPI I2C Address 
// default 0x08, shifted left by 1 bit for write/read
#define CYPD3177_HPI_ADDR 0x08

// HPI Register Addresses
#define REG_SYSTEM_STATE         0x04
#define REG_TYPEC_STATE          0x05
#define REG_PD_STATUS            0x06
#define REG_GET_SRC_CAP_CMD      0x0E
#define REG_SRC_CAP_START        0x20
#define REG_SRC_CAP_COUNT        0x1F

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL, 400000);
  Serial.println("ESP32 --> CYPD3177 HPI started");
  delay(1000);

  //Read System State
  uint8_t systemState = hpiRead8(REG_SYSTEM_STATE);
  Serial.print("System State: 0x");
  Serial.println(systemState, HEX);

  //Read Type-C State
  uint8_t typecState = hpiRead8(REG_TYPEC_STATE);
  Serial.print("Type-C State: 0x");
  Serial.println(typecState, HEX);

  /*
  //Request Source Capabilities
  hpiWrite8(REG_GET_SRC_CAP_CMD, 0x01); // Send "Get Source Capabilities" command
  delay(100); // Give time for response
  
  // Read how many Source Capabilities are available
  uint8_t srcCapCount = hpiRead8(REG_SRC_CAP_COUNT);
  Serial.print("Source Capabilities Count: ");
  Serial.println(srcCapCount);

  // Read all advertised Source PDOs
  for (uint8_t i = 0; i < srcCapCount; i++) {
    uint32_t pdo = hpiRead32(REG_SRC_CAP_START + i * 4);
    Serial.print("Source PDO ");
    Serial.print(i);
    Serial.print(": 0x");
    Serial.println(pdo, HEX);
  }
  */
}

void loop() {
  //Periodically read Type-C state
  uint8_t typecState = hpiRead8(REG_TYPEC_STATE);
  Serial.print("Type-C State: 0x");
  Serial.println(typecState, HEX);
  
  delay(5000);
}

// HPI Read 8-bit
uint8_t hpiRead8(uint8_t reg) {
  Wire.beginTransmission(CYPD3177_HPI_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false); // Repeated start
  
  Wire.requestFrom(CYPD3177_HPI_ADDR, (uint8_t)1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF; // Error 
}

// HPI Write 8-bit
void hpiWrite8(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(CYPD3177_HPI_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

// HPI Read 32-bit (for PDOs)
uint32_t hpiRead32(uint8_t reg) {
  Wire.beginTransmission(CYPD3177_HPI_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false); // Repeated start
  
  Wire.requestFrom(CYPD3177_HPI_ADDR, (uint8_t)4);
  uint32_t val = 0;
  if (Wire.available() == 4) {
    val |= (uint32_t)Wire.read();
    val |= (uint32_t)Wire.read() << 8;
    val |= (uint32_t)Wire.read() << 16;
    val |= (uint32_t)Wire.read() << 24;
  }
  return val;
}
