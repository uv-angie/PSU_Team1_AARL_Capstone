/*
 *  CD74HC4067 "Loop" example used for the basic MUX structure.
 *  https://www.instructables.com/Arduino-Interfacing-With-CD74HC4067-16-channel-MUX/
 *  Modified to include Liquid Wire and Strain Gauge sensors - Elijah Baros - 5/30/2026
 */

#include "CD74HC4067.h" // multiplexer library.
#include "Wire.h" //  I2C library

const int num_potentiometer = 12;        // how many potentiometers are there?
const int num_press_sensor = 24;         // and how many pressure sensors?
const int num_lw_sensor = 24;            // how many liquidwire sensors?
const int num_sg_sensor = 24;            // how many strain gauge sensors are there?



//const int data_length = num_potentiometer + num_press_sensor;
const int data_length = num_potentiometer + num_press_sensor + num_lw_sensor + num_sg_sensor; //added the liquidwire sensors
uint8_t sensor_data[data_length];       // storage array for all sensor data.

// --- Logical-to-physical mapping for potentiometers (joint angles) ---
// Order: ['L_hip_joint', 'L_knee_joint', 'L_ankle_joint', 
//         'R_hip_joint', 'R_knee_joint', 'R_ankle_joint', 
//         'L_scapula_joint', 'L_shoulder_joint', 'L_wrist_joint', 
//         'R_scapula_joint', 'R_shoulder_joint', 'R_wrist_joint']

const int L_scapula_joint   = 0;
const int L_shoulder_joint  = 1;
const int L_wrist_joint     = 2;
const int R_scapula_joint   = 3;
const int R_shoulder_joint  = 4;
const int R_wrist_joint     = 5;
const int L_hip_joint       = 6;
const int L_knee_joint      = 7;
const int L_ankle_joint     = 8;
const int R_hip_joint       = 9;
const int R_knee_joint      = 10;
const int R_ankle_joint     = 11;


const int L_scapula_joint_ext_muscle  = 19;
const int L_scapula_joint_flx_muscle  = 18;
const int L_shoulder_joint_ext_muscle = 3;
const int L_shoulder_joint_flx_muscle = 2;
const int L_wrist_joint_ext_muscle    = 4;
const int L_wrist_joint_flx_muscle    = 5;

const int R_scapula_joint_ext_muscle  = 13;
const int R_scapula_joint_flx_muscle  = 12;
const int R_shoulder_joint_ext_muscle = 8;
const int R_shoulder_joint_flx_muscle = 9;
const int R_wrist_joint_ext_muscle    = 7;
const int R_wrist_joint_flx_muscle    = 6;

const int L_hip_joint_ext_muscle      = 1;
const int L_hip_joint_flx_muscle      = 0;
const int L_knee_joint_ext_muscle     = 21;
const int L_knee_joint_flx_muscle     = 20;
const int L_ankle_joint_ext_muscle    = 22;
const int L_ankle_joint_flx_muscle    = 23;

const int R_hip_joint_ext_muscle      = 10;
const int R_hip_joint_flx_muscle      = 11;
const int R_knee_joint_ext_muscle     = 15;
const int R_knee_joint_flx_muscle     = 14;
const int R_ankle_joint_ext_muscle    = 16;
const int R_ankle_joint_flx_muscle    = 17;


const int L_scapula_joint_ext_muscle_lw   = 0;
const int L_scapula_joint_flx_muscle_lw   = 1;
const int L_shoulder_joint_ext_muscle_lw  = 2;
const int L_shoulder_joint_flx_muscle_lw  = 3;
const int L_wrist_joint_ext_muscle_lw     = 4;
const int L_wrist_joint_flx_muscle_lw     = 5;

const int R_scapula_joint_ext_muscle_lw   = 6;
const int R_scapula_joint_flx_muscle_lw   = 7;
const int R_shoulder_joint_ext_muscle_lw  = 8;
const int R_shoulder_joint_flx_muscle_lw  = 9;
const int R_wrist_joint_ext_muscle_lw     = 10;
const int R_wrist_joint_flx_muscle_lw     = 11;

