/*
This code increments the wiper value of the DS3502
10k digital potentiometer and prints the wiper value and
analog pin reading and converts it to a voltage

5/30/2026
Elijah Baros

Have a fantastic day!
I wonder if anyone will ever read these once I am gone
*/


// #include <Wire.h>

// #define DS3502_ADDR 0x28

// const int sense_pin = A0;
// float sense_val;

// void setup() {
//   Serial.begin(9600);
//   Wire.begin();

//   // Set wiper to 100
//   Wire.beginTransmission(DS3502_ADDR);
//   Wire.write(0x00);   // Wiper register
//   Wire.write(100);    // Value 0-127
//   Wire.endTransmission();
// }

// void loop() {
//   for (int i = 80; i < 120; i++){
//   Wire.beginTransmission(DS3502_ADDR);
//   Wire.write(0x00);   // Wiper register
//   Wire.write(i);    // Value 0-127
//   Wire.endTransmission();
//   sense_val = analogRead(sense_pin);
//   sense_val *= 3.3;
//   sense_val /= 1023;
//   Serial.print(sense_val);
//   Serial.println(" V");
//   delay(100);
//   }
// }



#include <Wire.h>

#define DS3502_ADDR 0x28

const int sense_pin = A0;

int sense_raw;
float sense_voltage;

void setup() {
  Serial.begin(9600);

  Wire.begin();

  delay(1000);

  Serial.println("Starting DS3502 test...");
}

void loop() {

  for (int i = 0; i <= 127; i++) {

    // Write new wiper value
  Wire.beginTransmission(DS3502_ADDR);
  Wire.write(0x00);   // Wiper Register
  Wire.write(i);      // wiper value
  Wire.endTransmission();

    // Allow voltage to settle
    delay(5);

    // Read analog voltage
    sense_raw = analogRead(sense_pin);

    // Convert ADC counts to voltage
    sense_voltage = (sense_raw * 3.3) / 1023.0;

    //Serial.print("Wiper: ");
    Serial.print(i);
    Serial.print(", ");

    //Serial.print("   ADC: ");
    Serial.print(sense_raw);
    Serial.print(", ");

    //Serial.print("   Voltage: ");
    Serial.println(sense_voltage, 4);
    //Serial.println(" V");

    delay(100);
  }

  Serial.println("----------------");
}