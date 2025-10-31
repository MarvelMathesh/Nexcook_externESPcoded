// Globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "HX711.h"

//cooktop stirrer pin definitions
#define ENA 15
#define IN1 14
#define IN2 13

//Spice dispenser pin definitions
// HX711 Load Cell
#define LOADCELL_DOUT_PIN 2
#define LOADCELL_SCK_PIN  1

// L298N-A: Position Motor (Motor A)
#define POSITION_ENA 4
#define POSITION_IN1 5
#define POSITION_IN2 6

// L298N-A: Rack & Pinion Motor (Motor B)
#define RACK_ENB 7
#define RACK_IN3 15
#define RACK_IN4 16

// L298N-B: Dispensing Motor (Motor A)
#define DISPENSE_ENA 17
#define DISPENSE_IN1 18
#define DISPENSE_IN2 8

// L298N-B: Actuator Motor (Motor B)
#define ACTUATOR_ENB 3
#define ACTUATOR_IN3 46
#define ACTUATOR_IN4 9


#endif
