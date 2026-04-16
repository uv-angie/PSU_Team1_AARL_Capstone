#include <math.h>

const int LpotPin     = A5;     // position sensor pin (linear pot)
const int ivalvePin   = 11;      // PWM pin for contraction (inlet)
const int evalvePin   = 9;      // PWM pin for extension (outlet)
const int LwPin       = A10;      // Liquid Wire Pin
const int PWM_start   = 105;    // starting PWM for sweep
const int PWM_max     = 255;    // max PWM
const int PWM_inc     = 3;      // PWM increment
const int numPoints   = 100;    // number of percentage thresholds (1% increments)
const uint32_t fallbackTimeout = 3000;  // ms to wait per point before fallback
const int PressurePin = A2; // pressure pin to measure internal pressure

double Len[numPoints];     // actual position thresholds

double posMax = 0;         // sensor value at fully contracted
double posMin = 0;         // sensor value at fully extended
float pressure = 0.0 ;        // pressure sensor value
float PSIpressure = 0.0 ;        // pressure sensor value
float LW = 0.0;    // Liquid Wire Sensor value
int Lpot_Out = 0; // output of linear potentiometer


void setup() {
    Serial.begin(9600);
    pinMode(ivalvePin, OUTPUT);
    pinMode(evalvePin, OUTPUT);

    // ensure actuator fully extended at start
    analogWrite(ivalvePin, 0);
    analogWrite(evalvePin, 255);

}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'a') {
    analogWrite(ivalvePin, 255);
    analogWrite(evalvePin, 0);
      }

    if (c == 'b') {
    analogWrite(ivalvePin, 0);
    analogWrite(evalvePin, 255);
      }
  }

pressure = analogRead(PressurePin) ; //Measure analog value
Lpot_Out = analogRead(LpotPin); 
LW = analogRead(LwPin);
LW = LW / 1024 * 4; //changed the 5 to a 4, might need to change back
pressure = pressure/1024 * 5; //Convert analog value to voltage, 
pressure = ((pressure / 5) -0.04)/ 0.0012858 ; //Convert to kPa
PSIpressure = (pressure * 0.145);
//Serial.print(Lpot_Out);
//Serial.print(" bits  ");
// Serial.print(pressure);
// Serial.print(" Kpa  ");
//Serial.print(PSIpressure); // in psi
//Serial.println(" Psi");
Serial.print(LW); // Input LiquidWire
Serial.println(" V");
 delay(1000);
}


// This is a test edit
