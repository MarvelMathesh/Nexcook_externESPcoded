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
  int weight;
  int id;
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
