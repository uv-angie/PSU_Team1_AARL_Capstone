/*
For we  have not followed cunningly devised fables,
when we made known unto you the power and coming of
our Lord Jesus Christ, but were eyewitnesses of his
majesty.
2 Peter 1:16

This code repeatedly prints the linear potentiometer
value, pressure, and LW value as the proportional
valve is opened for an extended period of time and closed
for an extended period of time.

Elijah Baros

5/30/2026

*/
#include <math.h>

const int LpotPin     = A5;     // position sensor pin (linear pot)
const int ivalvePin   = 1;      // PWM pin for contraction (inlet)
const int evalvePin   = 2;      // PWM pin for extension (outlet)
const int LwPin       = A1;      // Liquid Wire Pin
const int PressurePin = A2; // pressure pin to measure internal pressure


float pressure = 0.0 ;        // pressure sensor value
float PSIpressure = 0.0 ;        // pressure sensor value
float LW = 0.0;    // Liquid Wire Sensor value
int Lpot_Out = 0; // output of linear potentiometer
int i = 0;

void setup() {
    Serial.begin(9600);
    pinMode(ivalvePin, OUTPUT);
    pinMode(evalvePin, OUTPUT);

    pinMode(LwPin, INPUT);

    // ensure actuator fully extended at start
    analogWrite(ivalvePin, 0);
    analogWrite(evalvePin, 255);

}

void loop() {
  // if (Serial.available()) {
  //   char c = Serial.read();
  //   if (c == 'a') {
  //   analogWrite(ivalvePin, 255);
  //   analogWrite(evalvePin, 0);
  //     }

  //   if (c == 'b') {
  //   analogWrite(ivalvePin, 0);
  //   analogWrite(evalvePin, 255);
  //     }
  // }


//////////////////////////////////////////////////
analogWrite(ivalvePin, 255);
analogWrite(evalvePin, 0);
/////////////////////////////////////////////////

for(i = 0; i<30 ; i++){
pressure = analogRead(PressurePin) ; //Measure analog value
Lpot_Out = analogRead(LpotPin); 
LW = analogRead(LwPin);



pressure = pressure/1023 * 5; //Convert analog value to voltage, 
pressure = ((pressure / 5) -0.04)/ 0.0012858 ; //Convert to kPa
PSIpressure = (pressure * 0.145);
LW = LW / 1023 * 2.5; //changed the 5 to a 4, might need to change back



Serial.print(Lpot_Out);
//Serial.print(" bits,  ");
Serial.print(",  ");
Serial.print(pressure);
//Serial.print(" Kpa,  ");
Serial.print(",  ");
Serial.print(PSIpressure); // in psi
//Serial.print(" Psi,  ");
Serial.print(",  ");
Serial.println(LW); // unfiltered input LiquidWire
//Serial.println(" V ");

delay(100);
//delay(10);
}



 ///////////////////////////////////////////////////////////////////

analogWrite(ivalvePin, 0);
analogWrite(evalvePin, 255);
  
for(i = 0; i<30 ; i++){
pressure = analogRead(PressurePin) ; //Measure analog value
Lpot_Out = analogRead(LpotPin); 
LW = analogRead(LwPin);



pressure = pressure/1023 * 5; //Convert analog value to voltage, 
pressure = ((pressure / 5) -0.04)/ 0.0012858 ; //Convert to kPa
PSIpressure = (pressure * 0.145);
LW = LW / 1023 * 2.5; //changed the 5 to a 4, might need to change back


Serial.print(Lpot_Out);
//Serial.print(" bits,  ");
Serial.print(",  ");
Serial.print(pressure);
//Serial.print(" Kpa,  ");
Serial.print(",  ");
Serial.print(PSIpressure); // in psi
//Serial.print(" Psi,  ");
Serial.print(",  ");
Serial.println(LW); // unfiltered input LiquidWire
//Serial.println(" V ");

delay(100);
}
//delay(1);

 ////////////////////////////////////////////
}


// This is a test edit