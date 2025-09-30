// Globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "HX711.h"

extern HX711 scale;  // Declare the external variable

//cooktop stirrer pin definitions
#define ENA 15
#define IN1 14
#define IN2 13

//Spice dispenser pin definitions
// Stepper driver (A4988)
#define DIR_PIN   26
#define STEP_PIN  21

// L298N Motor A
#define ENA_PIN  38
#define IN1_PIN  37
#define IN2_PIN  36   

// L298N Motor B
#define ENB_PIN 33
#define IN3_PIN 35
#define IN4_PIN 34	

#define LOADCELL_DOUT_PIN 41
#define LOADCELL_SCK_PIN  40

//pumps
#define PUMP1_PIN 23
#define PUMP2_PIN 18
#define PUMP3_PIN 19
#define PUMP4_PIN 27
#define PUMP5_PIN 28
#define PUMP6_PIN 2
#define PUMP7_PIN 4



#endif
