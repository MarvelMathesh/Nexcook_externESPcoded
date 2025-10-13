#include "Module.h"
#include "globals.h"

int onTime = 5000; 

//spice
int motorSpeed = 85;

// Stepper settings
extern const int stepsPerRevolution = 200;  // Adjust based on your stepper motor
extern const int stepDelay = 2000; // microseconds between steps (adjust for speed)

// Hopper settings
const int HOPPER_STEPS_PER_REV = 200;
const int HOPPER_STEP_DELAY = 800; // microseconds
/*
//forward declarations of functions for spice dispenser
void moveStepperToPosition(int targetDegrees);
void runMotorA(bool forward, int duration);
void runMotorBUntilWeight(float targetWeight);*/

//forward declarations for hopper functions
void moveHopperStepperForward();
void moveHopperStepperBackward();
void motorA_forward();
void motorA_stop();
void motorB_forward();
void motorB_reverse();
void motorB_stop();
void motorC_forward();
void motorC_reverse();
void motorC_stop();

void SpiceDispenser::start()
{/*
 if(id==0){
    moveStepperToPosition(0);
 } else if(id==1){
    moveStepperToPosition(90);
 } else if(id==2){
    moveStepperToPosition(180);
 } else if(id==3){
    moveStepperToPosition(270);
 }  

 delay(500);

  //dock motor A
  runMotorA(true, 3000); // Forward for 3 seconds
  delay(500);

  runMotorBUntilWeight(weight);
  delay(500);

  //move motorA back to undocked position
  runMotorA(false, 3000);

  //move stepper back to home (0 deg)
  moveStepperToPosition(0);
*/

}
void SpiceDispenser::stop()
{

}

void Hopper::start()
{
  Serial.println(String("Starting hopper sequence for id ")+ id + " with weight " + weight);
  
  // Stepper forward
  moveHopperStepperForward();
  delay(1000);

  // Motor B forward - 300ms
  motorB_forward();
  delay(300);
  motorB_stop();
  delay(500);

  // Motor A forward - 4s
  motorA_forward();
  delay(4000);
  motorA_stop();
  delay(500);

  // Motor B reverse - 300ms
  motorB_reverse();
  delay(300);
  motorB_stop();
  delay(500);

  // Stepper back to original
  moveHopperStepperBackward();
  delay(1000);

  // Motor C forward - 3s
  motorC_forward();
  delay(3000);
  motorC_stop();
  delay(500);

  // Motor C reverse - 3s
  motorC_reverse();
  delay(3000);
  motorC_stop();
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
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 200);
}

void Stirrer::stop()
{
   analogWrite(ENA, 0);

}


void LiquidDispenser::start()
{
if(id==0){
  digitalWrite(PUMP7_PIN, HIGH);
  delay(onTime);
 }
 else {
  digitalWrite(PUMP5_PIN, HIGH);
  delay(onTime);
}
}

void LiquidDispenser::stop()
{
  if(id==0){
  digitalWrite(PUMP7_PIN, LOW);
  //delay(onTime);
  }
 else digitalWrite(PUMP5_PIN, LOW);
}

void Cleaning::start()
{
  
}

void Cleaning::stop()
{
  
}

/*
//spice dispenser functions
void moveStepperToPosition(int targetDegrees) {
  int currentPosition = 0; // Assuming we start at 0
  int targetSteps = (targetDegrees * stepsPerRevolution) / 360;
  int currentSteps = (currentPosition * stepsPerRevolution) / 360;
  int stepsToMove = targetSteps - currentSteps;
  
  // Set direction
  if (stepsToMove > 0) {
    digitalWrite(DIR_PIN, HIGH); // Clockwise
  } else {
    digitalWrite(DIR_PIN, LOW);  // Counter-clockwise
    stepsToMove = abs(stepsToMove);
  }
  
  // Move stepper
  for (int i = 0; i < stepsToMove; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelay);
  }
  
  Serial.print("Stepper moved to ");
  Serial.print(targetDegrees);
  Serial.println(" degrees");
}

void runMotorA(bool forward, int duration) {
  // Set direction
  if (forward) {
    digitalWrite(IN1_PIN, HIGH);
    digitalWrite(IN2_PIN, LOW);
  } else {
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }
  
  // Set speed
  digitalWrite(ENA_PIN, HIGH);
  
  // Run for specified duration
  delay(duration);
  
  // Stop motor
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  
  Serial.print("Motor A ran for ");
  Serial.print(duration);
  Serial.println(" ms");
}

void runMotorBUntilWeight(float targetWeight) {
  // Start Motor B
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
  digitalWrite(ENB_PIN, HIGH); // Full speed for Motor B
  
  float currentWeight = 0;
  
  while (currentWeight < targetWeight) {
    if (scale.is_ready()) {
      currentWeight = scale.get_units();
      Serial.print("Current weight: ");
      Serial.print(currentWeight);
      Serial.println(" g");
    }
    delay(100); // Check weight every 100ms
  }
  
  // Stop Motor B
  digitalWrite(ENA_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
  
  Serial.print("Motor B stopped. Final weight: ");
  Serial.print(currentWeight);
  Serial.println(" g");
}*/

// ================= HOPPER FUNCTIONS =================

void moveHopperStepperForward() {
  int steps = (HOPPER_STEPS_PER_REV * 2700) / 360;
  digitalWrite(DIR_PIN, HIGH);
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(HOPPER_STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(HOPPER_STEP_DELAY);
  }
}

void moveHopperStepperBackward() {
  int steps = (HOPPER_STEPS_PER_REV * 2700) / 360;
  digitalWrite(DIR_PIN, LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(HOPPER_STEP_DELAY);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(HOPPER_STEP_DELAY);
  }
}

// ------------------- MOTOR A (Hopper) -------------------
void motorA_forward() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
}

void motorA_stop() {
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
}

// ------------------- MOTOR B (Hopper) -------------------
void motorB_forward() {
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);
}

void motorB_reverse() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, HIGH);
}

void motorB_stop() {
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);
}

// ------------------- MOTOR C (Hopper) -------------------
void motorC_forward() {
  digitalWrite(INC1_PIN, HIGH);
  digitalWrite(INC2_PIN, LOW);
}

void motorC_reverse() {
  digitalWrite(INC1_PIN, LOW);
  digitalWrite(INC2_PIN, HIGH);
}

void motorC_stop() {
  digitalWrite(INC1_PIN, LOW);
  digitalWrite(INC2_PIN, LOW);
}
