#include "Module.h"
#include "HX711.h"
#include "globals.h"
#include "Wire.h"
HX711 scale;
float calibration_factor = -7050;


//I2C SETUP
#define I2C_SDA_PIN 0 // change to your chosen SDA pin
#define I2C_SCL_PIN 1   // change to your chosen SCL pin
#define SLAVE_ADDR  0x28


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
    Serial.begin(115200);                      // pc
    Serial1.begin(115200, SERIAL_8N1, 18, 17); // display


    //I2c setup for slave MCU
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    Wire.setClock(100000);

    // stirrer
    pinMode(ENA, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);

    // spice dispenser
    pinMode(DIR_PIN, OUTPUT);
    pinMode(STEP_PIN, OUTPUT);
    pinMode(IN1_PIN, OUTPUT);
    pinMode(IN2_PIN, OUTPUT);
    pinMode(IN3_PIN, OUTPUT);
    pinMode(IN4_PIN, OUTPUT);
    pinMode(ENA_PIN, OUTPUT);
    pinMode(ENB_PIN, OUTPUT);

    // hopper motor
    pinMode(ENC_PIN, OUTPUT);
    pinMode(INC1_PIN, OUTPUT);
    pinMode(INC2_PIN, OUTPUT);

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(calibration_factor);
    scale.tare();

    // Set pump pins as OUTPUT
    pinMode(PUMP1_PIN, OUTPUT);
    pinMode(PUMP2_PIN, OUTPUT);
    pinMode(PUMP3_PIN, OUTPUT);
    pinMode(PUMP4_PIN, OUTPUT);
    pinMode(PUMP5_PIN, OUTPUT);
    pinMode(PUMP6_PIN, OUTPUT);
    pinMode(PUMP7_PIN, OUTPUT);

    // Ensure all pumps are OFF initially
    digitalWrite(PUMP1_PIN, LOW);
    digitalWrite(PUMP2_PIN, LOW);
    digitalWrite(PUMP3_PIN, LOW);
    digitalWrite(PUMP4_PIN, LOW);
    digitalWrite(PUMP5_PIN, LOW);
    digitalWrite(PUMP6_PIN, LOW);
    digitalWrite(PUMP7_PIN, LOW);
}

void loop()
{
    if (Serial1.available() > 0)
    {   
        Wire.beginTransmission(SLAVE_ADDR);
        Serial.println("Serial1 available!");
        Serial1.setTimeout(100);
        inputString = Serial1.readString();
        Serial.println("After readString()");
        inputString.trim();
        stringcmp = true;
        if (inputString.length() > 0)
        {
            Serial.println(inputString); // Not being hit
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

void parseAndExecute(String command)
{
    if (command.startsWith("MODULE:"))
        command.remove(0, 7);

    int StepIndex = 0;
    command.trim();
    if (!command.endsWith(","))
        command += ',';

    int lastIndex = 0;
    while (true)
    {
        int nextIndex = command.indexOf(',', lastIndex);
        if (nextIndex == -1)
            break;

        String entry = command.substring(lastIndex, nextIndex);
        entry.trim();
        lastIndex = nextIndex + 1;
        if (entry.length() == 0)
            continue;

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
        while (seg < 3)
            parts[seg++] = "";

        String name = parts[0];
        int id = parts[1].length() ? parts[1].toInt() : -1;
        int value = parts[2].length() ? parts[2].toInt() : -1;

        if (name == "liquid")
        {
            if (id >= 0)
                liqdis.id = id;
            if (value >= 0)
                liqdis.vol = value;
            liqdis.start();
            liqdis.stop();
            Wire.write((const uint8_t *)inputString, strlen(inputString));
            Serial.println(String("Status: ") + StepIndex + " Done");
        }
        else if (name == "spice")
        {
            if (id >= 0)
                spicedis.id = id;
            if (value >= 0)
                spicedis.weight = value;
            spicedis.start();
            Serial.println(String("Status: ") + StepIndex + " Done");
        }
        else if (name == "hopper")
        {
            if (id >= 0)
                hopper.id = id;
            if (value >= 0)
                hopper.weight = value;
            hopper.start();
            Serial.println(String("Status: ") + StepIndex + " Done");
            Wire.write((const uint8_t *)inputString, strlen(inputString));
        }
        else if (name == "clean")
        {
            clean.start();
            Serial.println(String("Status: ") + StepIndex + " Done");
        }
        else if (name == "stirrer")
        {
            stirrer.start();
            delay(5000);
            stirrer.stop();
            Serial.println(String("Status: ") + StepIndex + " Done");
        }
        else
        {
            Serial.println("Unknown Module");
            Serial.println(String("Status: ") + StepIndex + " Error");
        }

        // Serial.println(String("Status: ")+StepIndex+" Done");
        StepIndex++;
    }

    Serial.println("complete");
}
