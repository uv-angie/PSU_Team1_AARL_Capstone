/*
 *  This code should be updated to the point where it will work with the digital 
    potentiometers, and it should calibrate before reading each strain gauge or
    liquid wire sensor.
 *  CD74HC4067 "Loop" example used for the basic MUX structure.
 *  https://www.instructables.com/Arduino-Interfacing-With-CD74HC4067-16-channel-MUX/
 *  Modified to include Liquid Wire and Strain Gauge sensors - Elijah Baros - 5/30/2026
 *
 *  LW and SG muxes share S0-S3 pins and use individual enable pins.
 *  Pot and pressure muxes are unchanged.
 */

#include "CD74HC4067.h"
#include "sensor_array.h"   // MuxArray + BridgeTuner (LW/SG only)
#include "capstone_api.h"

const int num_potentiometer = 12;
const int num_press_sensor  = 24;
const int num_lw_sensor     = 24;
const int num_sg_sensor     = 24;

const int data_length = num_potentiometer + num_press_sensor + num_lw_sensor + num_sg_sensor;
uint8_t sensor_data[data_length];

// ---------------------------------------------------------------------------
// Logical-to-physical channel maps — UNCHANGED
// ---------------------------------------------------------------------------

const int L_scapula_joint  = 0;
const int L_shoulder_joint = 1;
const int L_wrist_joint    = 2;
const int R_scapula_joint  = 3;
const int R_shoulder_joint = 4;
const int R_wrist_joint    = 5;
const int L_hip_joint      = 6;
const int L_knee_joint     = 7;
const int L_ankle_joint    = 8;
const int R_hip_joint      = 9;
const int R_knee_joint     = 10;
const int R_ankle_joint    = 11;

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

const int L_hip_joint_ext_muscle   = 1;
const int L_hip_joint_flx_muscle   = 0;
const int L_knee_joint_ext_muscle  = 21;
const int L_knee_joint_flx_muscle  = 20;
const int L_ankle_joint_ext_muscle = 22;
const int L_ankle_joint_flx_muscle = 23;

const int R_hip_joint_ext_muscle   = 10;
const int R_hip_joint_flx_muscle   = 11;
const int R_knee_joint_ext_muscle  = 15;
const int R_knee_joint_flx_muscle  = 14;
const int R_ankle_joint_ext_muscle = 16;
const int R_ankle_joint_flx_muscle = 17;

// LW channel indices (0-23, mapped inside MuxArray as sensor 0-23)
const int L_scapula_joint_ext_muscle_lw  = 0;
const int L_scapula_joint_flx_muscle_lw  = 1;
const int L_shoulder_joint_ext_muscle_lw = 2;
const int L_shoulder_joint_flx_muscle_lw = 3;
const int L_wrist_joint_ext_muscle_lw    = 4;
const int L_wrist_joint_flx_muscle_lw    = 5;
const int R_scapula_joint_ext_muscle_lw  = 6;
const int R_scapula_joint_flx_muscle_lw  = 7;
const int R_shoulder_joint_ext_muscle_lw = 8;
const int R_shoulder_joint_flx_muscle_lw = 9;
const int R_wrist_joint_ext_muscle_lw    = 10;
const int R_wrist_joint_flx_muscle_lw    = 11;
const int L_hip_joint_ext_muscle_lw      = 12;
const int L_hip_joint_flx_muscle_lw      = 13;
const int L_knee_joint_ext_muscle_lw     = 14;
const int L_knee_joint_flx_muscle_lw     = 15;
const int L_ankle_joint_ext_muscle_lw    = 16;
const int L_ankle_joint_flx_muscle_lw    = 17;
const int R_hip_joint_ext_muscle_lw      = 18;
const int R_hip_joint_flx_muscle_lw      = 19;
const int R_knee_joint_ext_muscle_lw     = 20;
const int R_knee_joint_flx_muscle_lw     = 21;
const int R_ankle_joint_ext_muscle_lw    = 22;
const int R_ankle_joint_flx_muscle_lw    = 23;

