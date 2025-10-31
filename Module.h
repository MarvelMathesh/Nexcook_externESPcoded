#ifndef MODULE_H
#define MODULE_H

#include <Adafruit_NeoPixel.h>
#include <Arduino.h> 
//on board rgb led setup (for diagnotic purposes)
#define LED_PIN 48
#define NUM_LEDS 1

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
extern Adafruit_NeoPixel strip;

class Module
{
public:
  virtual void start() = 0;
  virtual void stop() = 0;
};

class SpiceDispenser : public Module
{
public:
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
  
  int weight;
  int id;
  
  // Configuration constants
  static const int MOTOR_SPEED = 200;
  static const int RACK_FORWARD_TIME = 2000;
  static const int RACK_BACKWARD_TIME = 2000;
  static const int ACTUATOR_TIME = 5000;
  static const int TURMERIC_TIME = 5000;
  static const int SALT_TIME = 10000;
  static const int CHILI_TIME = 15000;
  static const int PEPPER_TIME = 20000;
};

class Hopper : public Module
{
public:
  void start() override;
  void stop() override;
  int weight;
  int id;
};

class Grind : public Module
{
public:
  void start() override;
  void stop() override;
};

class Chop : public Module
{
public:
  void start() override;
  void stop() override;
  
};

class Heat : public Module
{
public:
  void start() override;
  void stop() override;
  int temp;
};

class LiquidDispenser : public Module
{
public:
  void start() override;
  void stop() override;
  int vol;
  int id;
};

class Steamer : public Module
{
public:
  void start() override;
  void stop() override;
};

class Stirrer : public Module
{
public:
  void start() override;
  void stop() override;
};

class Cleaning : public Module
{
public:
  void start() override;
  void stop() override;
};

#endif
