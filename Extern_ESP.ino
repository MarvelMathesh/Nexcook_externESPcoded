#include "Module.h"
#include "HX711.h"
#include "globals.h"
HX711 scale;
float calibration_factor = -7050;

// UART SETUP for slave communication

#define SLAVE_RX_PIN 19 // Serial2 RX pin (connect to slave's Serial2 TX pin 17)
#define SLAVE_TX_PIN 20 // Serial2 TX pin (connect to slave's Serial2 RX pin 16)

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

String inputBuffer = "";
const int MAX_BUFFER_SIZE = 256; // Prevent buffer overflow

void setup()
{
    Serial.begin(115200);  // PC debug
    Serial1.begin(115200); // Display communication

    // Initialize Serial2 with specific pins for slave communication
    Serial2.begin(115200, SERIAL_8N1, SLAVE_RX_PIN, SLAVE_TX_PIN);

    // stirrer
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // spice dispenser motors
    pinMode(POSITION_ENA, OUTPUT);
    pinMode(POSITION_IN1, OUTPUT);
    pinMode(POSITION_IN2, OUTPUT);
    pinMode(RACK_ENB, OUTPUT);
    pinMode(RACK_IN3, OUTPUT);
    pinMode(RACK_IN4, OUTPUT);
    pinMode(DISPENSE_ENA, OUTPUT);
    pinMode(DISPENSE_IN1, OUTPUT);
    pinMode(DISPENSE_IN2, OUTPUT);
    pinMode(ACTUATOR_ENB, OUTPUT);
    pinMode(ACTUATOR_IN3, OUTPUT);
    pinMode(ACTUATOR_IN4, OUTPUT);

    // Initialize HX711 load cell
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor);
    scale.tare();
    
    // Stop all spice dispenser motors initially
    spicedis.stopPositionMotor();
    spicedis.stopRackMotor();
    spicedis.stopDispenseMotor();
    spicedis.stopActuatorMotor();
}

void loop()
{
    // Read all available data from Serial1
    while (Serial1.available() > 0)
    {
        char c = Serial1.read();
        if (c == '\n' || c == '\r')
        {
            if (inputBuffer.length() > 0)
            {
                inputBuffer.trim();
                if (inputBuffer.length() > 0)
                {
                    Serial.println("Received: " + inputBuffer); // Debug: show what was received

                    if (inputBuffer.startsWith("MODULE:"))
                    {
                        parseAndExecute(inputBuffer);
                    }
                    else
                    {
                        Serial.println("Invalid command format: " + inputBuffer);
                    }
                }
                inputBuffer = ""; // Clear buffer for next command
            }
        }
        else
        {
            // Prevent buffer overflow
            if (inputBuffer.length() < MAX_BUFFER_SIZE)
            {
                inputBuffer += c;
            }
            else
            {
                Serial.println("Buffer overflow! Clearing buffer.");
                inputBuffer = "";
            }
        }
    }
}

void parseAndExecute(String command)
{
    // Remove "MODULE:" prefix
    if (command.startsWith("MODULE:"))
        command = command.substring(7);

    command.trim();
    Serial.println("Processing command: " + command); // Debug output

    // Parse single command format: name|id|value
    String parts[3];
    int seg = 0, start = 0;
    while (seg < 3)
    {
        int bar = command.indexOf('|', start);
        if (bar == -1)
        {
            parts[seg++] = command.substring(start);
            break;
        }
        parts[seg++] = command.substring(start, bar);
        start = bar + 1;
    }
    while (seg < 3)
        parts[seg++] = "";

    String name = parts[0];
    int id = parts[1].length() ? parts[1].toInt() : -1;
    int value = parts[2].length() ? parts[2].toInt() : -1;

    Serial.println("Name: " + name + ", ID: " + String(id) + ", Value: " + String(value)); // Debug

    if (name == "liquid")
    {
        /*if (id >= 0)
            liqdis.id = id;
        if (value >= 0)
            liqdis.vol = value;*/
        //liqdis.start();
        //liqdis.stop();

        // Send command to slave via Serial2
        String slaveCommand = "MODULE:" + command;
        Serial2.println(slaveCommand+"\n");
        Serial2.flush(); // Ensure data is sent

        Serial.println("Status: liquid Done");
    }
    else if (name == "spice")
    {
        if (id >= 0)
            spicedis.id = id;
        if (value >= 0)
            spicedis.weight = value;
        spicedis.start();
        Serial.println("Status: spice Done");
    }
    else if (name == "hopper")
    {
        if (id >= 0)
            hopper.id = id;
        if (value >= 0)
            hopper.weight = value;

        // Send command to slave via Serial2
        String slaveCommand = "MODULE:" + command;
        Serial2.println(slaveCommand+"\n");
        Serial2.flush(); // Ensure data is sent

        Serial.println("Status: hopper Done");
    }
    else if (name == "clean")
    {
        //clean.start();
        Serial.println("Status: clean Done");
    }
    else if (name == "stirrer")
    {
        //stirrer.start();
        // Remove or reduce delay to prevent blocking
        delay(5000);
        //stirrer.stop();
        Serial.println("Status: stirrer Done");
    }
    else if (name == "chop")
    {
        //chop.start();
        Serial.println("Chopping...");
        delay(5000);
        Serial.println("Status: chop Done");
    }
    else if (name == "steam")
    {   
        Serial.println("Steaming...");
        delay(5000);
        Serial.println("Status: steam Done");
    }
    else
    {
        Serial.println("Unknown Module: " + name);
        Serial.println("Status: Error");
    }
}
