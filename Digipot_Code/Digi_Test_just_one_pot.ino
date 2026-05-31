/*
This code constantely reads the value when the 
DS3052 10k digital potentiometer is set to a 
specific value.

Elijah Baros
5/30/2026

It's the destination, its the friends we 
ran over along the way.
*/
#include <Wire.h>

#define DS3502_ADDR 0x28
int i = 90;

const int po4 = A3;


int raw4;


float voltage4;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  Wire.beginTransmission(DS3502_ADDR);
  Wire.write(0x00);   // Wiper Register
  Wire.write(i);      // wiper value
  Wire.endTransmission();  
  delay(1000);

  Serial.println("Starting DS3502 test...");
}

void loop() {
    // Read analog voltages
    raw4 = analogRead(po4);

    // Convert ADC counts to voltages
    voltage4 = (raw4 * 3.3) / 1023.0;


    Serial.print(i);
    Serial.print(", ");

    Serial.println(voltage4, 4);


    delay(100);
 // }

  Serial.println("----------------");
}