#include <Arduino.h>
#include <HardwareSerial.h>
#include <ESP32Servo.h>
#include <Stepper.h>


#define PUMP1_PIN 5
#define PUMP2_PIN 6
#define PUMP3_PIN 7
#define PUMP4_PIN 8
#define PUMP5_PIN 9
#define PUMP6_PIN 10
#define PUMP7_PIN 11
#define RELAY_PIN 12 // Control pin connected to BC547 base via 1k resistor

// UART communication pins
#define UART_RX_PIN 19 // Serial1 RX pin (connect to master's Serial2 TX)
#define UART_TX_PIN 20 // Serial1 TX pin (connect to master's Serial2 RX)

String inputBuffer = "";

int onTime = 5000;  // 10 seconds ON (in ms)
int offDelay = 1000; // 2 seconds OFF delay between pumps (in ms)

//HOPPER SYSTEM PIN DEFINITIONS
// ---------- Pin Definitions (same as before) ----------
// Stepper Motors (4-wire to ULN2003)
const int stepper1Pins[4] = {40,39,38,37};
const int stepper2Pins[4] = {18,17,16,15};
const int stepper3Pins[4] = {1,2,42,41};

// Servo
const int servoPin = 13;

// DC Motor (IN1, IN2) - single DC motor
const int dcPins[3] = {9, 45, 4}; // DC: IN1, IN2

// Grinder DC Motor (IN1, IN2)
const int grinderPins[3] = {35, 36, 14}; // Grinder: IN1, IN2

// Grinder DC Motor (IN1, IN2)
const int actuatorPins[3] = {47, 48, 46}; // Actuator: IN1, IN2


// ---------- Motor Objects & Settings ----------
Servo myServo;
const int stepsPerRev = 2048; // 28BYJ-48 geared stepper
Stepper stepper1(stepsPerRev, stepper1Pins[0], stepper1Pins[2], stepper1Pins[1], stepper1Pins[3]);
Stepper stepper2(stepsPerRev, stepper2Pins[0], stepper2Pins[2], stepper2Pins[1], stepper2Pins[3]);
Stepper stepper3(stepsPerRev, stepper3Pins[0], stepper3Pins[2], stepper3Pins[1], stepper3Pins[3]);

// Steps to move (you used these earlier — adjust if needed)
const long stepper1Steps = 512;  // ~90 degrees
const long stepper2Steps = 341;  // ~60 degrees
const long stepper3Steps = 682;  // ~120 degrees

// Grinder timings
const unsigned long grinderOnMs  = 3000UL; // 3 s ON per pulse
const unsigned long grinderOffMs = 1000UL; // 1 s OFF between pulses
const int grinderPulseCount = 3;           // 3 pulses

//Actautor timings
const unsigned long actuatorMs  = 7000UL; // 3 s ON per pulse

// Inter-item pause
const unsigned long interDelay = 2000UL; // 2 seconds pause between each action

void setup()
{
    // Initialize Serial for debug output
    Serial.begin(115200);
    Serial.println("ESP32-S3 UART Slave Started");

    // Initialize Serial2 for communication with master (use specific pins)
    Serial2.begin(115200, SERIAL_8N1, UART_RX_PIN, UART_TX_PIN);

    // Set pump pins as OUTPUT
    pinMode(PUMP1_PIN, OUTPUT);
    pinMode(PUMP2_PIN, OUTPUT);
    pinMode(PUMP3_PIN, OUTPUT);
    pinMode(PUMP4_PIN, OUTPUT);
    pinMode(PUMP5_PIN, OUTPUT);
    pinMode(PUMP6_PIN, OUTPUT);
    pinMode(PUMP7_PIN, OUTPUT);

    // Ensure all pumps are OFF initially
    digitalWrite(PUMP1_PIN, LOW);
    digitalWrite(PUMP2_PIN, LOW);
    digitalWrite(PUMP3_PIN, LOW);
    digitalWrite(PUMP4_PIN, LOW);
    digitalWrite(PUMP5_PIN, LOW);
    digitalWrite(PUMP6_PIN, LOW);
    digitalWrite(PUMP7_PIN, LOW);



  //--------------HOPPER SETUP
  // Stepper speed (Stepper library: speed is in RPM; keep low for 28BYJ-48)
  stepper1.setSpeed(10);
  stepper2.setSpeed(10);
  stepper3.setSpeed(10);

  // Servo attach
  myServo.attach(servoPin);
  myServo.write(0);

  // DC motor pins as outputs
  for (int i = 0; i < 3; ++i) {
    pinMode(dcPins[i], OUTPUT);
    pinMode(grinderPins[i], OUTPUT);
  }

  // Ensure motors off initially
  stopDC(dcPins);
  stopDC(grinderPins);

}

//-------------Hopper Helper Functions--------------------

