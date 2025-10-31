#include "Module.h"
#include "HX711.h"
#include "globals.h"

// UART Communication with Slave ESP32
#define SLAVE_RX_PIN 19
#define SLAVE_TX_PIN 20
#define MAX_BUFFER_SIZE 256

HX711 scale;
float calibration_factor = -7050;

SpiceDispenser spicedis;
Hopper hopper;
Grind grind;
Chop chop;
Heat heat;
LiquidDispenser liqdis;
Steamer steam;
Stirrer stirrer;
Cleaning clean;

String inputBuffer = "";

void setup() {
  // Initialize serial communications
  Serial.begin(115200);   // Debug
  Serial1.begin(115200);  // Display
  Serial2.begin(115200, SERIAL_8N1, SLAVE_RX_PIN, SLAVE_TX_PIN); // Slave ESP

  pinMode(STIRRER_ENA, OUTPUT);
  pinMode(STIRRER_IN1, OUTPUT);
  pinMode(STIRRER_IN2, OUTPUT);

  // Configure spice dispenser motor pins
  pinMode(POSITION_ENA, OUTPUT);
  pinMode(POSITION_IN1, OUTPUT);
  pinMode(POSITION_IN2, OUTPUT);
  pinMode(RACK_ENB, OUTPUT);
  pinMode(RACK_IN3, OUTPUT);
  pinMode(RACK_IN4, OUTPUT);
  pinMode(DISPENSE_ENA, OUTPUT);
  pinMode(DISPENSE_IN1, OUTPUT);
  pinMode(DISPENSE_IN2, OUTPUT);
  pinMode(ACTUATOR_ENB, OUTPUT);
  pinMode(ACTUATOR_IN3, OUTPUT);
  pinMode(ACTUATOR_IN4, OUTPUT);

  // Initialize load cell and tare
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor);
  scale.tare();
  
  // Ensure all motors stopped initially
  spicedis.stopPositionMotor();
  spicedis.stopRackMotor();
  spicedis.stopDispenseMotor();
  spicedis.stopActuatorMotor();
}

void loop() {
  // Read commands from display
  while (Serial1.available() > 0) {
    char c = Serial1.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();
        if (inputBuffer.length() > 0) {
          Serial.println("Received: " + inputBuffer);
          if (inputBuffer.startsWith("MODULE:")) {
            parseAndExecute(inputBuffer);
          } else {
            Serial.println("Invalid command format: " + inputBuffer);
          }
        }
        inputBuffer = "";
      }
    } else {
      if (inputBuffer.length() < MAX_BUFFER_SIZE) {
        inputBuffer += c;
      } else {
        Serial.println("Buffer overflow! Clearing buffer.");
        inputBuffer = "";
      }
    }
  }
}

void parseAndExecute(String command) {
  if (command.startsWith("MODULE:"))
    command = command.substring(7);

  command.trim();
  Serial.println("Processing command: " + command);

  // Parse command format: name|id|value
  String parts[3];
  int seg = 0, start = 0;
  while (seg < 3) {
    int bar = command.indexOf('|', start);
    if (bar == -1) {
      parts[seg++] = command.substring(start);
      break;
    }
    parts[seg++] = command.substring(start, bar);
    start = bar + 1;
  }
  while (seg < 3)
    parts[seg++] = "";

  String name = parts[0];
  int id = parts[1].length() ? parts[1].toInt() : -1;
  int value = parts[2].length() ? parts[2].toInt() : -1;

  Serial.println("Name: " + name + ", ID: " + String(id) + ", Value: " + String(value));

  // Forward liquid commands to slave
  if (name == "liquid") {
    String slaveCommand = "MODULE:" + command;
    Serial2.println(slaveCommand + "\n");
    Serial2.flush();
    Serial.println("Status: liquid Done");
  // Handle spice dispensing locally
  } else if (name == "spice") {
    if (id >= 0)
      spicedis.id = id;
    if (value >= 0)
      spicedis.weight = value;
    spicedis.start();
    Serial.println("Status: spice Done");
  // Forward hopper commands to slave
  } else if (name == "hopper") {
    if (id >= 0)
      hopper.id = id;
    if (value >= 0)
      hopper.weight = value;
    String slaveCommand = "MODULE:" + command;
    Serial2.println(slaveCommand + "\n");
    Serial2.flush();
    Serial.println("Status: hopper Done");
  } else if (name == "clean") {
    Serial.println("Status: clean Done");
  } else if (name == "stirrer") {
    delay(5000);
    Serial.println("Status: stirrer Done");
  } else if (name == "chop") {
    Serial.println("Chopping...");
    delay(5000);
    Serial.println("Status: chop Done");
  } else if (name == "steam") {
    Serial.println("Steaming...");
    delay(5000);
    Serial.println("Status: steam Done");
  } else {
    Serial.println("Unknown Module: " + name);
    Serial.println("Status: Error");
  }
}