const int L_hip_joint_ext_muscle_lw       = 12;
const int L_hip_joint_flx_muscle_lw       = 13;
const int L_knee_joint_ext_muscle_lw      = 14;
const int L_knee_joint_flx_muscle_lw      = 15;
const int L_ankle_joint_ext_muscle_lw     = 16;
const int L_ankle_joint_flx_muscle_lw     = 17;

const int R_hip_joint_ext_muscle_lw       = 18;
const int R_hip_joint_flx_muscle_lw       = 19;
const int R_knee_joint_ext_muscle_lw      = 20;
const int R_knee_joint_flx_muscle_lw      = 21;
const int R_ankle_joint_ext_muscle_lw     = 22;
const int R_ankle_joint_flx_muscle_lw     = 23;


const int L_scapula_joint_ext_muscle_sg   = 0;
const int L_scapula_joint_flx_muscle_sg   = 1;
const int L_shoulder_joint_ext_muscle_sg  = 2;
const int L_shoulder_joint_flx_muscle_sg  = 3;
const int L_wrist_joint_ext_muscle_sg     = 4;
const int L_wrist_joint_flx_muscle_sg     = 5;

const int R_scapula_joint_ext_muscle_sg   = 6;
const int R_scapula_joint_flx_muscle_sg   = 7;
const int R_shoulder_joint_ext_muscle_sg  = 8;
const int R_shoulder_joint_flx_muscle_sg  = 9;
const int R_wrist_joint_ext_muscle_sg     = 10;
const int R_wrist_joint_flx_muscle_sg     = 11;

const int L_hip_joint_ext_muscle_sg       = 12;
const int L_hip_joint_flx_muscle_sg       = 13;
const int L_knee_joint_ext_muscle_sg      = 14;
const int L_knee_joint_flx_muscle_sg      = 15;
const int L_ankle_joint_ext_muscle_sg     = 16;
const int L_ankle_joint_flx_muscle_sg     = 17;

const int R_hip_joint_ext_muscle_sg       = 18;
const int R_hip_joint_flx_muscle_sg       = 19;
const int R_knee_joint_ext_muscle_sg      = 20;
const int R_knee_joint_flx_muscle_sg      = 21;
const int R_ankle_joint_ext_muscle_sg     = 22;
const int R_ankle_joint_flx_muscle_sg     = 23;



// --- Arrays for polling order (order used by Python) ---

const uint8_t mux_channels[num_potentiometer] = {
  L_hip_joint, L_knee_joint, L_ankle_joint, 
  R_hip_joint, R_knee_joint, R_ankle_joint, 
  L_scapula_joint, L_shoulder_joint, L_wrist_joint, 
  R_scapula_joint, R_shoulder_joint, R_wrist_joint
};

const uint8_t press_mux_channel[num_press_sensor] = {  
  R_hip_joint_ext_muscle, R_hip_joint_flx_muscle, 
  R_knee_joint_ext_muscle, R_knee_joint_flx_muscle, 
  R_ankle_joint_ext_muscle, R_ankle_joint_flx_muscle,
 
  L_hip_joint_ext_muscle, L_hip_joint_flx_muscle, 
  L_knee_joint_ext_muscle, L_knee_joint_flx_muscle, 
  L_ankle_joint_ext_muscle, L_ankle_joint_flx_muscle, 
  
  R_scapula_joint_ext_muscle, R_scapula_joint_flx_muscle, 
  R_shoulder_joint_ext_muscle, R_shoulder_joint_flx_muscle, 
  R_wrist_joint_ext_muscle, R_wrist_joint_flx_muscle, 
  
  L_scapula_joint_ext_muscle, L_scapula_joint_flx_muscle, 
  L_shoulder_joint_ext_muscle, L_shoulder_joint_flx_muscle, 
  L_wrist_joint_ext_muscle, L_wrist_joint_flx_muscle
};



