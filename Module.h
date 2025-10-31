#ifndef MODULE_H
#define MODULE_H

#include <Arduino.h>

// Base class for all hardware modules
class Module {
public:
  virtual void start() = 0;
  virtual void stop() = 0;
};

// Spice dispenser with weight-based dispensing
class SpiceDispenser : public Module {
public:
  // Motor speed and timing constants
  static const int MOTOR_SPEED = 200;
  static const int RACK_FORWARD_TIME = 2000;
  static const int RACK_BACKWARD_TIME = 2000;
  static const int ACTUATOR_TIME = 5000;
  // Position motor timing for each spice location
  static const int TURMERIC_TIME = 5000;
  static const int SALT_TIME = 10000;
  static const int CHILI_TIME = 15000;
  static const int PEPPER_TIME = 20000;

  int id;
  int weight;

  void start() override;
  void stop() override;
  void dispenseSpice(int spiceID, float targetWeight);
  void movePositionMotor(int speed, bool forward);
  void stopPositionMotor();
  void moveRackMotor(int speed, bool forward);
  void stopRackMotor();
  void moveDispenseMotor(int speed, bool forward);
  void stopDispenseMotor();
  void moveActuatorMotor(int speed, bool forward);
  void stopActuatorMotor();
};

// Hopper for solid ingredients
class Hopper : public Module {
public:
  int id;
  int weight;

  void start() override;
  void stop() override;
};

// Grinding module
class Grind : public Module {
public:
  void start() override;
  void stop() override;
};

// Chopping module
class Chop : public Module {
public:
  void start() override;
  void stop() override;
};

// Heating element control
class Heat : public Module {
public:
  int temp;

  void start() override;
  void stop() override;
};

// Liquid dispenser (oil, water, etc.)
class LiquidDispenser : public Module {
public:
  int id;
  int vol;

  void start() override;
  void stop() override;
};

// Steaming module
class Steamer : public Module {
public:
  void start() override;
  void stop() override;
};

// Stirrer motor control
class Stirrer : public Module {
public:
  void start() override;
  void stop() override;
};

// Cleaning system
class Cleaning : public Module {
public:
  void start() override;
  void stop() override;
};

#endif