// SG channel indices (0-23, mapped inside MuxArray as sensor 24-47)
const int L_scapula_joint_ext_muscle_sg  = 0;
const int L_scapula_joint_flx_muscle_sg  = 1;
const int L_shoulder_joint_ext_muscle_sg = 2;
const int L_shoulder_joint_flx_muscle_sg = 3;
const int L_wrist_joint_ext_muscle_sg    = 4;
const int L_wrist_joint_flx_muscle_sg    = 5;
const int R_scapula_joint_ext_muscle_sg  = 6;
const int R_scapula_joint_flx_muscle_sg  = 7;
const int R_shoulder_joint_ext_muscle_sg = 8;
const int R_shoulder_joint_flx_muscle_sg = 9;
const int R_wrist_joint_ext_muscle_sg    = 10;
const int R_wrist_joint_flx_muscle_sg    = 11;
const int L_hip_joint_ext_muscle_sg      = 12;
const int L_hip_joint_flx_muscle_sg      = 13;
const int L_knee_joint_ext_muscle_sg     = 14;
const int L_knee_joint_flx_muscle_sg     = 15;
const int L_ankle_joint_ext_muscle_sg    = 16;
const int L_ankle_joint_flx_muscle_sg    = 17;
const int R_hip_joint_ext_muscle_sg      = 18;
const int R_hip_joint_flx_muscle_sg      = 19;
const int R_knee_joint_ext_muscle_sg     = 20;
const int R_knee_joint_flx_muscle_sg     = 21;
const int R_ankle_joint_ext_muscle_sg    = 22;
const int R_ankle_joint_flx_muscle_sg    = 23;

// ---------------------------------------------------------------------------
// Polling order arrays — UNCHANGED
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Pot and pressure muxes — UNCHANGED (independent objects, own pin sets)
// ---------------------------------------------------------------------------
CD74HC4067 mux(18, 17, 16, 15);
const int pot_pin = A0;

CD74HC4067 pressure_mux_1(23, 22, 21, 20);
const int pressure_pin_1 = A5;

CD74HC4067 pressure_mux_2(31, 30, 29, 28);
const int pressure_pin_2 = A13;

// ---------------------------------------------------------------------------
// LW and SG muxes — rewired to share S0-S3, selected via enable pins.
//
// All four share pins: S0=7, S1=6, S2=5, S3=4
//   LW mux (sensors 0-15)   EN = LW_EN_PIN   (e.g. A14)
//   LW/SG mux (sensors 16-23 LW / 0-7 SG)  EN = LWSG_EN_PIN  (e.g. A15)  <- shared board
//   SG mux (sensors 8-23)   EN = SG_EN_PIN   (e.g. A16)
//
// MuxArray sensor index layout (used by BridgeTuner::calibrateAll):
//   0-15  → LW board 0  (lw_mux,    en[0])
//   16-31 → LW/SG board (lw_sg_mux, en[1])   first 8 = LW ch16-23, next 8 = SG ch0-7
//   32-47 → SG board 1  (sg_mux,    en[2])
//
// All three boards share a single ADC pin — only one board is ever enabled
// at a time via its enable pin, so there is no signal conflict.
// UPDATE the EN pin numbers below to match your available Teensy pins.
// ---------------------------------------------------------------------------
#define LW_SG_S0  12
#define LW_SG_S1  11
#define LW_SG_S2  10
#define LW_SG_S3  9

#define LW_EN_PIN    33   // enable for lw_mux    — UPDATE as needed
#define LWSG_EN_PIN  34   // enable for lw_sg_mux — UPDATE as needed
#define SG_EN_PIN    35   // enable for sg_mux    — UPDATE as needed

const int lwsg_pin = A15; // single ADC pin shared by all three LW/SG boards