const uint8_t lw_mux_channels[num_lw_sensor] = {
  R_hip_joint_ext_muscle_lw, R_hip_joint_flx_muscle_lw, 
  R_knee_joint_ext_muscle_lw, R_knee_joint_flx_muscle_lw, 
  R_ankle_joint_ext_muscle_lw, R_ankle_joint_flx_muscle_lw,
 
  L_hip_joint_ext_muscle_lw, L_hip_joint_flx_muscle_lw, 
  L_knee_joint_ext_muscle_lw, L_knee_joint_flx_muscle_lw, 
  L_ankle_joint_ext_muscle_lw, L_ankle_joint_flx_muscle_lw, 
  
  R_scapula_joint_ext_muscle_lw, R_scapula_joint_flx_muscle_lw, 
  R_shoulder_joint_ext_muscle_lw, R_shoulder_joint_flx_muscle_lw, 
  R_wrist_joint_ext_muscle_lw, R_wrist_joint_flx_muscle_lw, 
  
  L_scapula_joint_ext_muscle_lw, L_scapula_joint_flx_muscle_lw, 
  L_shoulder_joint_ext_muscle_lw, L_shoulder_joint_flx_muscle_lw, 
  L_wrist_joint_ext_muscle_lw, L_wrist_joint_flx_muscle_lw
};


const uint8_t sg_mux_channels[num_sg_sensor] = {
  R_hip_joint_ext_muscle_sg, R_hip_joint_flx_muscle_sg, 
  R_knee_joint_ext_muscle_sg, R_knee_joint_flx_muscle_sg, 
  R_ankle_joint_ext_muscle_sg, R_ankle_joint_flx_muscle_sg,
 
  L_hip_joint_ext_muscle_sg, L_hip_joint_flx_muscle_sg, 
  L_knee_joint_ext_muscle_sg, L_knee_joint_flx_muscle_sg, 
  L_ankle_joint_ext_muscle_sg, L_ankle_joint_flx_muscle_sg, 
  
  R_scapula_joint_ext_muscle_sg, R_scapula_joint_flx_muscle_sg, 
  R_shoulder_joint_ext_muscle_sg, R_shoulder_joint_flx_muscle_sg, 
  R_wrist_joint_ext_muscle_sg, R_wrist_joint_flx_muscle_sg, 
  
  L_scapula_joint_ext_muscle_sg, L_scapula_joint_flx_muscle_sg, 
  L_shoulder_joint_ext_muscle_sg, L_shoulder_joint_flx_muscle_sg, 
  L_wrist_joint_ext_muscle_sg, L_wrist_joint_flx_muscle_sg
};



               // s0  s1  s2  s3
CD74HC4067  mux(18, 17, 16, 15);  // create a new CD74HC4067 object with its four control pins
const int pot_pin = A0;          // will be A17 on Muscle Mutt

CD74HC4067 pressure_mux_1(23, 22, 21, 20);  // create a new CD74HC4067 object 
const int pressure_pin_1 = A5;              // read pin for the CD74HC4067

CD74HC4067 pressure_mux_2(31, 30, 29, 28);  // create a new CD74HC4067 object 
const int pressure_pin_2 = A13;             // read pin for the CD74HC4067

CD74HC4067 lw_sg_mux(11, 10, 9, 8);  // create a new CD74HC4067 object, UPDATE control pins with available pins
const int lw_sg_pin = A17;           // read pin for the CD74HC4067

const int lw_EN = 33;     //create liquid wire enable pin
const int lw_sg_EN = 34;  //create liquid wire and strain gauge enable pin
const int sg_EN = 35;     //create strain gauge enable pin





void setup() {
    Serial.begin(9600);   // Initialize serial
    Serial.setTimeout(1); // Serial timeout

    pinMode(pot_pin, INPUT);        // set the initial mode of the common pin.
    pinMode(pressure_pin_1, INPUT); // set the initial mode of the common pin.
    pinMode(pressure_pin_2, INPUT); // set the initial mode of the common pin.
    pinMode(lw_sg_pin, INPUT);      // set the initial mode of the common pin

    pinMode(lw_EN, OUTPUT);      // set the initial mode of enable pin
    pinMode(lw_sg_EN, OUTPUT);      // set the initial mode of enable pin
    pinMode(sg_EN, OUTPUT);      // set the initial mode of enable pin

    // turn off all muxes
    digitalWrite(lw_EN, HIGH);
    digitalWrite(lw_sg_EN, HIGH);
    digitalWrite(sg_EN, HIGH);
    
    analogReadResolution(10);

    // Wire2.begin();
    // Wire2.setClock(400000);

    // calibrateAll();


}

