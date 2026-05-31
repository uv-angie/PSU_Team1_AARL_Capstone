/*
This code simply increments the wiper value of the 
DS3502 10k potentiometer from 0 to 127 repeatedly

ELijah Baros
5/30/2026

For God hath not given us the spirit of fear; but 
of power, and of love, and of a strong mind.
2 Timothy 1:7
*/

#include <Wire.h>

#define DS3502_ADDR 0x28

	const int signal_pin = A17; // Pin 41


	void setup()
	{		
	  Serial.begin(9600);
	  pinMode(signal_pin, INPUT); 
		Wire.begin();
		delay(1000);
	}
	

	void loop()
	{
		for(int i = 0; i < 128 ; i++){
			Wire.beginTransmission(DS3502_ADDR);
  		Wire.write(0x00);   // Wiper Register
  		Wire.write(i);      // wiper value
  		Wire.endTransmission();
			delay(100);
	  	int val = analogRead(signal_pin);// Read analog value
			// Serial.print("Wiper: ");
			// Serial.print(i);
			// Serial.print("| Voltage: ");
			Serial.println(val);
			delay(100);
		}
	}