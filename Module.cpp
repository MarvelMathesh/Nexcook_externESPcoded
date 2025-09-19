#include "Module.h"

//Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip(1, 48, NEO_GRB + NEO_KHZ800);   

void SpiceDispenser::start()
{
  Serial.println(String("Dispensing spice")+ weight + " of id "+id);
  strip.setPixelColor(0, strip.Color(255, 0, 0));
  delay(5000);

}
void SpiceDispenser::stop()
{

}

void Hopper::start()
{
  Serial.println(String("Dispensing hopper ")+ weight + " of id "+id);
  strip.setPixelColor(0, strip.Color(0, 255, 0));
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
  
}

void Stirrer::stop()
{
  

}


void LiquidDispenser::start()
{
 Serial.println(String("Dispensing liquid ")+ vol + " of id "+id); 
 strip.setPixelColor(0, strip.Color(0, 0, 255));
 delay(5000);
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

