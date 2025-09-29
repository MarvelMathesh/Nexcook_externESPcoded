#include "Module.h"

#define ENA 15
#define IN1 14
#define IN2 13

// Module object creation
SpiceDispenser spicedis;
Hopper hopper;
Grind grind;
Chop chop;
Heat heat;
LiquidDispenser liqdis;
Steamer steam;
Stirrer stirrer;
Cleaning clean;

String inputString = "";
bool stringcmp = false;

void setup()
{
    Serial.begin(115200);
    Serial1.begin(115200, SERIAL_8N1, 18, 17);
    strip.begin();
    strip.show();
    pinMode(ENA,OUTPUT);
    pinMode(IN1,OUTPUT);
    pinMode(IN2,OUTPUT);
}

void loop()
{   
    if (Serial1.available() > 0) {
    Serial1.setTimeout(100);
    String inputString = Serial1.readString();
    inputString.trim();
    
    if (inputString.length() > 0) {
      Serial.println(inputString);
      stringcmp = true;
    }
  }
    if (stringcmp)
    {
        if (inputString.startsWith("MODULE:"))
        {
            parseAndExecute(inputString.substring(7));
        }
        else
        {
            Serial.println("Unknown command received.");
        }
        inputString = "";
        stringcmp = false;
    }
}
/*
void serialEvent()
{
    if (Serial1.available() > 0) {
    Serial1.setTimeout(100);
    String inputString = Serial1.readString();
    inputString.trim();
    
    if (inputString.length() > 0) {
      Serial.println(inputString);
      stringcmp = true;
    }
  }
}*/

void parseAndExecute(String command)
{
    if (command.startsWith("MODULE:"))
        command.remove(0, 7);

    int StepIndex = 0;
    command.trim();
    if (!command.endsWith(",")) command += ',';   

    int lastIndex = 0;
    while (true)
    {
        int nextIndex = command.indexOf(',', lastIndex);
        if (nextIndex == -1) break;

        String entry = command.substring(lastIndex, nextIndex);
        entry.trim();
        lastIndex = nextIndex + 1;
        if (entry.length() == 0) continue;

        
        String parts[3];
        int seg = 0, start = 0;
        while (seg < 3)
        {
            int bar = entry.indexOf('|', start);
            if (bar == -1)
            {
                parts[seg++] = entry.substring(start);
                break;
            }
            parts[seg++] = entry.substring(start, bar);
            start = bar + 1;
        }
        while (seg < 3) parts[seg++] = "";

        String name  = parts[0];
        int    id    = parts[1].length() ? parts[1].toInt() : -1;
        int    value = parts[2].length() ? parts[2].toInt() : -1;

        
        if (name == "liquid")
        {
            if (id >= 0) liqdis.id = id;
            if (value >= 0) liqdis.vol = value;
            liqdis.start();
            Serial.println(String("Status: ")+StepIndex+" Done");
        }
        else if (name == "spice")
        {
            if (id >= 0) spicedis.id = id;
            if (value >= 0) spicedis.weight = value;
            spicedis.start();
            Serial.println(String("Status: ")+StepIndex+" Done");
        }
        else if (name == "hopper")
        {
            if (id >= 0) hopper.id = id;
            if (value >= 0) hopper.weight = value;
            hopper.start();
            Serial.println(String("Status: ")+StepIndex+" Done");
        }
        else if (name == "clean")
        {
            clean.start();
            Serial.println(String("Status: ")+StepIndex+" Done");
        }
        else if (name == "stirrer")
        {
          stirrer.start();
          delay(5000);
          stirrer.stop();
          Serial.println(String("Status: ")+StepIndex+" Done");
          
        }
        else
        {
          Serial.println("Unknown Module"); 
          Serial.println(String("Status: ")+StepIndex+" Error"); 
        }

      //Serial.println(String("Status: ")+StepIndex+" Done");
      StepIndex++;
    }

    Serial.println("complete");
}

