#include "Module.h"
#include "globals.h"
#include "HX711.h"

extern HX711 scale;
extern float calibration_factor;

void SpiceDispenser::start()
{
  dispenseSpice(id, (float)weight);
}

void SpiceDispenser::stop()
{
  stopPositionMotor();
  stopRackMotor();
  stopDispenseMotor();
  stopActuatorMotor();
}

void SpiceDispenser::dispenseSpice(int spiceID, float targetWeight) {
  int positionTime = 0;
  
  // Determine position time based on spice ID
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
  
  // Step 1: Position Motor forward to spice location
  movePositionMotor(MOTOR_SPEED, true);
  delay(positionTime);
  stopPositionMotor();
  delay(500);
  
  // Step 2: Tare the scale before dispensing
  long initialReading = scale.read_average(10);
  delay(500);
  
  // Step 3: Rack & Pinion Motor forward
  moveRackMotor(MOTOR_SPEED, true);
  delay(RACK_FORWARD_TIME);
  stopRackMotor();
  delay(500);
  
  // Step 4: Dispensing Motor until target weight reached
  moveDispenseMotor(MOTOR_SPEED, true);
  
  // Calculate target raw reading
  long targetRawReading = initialReading + (long)(targetWeight * calibration_factor);
  
  unsigned long lastPrint = 0;
  while (true) {
    if (scale.is_ready()) {
      long currentRawReading = scale.read_average(3);
      
      // Calculate current weight from raw reading
      float currentWeight = (float)(currentRawReading - initialReading) / calibration_factor;
      
      // Check if target weight reached (raw reading more negative)
      if (currentRawReading <= targetRawReading) {
        stopDispenseMotor();
        break;
      }
      
      // Slow down when close to target
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
  
  // Step 5: Rack & Pinion Motor backward
  moveRackMotor(MOTOR_SPEED, false);
  delay(RACK_BACKWARD_TIME);
  stopRackMotor();
  delay(500);
  
  // Step 6: Actuator Motor forward
  moveActuatorMotor(MOTOR_SPEED, true);
  delay(ACTUATOR_TIME);
  stopActuatorMotor();
  delay(500);
  
  // Step 7: Actuator Motor backward
  moveActuatorMotor(MOTOR_SPEED, false);
  delay(ACTUATOR_TIME);
  stopActuatorMotor();
  delay(500);
  
  // Step 8: Position Motor backward (return to home)
  movePositionMotor(MOTOR_SPEED, false);
  delay(positionTime);
  stopPositionMotor();
}

// Position Motor Control Functions
void SpiceDispenser::movePositionMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  
  if (forward) {
    digitalWrite(POSITION_IN1, HIGH);
    digitalWrite(POSITION_IN2, LOW);
  } else {
    digitalWrite(POSITION_IN1, LOW);
    digitalWrite(POSITION_IN2, HIGH);
  }
  analogWrite(POSITION_ENA, speed);
}

void SpiceDispenser::stopPositionMotor() {
  digitalWrite(POSITION_IN1, LOW);
  digitalWrite(POSITION_IN2, LOW);
  analogWrite(POSITION_ENA, 0);
}

// Rack & Pinion Motor Control Functions
void SpiceDispenser::moveRackMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  
  if (forward) {
    digitalWrite(RACK_IN3, HIGH);
    digitalWrite(RACK_IN4, LOW);
  } else {
    digitalWrite(RACK_IN3, LOW);
    digitalWrite(RACK_IN4, HIGH);
  }
  analogWrite(RACK_ENB, speed);
}

void SpiceDispenser::stopRackMotor() {
  digitalWrite(RACK_IN3, LOW);
  digitalWrite(RACK_IN4, LOW);
  analogWrite(RACK_ENB, 0);
}

// Dispensing Motor Control Functions
void SpiceDispenser::moveDispenseMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  
  if (forward) {
    digitalWrite(DISPENSE_IN1, HIGH);
    digitalWrite(DISPENSE_IN2, LOW);
  } else {
    digitalWrite(DISPENSE_IN1, LOW);
    digitalWrite(DISPENSE_IN2, HIGH);
  }
  analogWrite(DISPENSE_ENA, speed);
}

void SpiceDispenser::stopDispenseMotor() {
  digitalWrite(DISPENSE_IN1, LOW);
  digitalWrite(DISPENSE_IN2, LOW);
  analogWrite(DISPENSE_ENA, 0);
}

// Actuator Motor Control Functions
void SpiceDispenser::moveActuatorMotor(int speed, bool forward) {
  speed = constrain(speed, 0, 255);
  
  if (forward) {
    digitalWrite(ACTUATOR_IN3, HIGH);
    digitalWrite(ACTUATOR_IN4, LOW);
  } else {
    digitalWrite(ACTUATOR_IN3, LOW);
    digitalWrite(ACTUATOR_IN4, HIGH);
  }
  analogWrite(ACTUATOR_ENB, speed);
}

void SpiceDispenser::stopActuatorMotor() {
  digitalWrite(ACTUATOR_IN3, LOW);
  digitalWrite(ACTUATOR_IN4, LOW);
  analogWrite(ACTUATOR_ENB, 0);
}

void Hopper::start()
{
  
}

void Hopper::stop()
{
  
}


void Grind::start()
{
  

}

void Grind::stop()
{
  

}



void Chop::start()
{
  

}

void Chop::stop()
{
 

}


void Heat::start()
{
  
}

void Heat::stop()
{
  
  
}


void Steamer::start()
{
  
}

void Steamer::stop()
{
  

}

void Stirrer::start()
{
  /*digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);*/
}

void Stirrer::stop()
{
   //analogWrite(ENA, 0);

}


void LiquidDispenser::start()
{

}

void LiquidDispenser::stop()
{

}

void Cleaning::start()
{
  
}

void Cleaning::stop()
{
  
}
