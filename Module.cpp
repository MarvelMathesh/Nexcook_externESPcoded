#include "Module.h"
#include "globals.h"

int onTime = 5000; 

//spice
int motorSpeed = 85;

// Stepper settings
extern const int stepsPerRevolution = 200;  // Adjust based on your stepper motor
extern const int stepDelay = 2000; // microseconds between steps (adjust for speed)




//forward declararations of funcions for spice dispenser
void moveStepperToPosition(int targetDegrees);
void runMotorA(bool forward, int duration);
void runMotorBUntilWeight(float targetWeight);

void SpiceDispenser::start()
{
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


}
void SpiceDispenser::stop()
{

}

void Hopper::start()
{
  Serial.println(String("Dispensing hopper ")+ weight + " of id "+id);
  delay(5000);
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
}
