#include "Module.h"

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
    strip.begin();
    strip.show();
}

void loop()
{
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

void serialEvent()
{
    while (Serial.available())
    {
        char inChar = (char)Serial.read();
        inputString += inChar;
        if (inChar == '\n' || inChar == '\r')
        {
            stringcmp = true;
        }
    }
}

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

