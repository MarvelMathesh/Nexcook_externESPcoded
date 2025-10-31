#include <Arduino.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <Stepper.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// UART Communication
#define UART_RX_PIN 19
#define UART_TX_PIN 20

// Liquid Dispenser
#define PUMP1_PIN 5
#define PUMP2_PIN 6
#define PUMP3_PIN 7
#define PUMP4_PIN 8
#define PUMP5_PIN 9
#define PUMP6_PIN 17
#define PUMP7_PIN 11
#define ONE_WIRE_BUS 12
#define RELAY_PIN 10

// Hopper System
#define SERVO_PIN 13
#define DC_IN1 9
#define DC_IN2 46
#define DC_EN 4
#define GRINDER_IN1 35
#define GRINDER_IN2 36
#define GRINDER_EN 14
#define ACT_IN1 3
#define ACT_IN2 8
#define ACT_ENA 12
#define ACT_SPEED 255

// Stepper motor pin configurations
const int STEPPER1_PINS[4] = {39, 40, 37, 38}; // Spinach
const int STEPPER2_PINS[4] = {17, 18, 16, 15};
const int STEPPER3_PINS[4] = {41, 42, 2, 1};
const int STEPS_PER_REV = 2048;
const long STEPPER1_STEPS = 341;
const long STEPPER2_STEPS = 512;
const long STEPPER3_STEPS = 682;

// Grinder timing configuration
const unsigned long GRINDER_ON_MS = 3000UL;
const unsigned long GRINDER_OFF_MS = 1000UL;
const int GRINDER_PULSE_COUNT = 3;
const unsigned long INTER_DELAY = 2000UL;
const int PUMP_ON_TIME = 5000;
const int PUMP_OFF_DELAY = 1000;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
float temperature = 0.0;

Servo myServo;
Stepper stepper1(STEPS_PER_REV, STEPPER1_PINS[0], STEPPER1_PINS[2], STEPPER1_PINS[1], STEPPER1_PINS[3]);
Stepper stepper2(STEPS_PER_REV, STEPPER2_PINS[0], STEPPER2_PINS[2], STEPPER2_PINS[1], STEPPER2_PINS[3]);
Stepper stepper3(STEPS_PER_REV, STEPPER3_PINS[0], STEPPER3_PINS[2], STEPPER3_PINS[1], STEPPER3_PINS[3]);

String inputBuffer = "";

void setup() {
  // Initialize serial communications
  Serial.begin(115200);
  Serial.println("ESP32-S3 UART Slave Started");
  Serial2.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

  // Configure pump pins
  pinMode(PUMP1_PIN, OUTPUT);
  pinMode(PUMP2_PIN, OUTPUT);
  pinMode(PUMP3_PIN, OUTPUT);
  pinMode(PUMP4_PIN, OUTPUT);
  pinMode(PUMP5_PIN, OUTPUT);
  pinMode(PUMP6_PIN, OUTPUT);
  pinMode(PUMP7_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(PUMP1_PIN, LOW);
  digitalWrite(PUMP2_PIN, LOW);
  digitalWrite(PUMP3_PIN, LOW);
  digitalWrite(PUMP4_PIN, LOW);
  digitalWrite(PUMP5_PIN, LOW);
  digitalWrite(PUMP6_PIN, LOW);
  digitalWrite(PUMP7_PIN, LOW);
  digitalWrite(RELAY_PIN, LOW);

  sensors.begin();

  // Configure stepper motors
  stepper1.setSpeed(10);
  stepper2.setSpeed(10);
  stepper3.setSpeed(10);

  // Initialize servo
  myServo.attach(SERVO_PIN);
  myServo.write(0);

  // Configure DC motor pins
  pinMode(DC_IN1, OUTPUT);
  pinMode(DC_IN2, OUTPUT);
  pinMode(DC_EN, OUTPUT);
  pinMode(GRINDER_IN1, OUTPUT);
  pinMode(GRINDER_IN2, OUTPUT);
  pinMode(GRINDER_EN, OUTPUT);
  pinMode(ACT_IN1, OUTPUT);
  pinMode(ACT_IN2, OUTPUT);
  pinMode(ACT_ENA, OUTPUT);

  stopDC(DC_IN1, DC_IN2, DC_EN);
  stopDC(GRINDER_IN1, GRINDER_IN2, GRINDER_EN);
}

void loop() {
  // Read commands from master ESP
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        inputBuffer.trim();
        if (inputBuffer.length() > 0) {
          Serial.println(inputBuffer);
          if (inputBuffer.startsWith("MODULE:")) {
            parseAndExecute(inputBuffer.substring(7));
          }
        }
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
    }
  }
}

// Monitor and control Peltier cooler
void checkAndControlPeltier() {
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);
  Serial.print("Current Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  if (temperature < 25.5) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println("Peltier OFF (below 20°C)");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println("Peltier ON (above 20°C)");
  }
}

