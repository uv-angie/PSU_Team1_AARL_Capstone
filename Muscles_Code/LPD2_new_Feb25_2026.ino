//Real-time Length Sensing
//By: Rochelle Jubert
//Editted by: Elijah Baros on February 25th 2026

/* 
I combined Leo's debugging code to Rochelle's old code. This new code basically 
will inflate the muscle if an 'a' is pressed, and it will deflate the muscle
if a 'b is pressed. It also serially prints the data it collects about strain, 
pressure, displacement, strain displacement, inlet pwm signal, outlet pwm signal, 
and a timestamp.
*/


#include <math.h>

// Assigning Linear Potentiometer to Analog Input A0, pressure sensor to A2, and solenoid valve to 7
 // const int pot = A0; 
  //const int psensor = A2; 
 // const int straingauge = A10; //changed to A10 from A5
  //float strainval[2];
  //float strainfiltval;
 // int solenoidPin = 10;
  // #define PWM_PIN 6
  // #define solenoidPin 10
  // int pwmValue = 0 ;
   int ipwmValue = 0;// inlet PWM value
   int epwmValue = 0;// outlet PWM value
  // int pinState = LOW;
  // unsigned long previousMillis = 0;
  // const long interval = 1000;
  // const long rampDuration = 4000;


//From Leo's debug code
////////////////////////////////////////////////////
const int pot     = A5;     // position sensor pin (linear pot)
const int ivalvePin   = 1;      // PWM pin for contraction (inlet) 
const int evalvePin   = 2;      // PWM pin for extension (outlet)          
const int straingauge = A10;      // Liquid Wire Pin
const int PWM_start   = 105;    // starting PWM for sweep
const int PWM_max     = 255;    // max PWM
const int PWM_inc     = 3;      // PWM increment
const int numPoints   = 100;    // number of percentage thresholds (1% increments)
const uint32_t fallbackTimeout = 3000;  // ms to wait per point before fallback
const int psensor = A2; // pressure pin to measure internal pressure

double Len[numPoints];     // actual position thresholds

double posMax = 0;         // sensor value at fully contracted
double posMin = 0;         // sensor value at fully extended
float pressure = 0.0 ;        // pressure sensor value
float PSIpressure = 0.0 ;        // pressure sensor value
float LW = 0.0;    // Liquid Wire Sensor value
int Lpot_Out = 0; // output of linear potentiometer
/////////////////////////////////////////////////////////////////


//Pulse Information

  // int delayBetweenPulses = 1000; // Initial delay between pulses in milliseconds, starting at 500ms
  // const int pulseWidth = 35;    // Width of each pulse in milliseconds
  // bool isPumpingFirst = false;  // Flag indicating whether the pump is currently active for the first pulse
  // bool isPumpingSecond = false; // Flag indicating whether the pump is currently active for the second pulse
  // unsigned long delayZeroTime = 0; // Timestamp when delay between pulses first reaches zero
  // bool continueRunning = true; // Flag to control the execution of the main loop













void setup() {

//Set PWM resolution to 12 bits (0-4095)
  // analogWriteResolution(12);
  // pinMode(PWM_PIN,OUTPUT);

// Establishing serial communication between Arduino and computer
 // Serial.begin(9600);

//Establish the solenoid pin as an OUTPUT
  // pinMode(solenoidPin, OUTPUT);


//From Leo's debug code
/////////////////////////////////////////////////////////////
    Serial.begin(9600);
    pinMode(ivalvePin, OUTPUT);
    pinMode(evalvePin, OUTPUT);

    // ensure actuator fully extended at start
    analogWrite(ivalvePin, 0);
    analogWrite(evalvePin, 255);
//////////////////////////////////////////////////////////////


}

















