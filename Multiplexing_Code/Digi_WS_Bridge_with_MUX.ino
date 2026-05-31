/*
This code sets the DS3502 10 Digital potetiometer
to one of the wiper values and sets the CD74HC4067
multiplexer to one specific channel and reads an analog value.

Elijah Baros
5/30/2026

vroom vroom
*/
	

#include <CD74HC4067.h>
#include <Wire.h>

#define DS3502_ADDR 0x28

	            // s0 s1 s2 s3: select pins
	CD74HC4067 mux(4, 5, 6, 7);  // create a new CD74HC4067 object with its four select lines - 8,9,10,11
	

	const int signal_pin = A11; // Pin 25 - C
	

	void setup()
	{
		
	  Serial.begin(9600);
	  pinMode(signal_pin, INPUT); // Set as input for reading through signal pin

		// Set the Digital Potentiometers to the desired resistance 
		// Resistance will be equal to 78.125 multiplied by the wiper value in the Wire.write(); statement
		Wire.begin();
		Wire.beginTransmission(DS3502_ADDR);
  	Wire.write(0x00);   // Wiper Register
  	Wire.write(1);      // wiper value
  	Wire.endTransmission();
	}
	

	void loop()
	{
		int m = 1; //sensor label
	  // loop through channels 0 - 15
	    //for (int i = 0; i < 16; i = i + 2) {
			int i = 0; //remove if I put for loop back
	      mux.channel(i);
			 	
				for(int j = 0; j < 200; j++){
	      	float val = analogRead(signal_pin);                       // Read analog value
					//val = val / (5 * 1023) * 2.5; //Convert to voltage
	        Serial.print(m);
					Serial.print(", ");
					Serial.println(val);
					//Serial.println("Channel "+String(i)+": "+String(val));  // Print value
			 		delay(100);
			}
			//m++;//increment sensor value
	// }
	 
	//mux.channel(0);
	//delay(100);
	}