// Grinder sequence with servo release
void grind() {
  Serial.println("Grinder: " + String(GRINDER_PULSE_COUNT) + " pulses of 3s ON / 1s OFF");
  for (int p = 1; p <= GRINDER_PULSE_COUNT; ++p) {
    Serial.printf("Grinder pulse %d: ON\n", p);
    setDCForward(GRINDER_IN1, GRINDER_IN2, GRINDER_EN);
    delay(GRINDER_ON_MS);
    stopDC(GRINDER_IN1, GRINDER_IN2, GRINDER_EN);
    Serial.printf("Grinder pulse %d: OFF\n", p);
    if (p < GRINDER_PULSE_COUNT)
      delay(GRINDER_OFF_MS);
  }
  servoSweepBlocking(0, 90, 2);
  delay(1000);
  servoSweepBlocking(90, 0, 2);
  Serial.println("Servo done.");
  Serial.println("Grinder sequence done.");
}

// Generic motor control with direction
void moveMotor(int in1Pin, int in2Pin, int enaPin, int direction, int speed) {
  if (speed < 0) speed = 0;
  if (speed > 255) speed = 255;

  if (direction == 1) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
    analogWrite(enaPin, speed);
  } else if (direction == -1) {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
    analogWrite(enaPin, speed);
  } else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, LOW);
    analogWrite(enaPin, 0);
  }
}

// Actuator cutting sequence
void cutter() {
  Serial.println("  Extending...");
  moveMotor(ACT_IN1, ACT_IN2, ACT_ENA, 1, ACT_SPEED);
  delay(5000);
  
  Serial.println("  Holding...");
  moveMotor(ACT_IN1, ACT_IN2, ACT_ENA, 0, 0);
  delay(2000);
  
  Serial.println("  Retracting...");
  moveMotor(ACT_IN1, ACT_IN2, ACT_ENA, -1, ACT_SPEED);
  delay(5000);
  
  moveMotor(ACT_IN1, ACT_IN2, ACT_ENA, 0, 0);
  Serial.println("Actuator Sequence Complete.");
}

// Move stepper motor with smooth stepping
void moveStepperBlocking(Stepper &stp, long stepsToMove) {
  long remaining = stepsToMove;
  while (remaining > 0) {
    stp.step(1);
    --remaining;
    delay(1);
  }
}

// Smooth servo movement
void servoSweepBlocking(int fromDeg, int toDeg, unsigned long stepDelayMs) {
  if (fromDeg < toDeg) {
    for (int pos = fromDeg; pos <= toDeg; ++pos) {
      myServo.write(pos);
      delay(stepDelayMs);
    }
  } else {
    for (int pos = fromDeg; pos >= toDeg; --pos) {
      myServo.write(pos);
      delay(stepDelayMs);
    }
  }
}

// DC motor direction control helpers
void setDCForward(int in1, int in2, int en) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(en, HIGH);
}

void setDCBackward(int in1, int in2, int en) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(en, HIGH);
}

void stopDC(int in1, int in2, int en) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(en, LOW);
}

void parseAndExecute(String command) {
  command.trim();

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

  checkAndControlPeltier();

  // Handle liquid dispensing
  if (name == "liquid") {
    if (id == 0) {
      digitalWrite(PUMP1_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP1_PIN, LOW);
    } else {
      digitalWrite(PUMP2_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP2_PIN, LOW);
    }
    Serial.println("Status: liquid Done");
  // Handle hopper operations
  } else if (name == "hopper") {
    // Spinach with grinding
    if (id == 0) {
      Serial.println("Moving Spinach Stepper");
      moveStepperBlocking(stepper1, STEPPER1_STEPS);
      Serial.println("Turning on Spinach Pumps");
      digitalWrite(PUMP3_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP3_PIN, LOW);
      digitalWrite(PUMP4_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP4_PIN, LOW);
      grind();
    // Cashew with grinding
    } else if (id == 1) {
      Serial.println("Moving cashew Stepper");
      moveStepperBlocking(stepper3, STEPPER1_STEPS);
      grind();
    // Toor dal with pumps
    } else if (id == 2) {
      Serial.println("Moving toor dal stepper");
      moveStepperBlocking(stepper2, STEPPER2_STEPS);
      digitalWrite(PUMP5_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP5_PIN, LOW);
      digitalWrite(PUMP6_PIN, HIGH);
      delay(PUMP_ON_TIME);
      digitalWrite(PUMP6_PIN, LOW);
    // Tomato/onion with cutting
    } else if (id == 3) {
      Serial.println("Tomato DC motors...");
      setDCForward(DC_IN1, DC_IN2, DC_EN);
      delay(5000);
      setDCBackward(DC_IN1, DC_IN2, DC_EN);
      delay(5000);
      stopDC(DC_IN1, DC_IN2, DC_EN);
      cutter();
    }
    Serial.println("Status: hopper Done");
  } else {
    Serial.println("Status: " + name + " Done");
  }
}
