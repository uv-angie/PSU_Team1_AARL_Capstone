/*
 *  CD74HC4067 "Loop" example used for the basic MUX structure.
 *  https://www.instructables.com/Arduino-Interfacing-With-CD74HC4067-16-channel-MUX/

 This code switches between printing when connected to channel 0 and 1 of the 
 CD74HC4067 16 channel multiplexer.

 Elijah Baros
5/30/2026

For I know the thoughts that I think toward you, saith the
LORD, thoughts of peace, and not of evil, to give you an 
expected end.
Jeremiah 29:11
 */

#include "CD74HC4067.h" // multiplexer library.

const int num_lw_sensor = 2;            // how many liquidwire sensors are there?

const int lw1 = 0;
const int lw2 = 1;


float LW = 0.0;
// --- Arrays for polling order (order used by Python) ---
              //  s0 s1 s2 s3
CD74HC4067 lw_mux(4, 5, 6, 7);  // create a new CD74HC4067 object, UPDATE control pins with available pins
const int lw_pin = A10; //UPDATE later with the actual pin




int raw_data; // raw data from serial.

void setup() {
    Serial.begin(9600); // Initialize serial
    //Serial.setTimeout(1); // Serial timeout

    pinMode(lw_pin, INPUT); // set the initial mode of the common pin
}


void loop() {

 // if (Serial.available()) {
    // raw_data = Serial.read(); 
    

    //   raw_data = Serial.read();
    for(int i=0; i<100;i++){ 
      lw_mux.channel(lw1);
      analogRead(lw_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
      LW = analogRead(lw_pin);///1023 * 2.5;
      Serial.print("Channel: ");
      Serial.print(lw1);
      Serial.print(": ");
      Serial.print(LW);
      Serial.println();
      //Serial.println(" V");
      delay(100);
    }
    for(int i=0; i<100;i++){ 
      //raw_data = Serial.read(); 
      lw_mux.channel(lw2);
      analogRead(lw_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
      LW = analogRead(lw_pin);///1023 * 2.5;
      Serial.print("Channel ");
      Serial.print(lw2);
      Serial.print(": ");
      Serial.print(LW);
      Serial.println();
      //Serial.println(" V");
      delay(100);
    }


    // analogRead(lw_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
    // LW = analogRead(lw_pin)/1023 * 2.5;
    // Serial.print(LW);
    // Serial.println(" V");
    // delay(100);
    
 // }
}