void grind(){
  Serial.println("Grinder: " + String(grinderPulseCount) + " pulses of 3s ON / 1s OFF");
  for (int p = 1; p <= grinderPulseCount; ++p) {
    Serial.printf("Grinder pulse %d: ON\n", p);
    setDCForward(grinderPins);
    delay(grinderOnMs);
    stopDC(grinderPins);
    Serial.printf("Grinder pulse %d: OFF\n", p);
    if (p < grinderPulseCount) delay(grinderOffMs); // wait between pulses
  }
  servoSweepBlocking(0, 90, 2);   // step delay 2 ms for smoothness
  delay(1000);
  servoSweepBlocking(90, 0, 2);
  Serial.println("Servo done.");
  Serial.println("Grinder sequence done.");
}

void cutter(){
  Serial.println("Extending Cutter Actuator...");
  setDCForward(actuatorPins);
  delay(actuatorMs);

  Serial.println("Retracting Cutter Actuator...");
  setDCBackward(actuatorPins);
  delay(actuatorMs);
  stopDC(actuatorPins);
}

void moveStepperBlocking(Stepper &stp, long stepsToMove) {
  // move() is blocking for Stepper library; we'll step one-by-one to be a bit gentler
  long remaining = stepsToMove;
  while (remaining > 0) {
    stp.step(1);      // single step
    --remaining;
    delay(1);         // tiny pause to avoid hammering CPU (and smooth motion)
  }
}

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

// DC helper: set forward (IN1=HIGH, IN2=LOW, EN=HIGH)
void setDCForward(const int dc[3]) {
  digitalWrite(dc[0], HIGH);
  digitalWrite(dc[1], LOW);
  digitalWrite(dc[2], HIGH);
}

void setDCBackward(const int dc[3]) {
  digitalWrite(dc[0], LOW);
  digitalWrite(dc[1], HIGH);
  digitalWrite(dc[2], HIGH);
}

// DC helper: stop
void stopDC(const int dc[3]) {
  digitalWrite(dc[0], LOW);
  digitalWrite(dc[1], LOW);
  digitalWrite(dc[2], LOW);
}

void parseAndExecute(String command)
{
    if (command.startsWith("MODULE:"))
        command.remove(0, 7);

    command.trim();

    // Parse single command format: name|id|value
    String parts[3];
    int seg = 0, start = 0;
    while (seg < 3)
    {
        int bar = command.indexOf('|', start);
        if (bar == -1)
        {
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

    if (name == "liquid")
    {      
          if(id==0){
            digitalWrite(PUMP1_PIN, HIGH);
            delay(onTime);
            digitalWrite(PUMP1_PIN, LOW);
          } else{
            digitalWrite(PUMP2_PIN, HIGH);
            delay(onTime);
            digitalWrite(PUMP2_PIN, LOW);
          }

        Serial.println("Status: liquid Done");
    }
    else if (name == "hopper")
    {   if(id==0){
        //-------------spinach----------------
        Serial.println("Moving Spinach Stepper");
        moveStepperBlocking(stepper2, stepper2Steps);
        Serial.println("Turning on Spinach Pumps");
        digitalWrite(PUMP3_PIN, HIGH);
        delay(onTime);
        digitalWrite(PUMP3_PIN, LOW);
        digitalWrite(PUMP4_PIN, HIGH);
        delay(onTime);
        digitalWrite(PUMP4_PIN, LOW);
        grind();
    }else if(id==1){
      //--------------Cashew-------------------
        Serial.println("Moving cashew Stepper");
        moveStepperBlocking(stepper3, stepper3Steps);
        grind();
    }else if(id==2){
      //-----------Toor Dal------------------
        Serial.println("Moving toor dal stepper");
        moveStepperBlocking(stepper1, stepper1Steps);
        digitalWrite(PUMP5_PIN, HIGH);
        delay(onTime);
        digitalWrite(PUMP5_PIN, LOW);

        digitalWrite(PUMP6_PIN, HIGH);
        delay(onTime);
        digitalWrite(PUMP6_PIN, LOW);
        //delay(offDelay);
    }else if(id==3){
      //------------ONION & TOMATO---------------
      Serial.println("Tomato DC motors...");
      setDCForward(dcPins);
      delay(5000);
      setDCBackward(dcPins);
      delay(5000);
      stopDC(dcPins);
      cutter();
    }
        Serial.println("Status: hopper Done");
    }
    else
    {
        Serial.println("Status: " + name + " Done");
    }
}

void loop()
{
    // Read all available data from Serial2
    while (Serial2.available() > 0)
    {
        char c = Serial2.read();
        if (c == '\n' || c == '\r')
        {
            if (inputBuffer.length() > 0)
            {
                inputBuffer.trim();
                if (inputBuffer.length() > 0)
                {
                    Serial.println(inputBuffer); // Display received command on USB UART

                    if (inputBuffer.startsWith("MODULE:"))
                    {
                        parseAndExecute(inputBuffer.substring(7));
                    }
                }
                inputBuffer = ""; // Clear buffer for next command
            }
        }
        else
        {
            inputBuffer += c;
        }
    }
}
