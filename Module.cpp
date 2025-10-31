#include "Module.h"
#include "globals.h"
#include "HX711.h"

extern HX711 scale;
extern float calibration_factor;

void SpiceDispenser::start() {
  dispenseSpice(id, (float)weight);
}

void SpiceDispenser::stop() {
  stopPositionMotor();
  stopRackMotor();
  stopDispenseMotor();
  stopActuatorMotor();
}

void SpiceDispenser::dispenseSpice(int spiceID, float targetWeight) {
  // Determine position time based on spice ID
  int positionTime = 0;
  
  if (spiceID == 1) {
    positionTime = TURMERIC_TIME;
  } else if (spiceID == 2) {
    positionTime = SALT_TIME;
  } else if (spiceID == 3) {
    positionTime = CHILI_TIME;
  } else if (spiceID == 4) {
    positionTime = PEPPER_TIME;
  } else {
    return;
  }
  
  // Move to spice location
  movePositionMotor(MOTOR_SPEED, true);
  delay(positionTime);
  stopPositionMotor();
  delay(500);
  
  // Tare scale before dispensing
  long initialReading = scale.read_average(10);
  delay(500);
  
  // Move rack forward to engage dispenser
  moveRackMotor(MOTOR_SPEED, true);
  delay(RACK_FORWARD_TIME);
  stopRackMotor();
  delay(500);
  
  // Dispense until target weight reached
  moveDispenseMotor(MOTOR_SPEED, true);
  long targetRawReading = initialReading + (long)(targetWeight * calibration_factor);
  
  while (true) {
    if (scale.is_ready()) {
      long currentRawReading = scale.read_average(3);
      float currentWeight = (float)(currentRawReading - initialReading) / calibration_factor;
      
      if (currentRawReading <= targetRawReading) {
        stopDispenseMotor();
        break;
      }
      
      float remaining = targetWeight - currentWeight;
      if (remaining < 2.0 && remaining > 0) {
        stopDispenseMotor();
        delay(100);
        moveDispenseMotor(MOTOR_SPEED, true);
        delay(200);
      }
    }
    delay(50);
  }
  delay(500);
  
  moveRackMotor(MOTOR_SPEED, false);
  delay(RACK_BACKWARD_TIME);
  stopRackMotor();
  delay(500);
  
  // Actuator forward to release spice
  moveActuatorMotor(MOTOR_SPEED, true);
  delay(ACTUATOR_TIME);
  stopActuatorMotor();
  delay(500);
  
  moveActuatorMotor(MOTOR_SPEED, false);
  delay(ACTUATOR_TIME);
  stopActuatorMotor();
  delay(500);
  
  // Return to home position
  movePositionMotor(MOTOR_SPEED, false);
  delay(positionTime);
  stopPositionMotor();
}

void SpiceDispenser::movePositionMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  digitalWrite(POSITION_IN1, forward ? HIGH : LOW);
  digitalWrite(POSITION_IN2, forward ? LOW : HIGH);
  analogWrite(POSITION_ENA, speed);
}

void SpiceDispenser::stopPositionMotor() {
  digitalWrite(POSITION_IN1, LOW);
  digitalWrite(POSITION_IN2, LOW);
  analogWrite(POSITION_ENA, 0);
}

void SpiceDispenser::moveRackMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  digitalWrite(RACK_IN3, forward ? HIGH : LOW);
  digitalWrite(RACK_IN4, forward ? LOW : HIGH);
  analogWrite(RACK_ENB, speed);
}

void SpiceDispenser::stopRackMotor() {
  digitalWrite(RACK_IN3, LOW);
  digitalWrite(RACK_IN4, LOW);
  analogWrite(RACK_ENB, 0);
}

void SpiceDispenser::moveDispenseMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  digitalWrite(DISPENSE_IN1, forward ? HIGH : LOW);
  digitalWrite(DISPENSE_IN2, forward ? LOW : HIGH);
  analogWrite(DISPENSE_ENA, speed);
}

void SpiceDispenser::stopDispenseMotor() {
  digitalWrite(DISPENSE_IN1, LOW);
  digitalWrite(DISPENSE_IN2, LOW);
  analogWrite(DISPENSE_ENA, 0);
}

void SpiceDispenser::moveActuatorMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  digitalWrite(ACTUATOR_IN3, forward ? HIGH : LOW);
  digitalWrite(ACTUATOR_IN4, forward ? LOW : HIGH);
  analogWrite(ACTUATOR_ENB, speed);
}

void SpiceDispenser::stopActuatorMotor() {
  digitalWrite(ACTUATOR_IN3, LOW);
  digitalWrite(ACTUATOR_IN4, LOW);
  analogWrite(ACTUATOR_ENB, 0);
}

void Hopper::start() {}
void Hopper::stop() {}

void Grind::start() {}
void Grind::stop() {}

void Chop::start() {}
void Chop::stop() {}

void Heat::start() {}
void Heat::stop() {}

void Steamer::start() {}
void Steamer::stop() {}

void Stirrer::start() {}
void Stirrer::stop() {}

void LiquidDispenser::start() {}
void LiquidDispenser::stop() {}

void Cleaning::start() {}
void Cleaning::stop() {}