// MuxArray wraps the three LW/SG boards with shared S0-S3 + enable pins
MuxArray lwsg_mux_array(LW_SG_S0, LW_SG_S1, LW_SG_S2, LW_SG_S3,
                        LW_EN_PIN, LWSG_EN_PIN, SG_EN_PIN);

BridgeTuner tuner;

int raw_data;

// ---------------------------------------------------------------------------
// setup
// ---------------------------------------------------------------------------
void setup() {
    Serial.begin(9600);
    Serial.setTimeout(1);

    pinMode(pot_pin,        INPUT);
    pinMode(pressure_pin_1, INPUT);
    pinMode(pressure_pin_2, INPUT);
    pinMode(lwsg_pin,       INPUT); // shared ADC pin for all LW/SG boards

    // Initialize LW/SG mux array and bridge tuner, then calibrate all 48 sensors
    lwsg_mux_array.begin();
    tuner.begin();

    Serial.println("Calibrating LW/SG sensors...");
    tuner.calibrateAll(lwsg_mux_array);
    Serial.println("Calibration complete.");
}

// ---------------------------------------------------------------------------
// read_all_joints
// Pot and pressure polling unchanged.
// LW and SG now use enable-based selectChannel() via lwsg_mux_array.
// ---------------------------------------------------------------------------
void read_all_joints() {

    // --- Potentiometers — unchanged ---
    for (int i = 0; i < num_potentiometer; i++) {
        mux.channel(mux_channels[i]);
        analogRead(pot_pin);
        sensor_data[i] = analogRead(pot_pin) / 4;
    }

    // --- Pressure sensors — unchanged ---
    for (int i = 0; i < num_press_sensor; i++) {
        if (press_mux_channel[i] <= 11) {
            pressure_mux_1.channel(press_mux_channel[i] + 2);
            analogRead(pressure_pin_1);
            sensor_data[num_potentiometer + i] = analogRead(pressure_pin_1) / 4;
        } else {
            pressure_mux_2.channel(press_mux_channel[i] + 2 - 12);
            analogRead(pressure_pin_2);
            sensor_data[num_potentiometer + i] = analogRead(pressure_pin_2) / 4;
        }
    }

    // --- Liquid Wire sensors ---
    // All boards share lwsg_pin — only one board enabled at a time via its EN pin.
    // applyCalibration() restores the correct wiper positions for each sensor
    // before reading, so the bridge is balanced to its calibrated baseline.
    for (int i = 0; i < num_lw_sensor; i++) {
        int sensorIndex = lw_mux_channels[i]; // 0-23
        lwsg_mux_array.selectChannel(sensorIndex);
        tuner.applyCalibration(sensorIndex);
        analogRead(lwsg_pin); // dummy read — let ADC capacitor charge
        sensor_data[num_potentiometer + num_press_sensor + i] = analogRead(lwsg_pin) / 4;
        lwsg_mux_array.disableAll();
    }

    // --- Strain Gauge sensors ---
    // SG channels offset by +24 to map into the upper half of MuxArray's 48-sensor space.
    for (int i = 0; i < num_sg_sensor; i++) {
        int sensorIndex = sg_mux_channels[i] + 24; // offset into MuxArray's 48-sensor space
        lwsg_mux_array.selectChannel(sensorIndex);
        tuner.applyCalibration(sensorIndex);
        analogRead(lwsg_pin); // dummy read — let ADC capacitor charge
        sensor_data[num_potentiometer + num_press_sensor + num_lw_sensor + i] = analogRead(lwsg_pin) / 4;
        lwsg_mux_array.disableAll();
    }

    // --- Print ---
    for (int i = 0; i < data_length; i++) Serial.printf("%4d", i);
    Serial.print("\n");
    for (int i = 0; i < data_length; i++) Serial.printf("%4d", sensor_data[i]);
    Serial.print("\n");
}

// ---------------------------------------------------------------------------
// loop
// ---------------------------------------------------------------------------
void loop() {
    read_all_joints();
    delay(1000);
}