// Select through all multiplexor pins and update sensor data array
void read_all_joints() {
  // Poll potentiometers in logical order
  for (int i = 0; i < num_potentiometer; i++) {
    mux.channel(mux_channels[i]);
    analogRead(pot_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
    sensor_data[i] = analogRead(pot_pin)/4; // read potentiometer data, byte-sized
  }
  // Poll pressure sensors in logical order
  for (int i = 0; i < num_press_sensor; i++) {
    if (press_mux_channel[i] <= 11) {
        pressure_mux_1.channel(press_mux_channel[i]+2); // Add 2 due to MUX configuration
        analogRead(pressure_pin_1); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + i] = analogRead(pressure_pin_1)/4; // read byte-sized data
    }
    else if (press_mux_channel[i] > 11) {
        pressure_mux_2.channel(press_mux_channel[i]+2-12);
        analogRead(pressure_pin_2); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + i] = analogRead(pressure_pin_2)/4; // read byte-sized data
    }    
  }
  
  
  
  //  Poll Liquid Wire sensors in logical order

  for (int i = 0; i < num_lw_sensor; i++) {

    if (i<=15){//lw_mux_channels[i] <= 15) { 
        //  Enable Liquid Wire mux 
        
        digitalWrite(lw_sg_EN, HIGH);
        digitalWrite(sg_EN, HIGH);
        digitalWrite(lw_EN, LOW);

        lw_sg_mux.channel(i);//lw_mux_channels[i]); 
        analogRead(lw_sg_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + num_press_sensor + i] = analogRead(lw_sg_pin)/4; // read byte-sized data
    }
    else if (i>15){//lw_mux_channels[i] > 15) {
        //  Enable Liquid Wire mux 
        digitalWrite(lw_EN, HIGH);
        digitalWrite(sg_EN, HIGH);
        digitalWrite(lw_sg_EN, LOW);  
        lw_sg_mux.channel(i-16);//lw_mux_channels[i]-16);
        analogRead(lw_sg_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + num_press_sensor + i] = analogRead(lw_sg_pin)/4; // read byte-sized data
    }
  }    
  //Poll strain gauge sensors in logical order
  for (int i = 0; i < num_sg_sensor; i++) {
    if (i <= 15){//sg_mux_channels[i] <= 15) {
        //  Enable Liquid Wire mux 
        digitalWrite(lw_EN, HIGH);
        digitalWrite(lw_sg_EN, HIGH);
        digitalWrite(sg_EN, LOW);
        lw_sg_mux.channel(i);//sg_mux_channels[i]);
        analogRead(lw_sg_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + num_press_sensor + num_lw_sensor + i] = analogRead(lw_sg_pin)/4; // read byte-sized data
    }
    else if (i>15){//sg_mux_channels[i] > 15) {
        //  Enable Liquid Wire mux 
        digitalWrite(lw_EN, HIGH);
        digitalWrite(sg_EN, HIGH);
        digitalWrite(lw_sg_EN, LOW);
        
        lw_sg_mux.channel(i-8);//sg_mux_channels[i]-8);
        analogRead(lw_sg_pin); // Dummy read! Allows the Teensy ADC pin capacitor to charge
        sensor_data[num_potentiometer + num_press_sensor + num_lw_sensor + i] = analogRead(lw_sg_pin)/4; // read byte-sized data
    }  
  }

  // --- Printing (single dynamic row) ---
  for (int i = 0; i < data_length; i++) {
    Serial.printf("%4d", i); // fixed width: 4 spaces
  }
  Serial.print("\n"); // carriage return → overwrite same line
  for (int i = 0; i < data_length; i++) {
    Serial.printf("%4d", sensor_data[i]); // fixed width: 4 spaces
  }
  Serial.print("\n"); // carriage return → overwrite same line
}




// When requested over serial, read and send sensor data
void loop() {
      read_all_joints();
 
}