void loop() {

  //OPTION 1: Actuate the solenoid valve using the transistor (Binary ON/OFF Solenoid)
    // unsigned long currentMillis = millis();
    //   writeData();    

    //   if (currentMillis - previousMillis >= interval) {
    //     // save the last time you blinked the LED
    //       previousMillis = currentMillis;

    //     // if the LED is off turn it on and vice-versa:
    //       if (pinState == LOW) {
    //         pinState = HIGH;
    //       } else {
    //         pinState = LOW;
    //   }

    //   // set the LED with the ledState of the variable:
    //   digitalWrite(solenoidPin, pinState);
    // }

    //analogWrite(PWM_PIN, pwmValue);

  //OPTION 2: This is creating steps using the proportional solenoid valve
    // if (currentMillis - previousMillis >= interval) {
    //   previousMillis = currentMillis;
    
    //   // Increment the PWM value by 1024 (adjust as needed)
    //   pwmValue += 1024; //1024;

    //   // Ensure the PWM value stays within the acceptable range (0-4096)
    //   pwmValue = constrain(pwmValue, 0, 4096);

    //   analogWrite(PWM_PIN, pwmValue);
    // }
    

  //OPTION 3: Ramp code    
    // rampUp();
    // delay(20);  // Delay for 1 second between ramps
    // rampDown();
    // delay(20);

  //Option 4: Hold Up and Hold Down
    // holdup();
    // delay(1);
    // holddown();
    // delay(1);

  //Option 5: Ramp and Hold
    // rampUp();
    // delay(1);
    // holdup();
    // delay(1);
    // rampDown();
    // delay(1);
    // holddown();

  //OPTION 6: 2 valve configuration
    // unsigned long currentMillis = millis();
    // //writeData();

    //   if (currentMillis - previousMillis >= interval) {
    //     // save the last time you blinked the LED
    //       previousMillis = currentMillis;

    //     // if the LED is off turn it on and vice-versa:
    //       if (pinState == LOW) {
    //         pinState = HIGH;
    //         pwmValue = 0;
    //         // set the LED with the ledState of the variable:
    //           digitalWrite(solenoidPin, pinState);
    //           analogWrite(PWM_PIN, pwmValue);
    //       } else {
    //         pinState = LOW;
    //         pwmValue = 100;
    //         // set the LED with the ledState of the variable:
    //           digitalWrite(solenoidPin, pinState);
    //           analogWrite(PWM_PIN, pwmValue);
    


     //}

  //Option 7: Pulsing
    // unsigned long currentMillis = millis();
    // unsigned long timeSinceLastIntervalStart = currentMillis - previousMillis; // Time since the last interval start
    // writeData();

    // if (!continueRunning) {
    // return; // Exit the loop if the continueRunning flag is false
    // }

    // // Pulse control logic for the first pulse
    //   if (timeSinceLastIntervalStart < pulseWidth && !isPumpingFirst) {
    //     digitalWrite(solenoidPin, HIGH); // Activate the pump for the first pulse
    //     isPumpingFirst = true;
    //   } else if (timeSinceLastIntervalStart >= pulseWidth && isPumpingFirst) {
    //     digitalWrite(solenoidPin, LOW); // Deactivate the pump after the first pulse
    //     isPumpingFirst = false;
    //   }

    // // Pulse control logic for the second pulse
    //   if (timeSinceLastIntervalStart >= delayBetweenPulses && timeSinceLastIntervalStart < delayBetweenPulses + pulseWidth && !isPumpingSecond) {
    //     digitalWrite(solenoidPin, HIGH); // Activate the pump for the second pulse
    //     isPumpingSecond = true;
    //   } else if (timeSinceLastIntervalStart >= delayBetweenPulses + pulseWidth && isPumpingSecond) {
    //     digitalWrite(solenoidPin, LOW); // Deactivate the pump after the second pulse
    //     isPumpingSecond = false;
    //   }

    // // Adjust delay between pulses at the end of the interval
    //   if (timeSinceLastIntervalStart >= interval) {
    //     previousMillis = currentMillis; // Reset the interval timer for the next cycle

    // // Adjust delay between pulses at the end of the interval
    //   if (timeSinceLastIntervalStart >= interval) {
    //   previousMillis = currentMillis; // Reset the interval timer for the next cycle

    //   // Adjust delay between pulses according to the specified logic
    //     if (delayBetweenPulses >= 1000) {  //for range of > 1000 , reduce by 100ms per interval
    //       delayBetweenPulses -= 100;
    //     } else if (delayBetweenPulses >= 500) { //for range of 1000 to 500ms, reduce by 50ms per interval
    //       delayBetweenPulses -= 50;
    //     } else if (delayBetweenPulses >= 250) { //for range of 500 to 250ms, reduce by 25ms per interval
    //       delayBetweenPulses -= 25;
    //     } else if (delayBetweenPulses >= 100) { //for range of 250 to 100ms, reduce by 15ms per interval
    //       delayBetweenPulses -= 15;
    //     } else if (delayBetweenPulses > 0) {    //for range of 100 to 0ms, reduce by 10ms per interval
    //       delayBetweenPulses -= 10;
    //     }

    //   // Ensure delayBetweenPulses doesn't go below 0
    //     if (delayBetweenPulses < 0) {
    //       delayBetweenPulses = 0;
    //     }

    //   // Record the time when delay between pulses first reaches zero
    //     if (delayBetweenPulses <= 25 && delayZeroTime == 0) {
    //       delayZeroTime = currentMillis;
    //     }

    //   // Check if we should stop running after a certain time with zero delay
    //     if (delayZeroTime > 0 && (currentMillis - delayZeroTime >= 0)) {
    //       continueRunning = false; // Stops the loop after 4 seconds of zero delay
    //     }
    //   }
    
    //   }

  //Option 8: Random PWM Signal
    // unsigned long startTime = millis();
    // unsigned long currentTime;
    // int currentValue = 0;
    // int targetValue = 0;

    // while ((currentTime = millis()) - startTime < 40000) {
    //   // Generate a random target value
    //   targetValue = random(4095 + 1);
    
    //   // Ramp up or down to the target value
    //     while (currentValue != targetValue) {
    //       if (currentValue < targetValue) {
    //         currentValue +=120;
    //         if (currentValue > targetValue) {
    //           currentValue = targetValue;
    //         }
    //       } else {
    //         currentValue-=120;
    //         if (currentValue < targetValue){
    //           currentValue = targetValue;
    //         }
    //       }
    //       analogWrite(PWM_PIN, currentValue);
    //       writeData();
    //       delay(1); // adjust the speed of the ramp
    //     }
    
    //   // Wait for a random amount of time before changing the target value
    //     delay(random(10, 100)); // adjust the range of delay
    // }

  //Option 9: Ramp Air + Quick Release
  //Have proportional valve in line with solenoid to release
    // delay(1);
    // rampUp();
    // delay(1);
    // open();

  //Option 10: Quick Fill + Ramp Release
  //Have solenoid in line with proportional valve to release, change rampUp mapping to 200, 4095 (adjust as needed)
    // open2();
    // delay(1);    
    // rampUp();
    // delay(1);


//This is from Leo's debug code, but editted
///////////////////////////////////////////////////////////////

if (Serial.available()) {
  char c = Serial.read();
  if (c == 'a') {
    ipwmValue = 255;
    epwmValue = 0;
    analogWrite(ivalvePin, ipwmValue);
    analogWrite(evalvePin, epwmValue);
    }

  if (c == 'b') {
    ipwmValue = 0;
    epwmValue = 255;
    analogWrite(ivalvePin, ipwmValue);
    analogWrite(evalvePin, epwmValue);
    }
} 

// pressure = analogRead(psensor) ; //Measure analog value
// Lpot_Out = analogRead(pot); 
// LW = analogRead(straingauge);
// LW = LW / 1023 * 3.3; //changed the 5 to a 4, might need to change back
// pressure = pressure/1024 * 5; //Convert analog value to voltage, 
// pressure = ((pressure / 5) -0.04)/ 0.0012858 ; //Convert to kPa
// PSIpressure = (pressure * 0.145);
// Serial.print(pot);
// Serial.print(" bits  ");
// Serial.print(pressure);
// Serial.print(" Kpa  ");
// Serial.print(PSIpressure); // in psi
// Serial.println(" Psi");
// Serial.print(LW); // Input LiquidWire
// Serial.println(" V");

writeData(ipwmValue, epwmValue);
 delay(1000);

//////////////////////////////////////////////////////////////


// void open() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= interval) {
//     pwmValue = 0;
//     digitalWrite(solenoidPin, HIGH); 
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
    
//   }
//     digitalWrite(solenoidPin, LOW);  // Ensure PWM is off at the end
// }

// void open2() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= interval) {
//     digitalWrite(PWM_PIN, 0);
//     digitalWrite(solenoidPin, HIGH); 
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
    
//   }
//    digitalWrite(solenoidPin, LOW);  // Ensure PWM is off at the end
// }

// void rampUp() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= rampDuration) {
//     pwmValue = map(currentTime - startTime, 0, rampDuration, 200, 4095);
//     analogWrite(PWM_PIN, pwmValue);
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
//   }

//   analogWrite(PWM_PIN, 0);  // Ensure PWM is off at the end
// }

// void rampDown() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= rampDuration) {
//     pwmValue = map(currentTime - startTime, 0, rampDuration, 4095, 0);
//     analogWrite(PWM_PIN, pwmValue);
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
//   }

//   analogWrite(PWM_PIN, 0);  // Ensure PWM is off at the end
// }

// void holdup() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= interval) {
//     pwmValue = 4095;
//     analogWrite(PWM_PIN, pwmValue);
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
//   }

//   analogWrite(PWM_PIN, 0);  // Ensure PWM is off at the end
// }

// void holddown() {
//   unsigned long startTime = millis();
//   unsigned long currentTime;
//   int pwmValue;

//   while ((currentTime = millis()) - startTime <= interval) {
//     pwmValue = 0;
//     analogWrite(PWM_PIN, pwmValue);
//     writeData(pwmValue);
//     delay(10);  // Adjust delay as needed for smoother ramp
//   }

//   analogWrite(PWM_PIN, 0);  // Ensure PWM is off at the end
// }















void writeData(int ipwmValue, int epwmValue) {

  //Generate timestamp
    long timestamp = millis();

  // Assign read potentiometer value to variable analogValue and pressure value to pValue
    float potValue = analogRead(pot);
    float pValue = analogRead(psensor);

  // Use H-diagram to interpolate and establish ratio between displacement and potentiometer reading
    //float displacement = potValue*0.0993157; old calibration
    float displacement = potValue*0.1013782991; //new calibration, might need to change for LW Capstone
    float pressure = pValue*0.1084 - 2.1067; // might need new calibration for LW Capstone


//Editted, but from Leo's debug code
////////////////////////////////////////////

// pressure = analogRead(psensor) ; //Measure analog value
// pressure = pressure/1024 * 5; //Convert analog value to voltage, 
// pressure = ((pressure / 5) -0.04)/ 0.0012858 ; //Convert to kPa
// PSIpressure = (pressure * 0.145);
//maybe this calibration will be more accurate?????????????????

/////////////////////////////////////////////


  //Calibrate strain gauge reading
    float strainvoltage = analogRead(straingauge);

  //Calculate strain gauge values
    //strainval[0] = strainval[1];
    //strainval[1] = strainval[2];
		//strainval[1] = (2.452372752527856026e-1 * strainvoltage) + (0.50952544949442879485 * strainval[0]); //+ (-0.95099351006330667957 * strainval[1]);

    //strainfiltval = (strainval[0] + strainval[1]); //+ 2.000000 * strainval[1];
    //float straindisp = strainvoltage * 0.25547 - 159.203; old calibration before adding table top power supply
    float straindisp = strainvoltage * 0.066573 + 41.652; //might need to change for LW Capstone
    //float straindisp2 = straindisp * 0.973 + 1.856;

    
    if (timestamp<40000) {
      
      Serial.print(displacement);
      Serial.print(", ");
      Serial.print(strainvoltage);
      Serial.print(", ");
      Serial.print(straindisp);
      Serial.print(", ");
      Serial.print(ipwmValue); //added i and e to create two variables since there are two valves
      Serial.print(", ");
      Serial.print(epwmValue); //added i and e to create two variables since there are two valves
      Serial.print(", ");
      Serial.print(pressure);
      Serial.print(", ");
      Serial.println(timestamp);
      
      //Serial.println("   done.");

    // Logs data every 10 milliseconds 
      delay(10);}


}
