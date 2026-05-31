/*
This code prints the calculated voltage 
when incrementing the wiper of the DS3502
10 digital potentiometer.

Elijah Baros
5/30/2026

I sometimes like the rain. Maybe it justifies
staying inside.
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
// int i = 50;
const int po1 = A0;
const int po2 = A1;
const int po3 = A2;
const int po4 = A3;

int raw1;
int raw2;
int raw3;
int raw4;

float voltage1;
float voltage2;
float voltage3;
float voltage4;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  
  delay(1000);

  Serial.println("Starting DS3502 test...");
}

void loop() {

  for (int i = 0; i <= 127; i++) {

    //Write new wiper value
  Wire.beginTransmission(DS3502_ADDR);
  Wire.write(0x00);   // Wiper Register
  Wire.write(i);      // wiper value
  Wire.endTransmission();

    // Allow voltage to settle
    delay(100);

    // Read analog voltages
    raw1 = analogRead(po1);
    raw2 = analogRead(po2);
    raw3 = analogRead(po3);
    raw4 = analogRead(po4);

    // Convert ADC counts to voltages
    voltage1 = (raw1 * 3.3) / 1023.0;
    voltage2 = (raw2 * 3.3) / 1023.0;
    voltage3 = (raw3 * 3.3) / 1023.0;
    voltage4 = (raw4 * 3.3) / 1023.0;
//First potentiometer
   // Serial.print("Digital Potentiometer 1 | Wiper: ");
    Serial.print(i);
    Serial.print(", ");
   // Serial.print(", ");

   // Serial.print("   Voltage: ");
    Serial.print(voltage1, 4);
    Serial.print(", ");
    //Serial.println(" V");

//Second potentiometer
    // Serial.print("Digital Potentiometer 2 | Wiper: ");
    // Serial.print(i);
    // Serial.print(", ");

    // Serial.print("   Voltage: ");
    Serial.print(voltage2, 4);
    Serial.print(", ");
    // Serial.println(" V");

//Third potentiometer
    // Serial.print("Digital Potentiometer 3 | Wiper: ");
    // Serial.print(i);
    // Serial.print(", ");
    // Serial.print("   Voltage: ");
    Serial.print(voltage3, 4);
    Serial.print(", ");
    // Serial.println(" V");

//Fourth potentiometer
    // Serial.print("Digital Potentiometer 4 | Wiper: ");
    // Serial.print(i);
    // Serial.print(", ");

    // Serial.print("   Voltage: ");
    Serial.println(voltage4, 4);
    // Serial.println(" V");


    delay(100);
  }

  Serial.println("----------------");
}