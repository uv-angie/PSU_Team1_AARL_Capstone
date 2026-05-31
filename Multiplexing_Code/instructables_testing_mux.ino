/*
This code increments through the even channels of the 
CD74HC4067 16 channel multiplexer

Elijah Baros
5/30/2026

And I say unto you, Ask, and it shall be given you;
seek, and ye shall find; knock, and it shall be 
opened unto you.
Luke 11:9
*/
	

	#include <CD74HC4067.h>
	

	            // s0 s1 s2 s3: select pins
	CD74HC4067 mux(4, 5, 6, 7);  // create a new CD74HC4067 object with its four select lines - 8,9,10,11
	

	const int signal_pin = A10; // Pin A0 - Connected to Sig pin of CD74HC4067
	

	void setup()
	{
	    Serial.begin(9600);
	    pinMode(signal_pin, INPUT); // Set as input for reading through signal pin
	}
	

	void loop()
	{
		int m = 1; //sensor label
	  // loop through channels 0 - 15
	    for (int i = 0; i < 16; i = i + 2) {
			
	      mux.channel(i);
			 	
				for(int j = 0; j < 100; j++){
	      	float val = analogRead(signal_pin);                       // Read analog value
					val = val / (5 * 1023) * 3.3; //Convert to voltage
	        Serial.print(m);
					Serial.print(", ");
					Serial.println(val);
					//Serial.println("Channel "+String(i)+": "+String(val));  // Print value
			 		delay(100);
			}
			m++;//increment sensor value
	 }
	 
	//mux.channel(0);
	//delay(100);
	}