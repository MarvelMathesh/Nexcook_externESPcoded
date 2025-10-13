#include <Wire.h>

#define RELAY_PIN     17  // Control pin connected to BC547 base via 1k resistor
#define I2C_SDA_PIN   13  // I2C SDA pin
#define I2C_SCL_PIN   14  // I2C SCL pin
#define SLAVE_ADDR    0x28

volatile bool messageReceived = false;
String receivedMessage = "";

void onI2CReceive(int numBytes) {
  receivedMessage = "";
  while (Wire.available()) {
    char c = Wire.read();
    receivedMessage += c;
  }
  messageReceived = true;
}

void setup() {
  // Initialize relay
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);  // Ensure Peltier is OFF initially

  // Initialize Serial
  Serial.begin(115200);
  delay(100);  // Small delay to ensure Serial is ready
  Serial.println("ESP32-S3 I2C Slave + Relay Control Started");

  // Initialize I2C in Slave Mode
  Wire.begin((uint8_t)SLAVE_ADDR, I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.onReceive(onI2CReceive);
}

void loop() {
  // Check if any I2C message was received
  if (messageReceived) {
    Serial.print("Received via I2C: ");
    Serial.println(receivedMessage);
    messageReceived = false;
  }

  // Turn ON Peltier
  digitalWrite(RELAY_PIN, HIGH);
  Serial.println("Peltier ON for 3 minutes...");

  delay(180000);  // 3 minutes

  // Turn OFF Peltier
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("Peltier OFF now.");

  while (true) {
    // Optional: halt further execution
    delay(1000);
  }
}
