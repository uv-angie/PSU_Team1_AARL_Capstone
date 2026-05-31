#include <Arduino.h>
#include <Wire.h>
#include <ADC.h>

// ---------------------------------------------------------
// HARDWARE MAPPING & CONSTANTS
// ---------------------------------------------------------
#define MUX_S0 2
#define MUX_S1 3
#define MUX_S2 4
#define MUX_S3 5
#define MUX_EN1 6
#define MUX_EN2 7
#define MUX_EN3 8

#define ADC_PIN 14 // A0
#define FAULT_THRESHOLD_V 3.0 // Fault detection limit
#define TARGET_BASELINE_V 1.24 // INA125 Pseudo-ground
#define IA_GAIN 64.0

// DS3502 I2C Addresses
const uint8_t ADDR_COARSE = 0x28;
const uint8_t ADDR_MEDIUM = 0x29;
const uint8_t ADDR_FINE   = 0x2A;

ADC *adc = new ADC();

// ---------------------------------------------------------
// DS3502 I2C CONTROL API
// ---------------------------------------------------------
void setDS3502Mode(uint8_t address, bool useSRAM) {
    Wire.beginTransmission(address);
    Wire.write(0x02); // Control Register
    Wire.write(useSRAM ? 0x80 : 0x00); // 0x80 = MODE 1 (SRAM), 0x00 = MODE 0 (EEPROM)
    Wire.endTransmission();
}

void setDS3502Wiper(uint8_t address, uint8_t position) {
    if (position > 127) position = 127;
    Wire.beginTransmission(address);
    Wire.write(0x00); // Wiper Register
    Wire.write(position);
    Wire.endTransmission();
}

// ---------------------------------------------------------
// MULTIPLEXER CONTROL API
// ---------------------------------------------------------
void disableAllSensors() {
    digitalWrite(MUX_EN1, HIGH);
    digitalWrite(MUX_EN2, HIGH);
    digitalWrite(MUX_EN3, HIGH);
}

void selectSensor(int sensorIndex) {
    disableAllSensors();
    if (sensorIndex < 0 || sensorIndex > 47) return;

    int muxBoard = sensorIndex / 16;
    int channel = sensorIndex % 16;

    digitalWrite(MUX_S0, channel & 0x01);
    digitalWrite(MUX_S1, (channel >> 1) & 0x01);
    digitalWrite(MUX_S2, (channel >> 2) & 0x01);
    digitalWrite(MUX_S3, (channel >> 3) & 0x01);

    if (muxBoard == 0) digitalWrite(MUX_EN1, LOW);
    else if (muxBoard == 1) digitalWrite(MUX_EN2, LOW);
    else if (muxBoard == 2) digitalWrite(MUX_EN3, LOW);

    delayMicroseconds(50); // Allow analog switch RC settling
}

// ---------------------------------------------------------
// HIGH-RES ADC MEASUREMENT
// ---------------------------------------------------------
float readAmplifiedVoltage() {
    // 12-bit resolution yields 0-4095 for 0-3.3V
    int raw = adc->adc0->analogRead(ADC_PIN);
    return (raw * 3.3) / 4095.0;
}

// ---------------------------------------------------------
// FEATURE A: AUTO-CALIBRATION LOOP
// ---------------------------------------------------------
bool calibrateSensor(int sensorIndex) {
    selectSensor(sensorIndex);
    
    // Switch all to SRAM for instant real-time tuning (bypass 10ms EEPROM delay)
    setDS3502Mode(ADDR_COARSE, true);
    setDS3502Mode(ADDR_MEDIUM, true);
    setDS3502Mode(ADDR_FINE, true);

    uint8_t bestCoarse = 0, bestMedium = 0, bestFine = 0;
    
    // Start at Minimum Resistance (V_out will be heavily clipped at ~3.3V max)
    setDS3502Wiper(ADDR_COARSE, 0);
    setDS3502Wiper(ADDR_MEDIUM, 0);
    setDS3502Wiper(ADDR_FINE, 0);
    delay(2);

    // 1. COARSE SWEEP
    for (int i = 0; i <= 127; i++) {
        setDS3502Wiper(ADDR_COARSE, i);
        delayMicroseconds(500); 
        float v = readAmplifiedVoltage();
        
        // We approach 1.24V from above. Stop the moment we dip below.
        if (v < TARGET_BASELINE_V) {
            bestCoarse = (i > 0) ? i - 1 : 0; // Step back to leave positive residual for Medium
            break;
        }
        bestCoarse = i;
    }
    setDS3502Wiper(ADDR_COARSE, bestCoarse);

    // Hardware Fault Check: If Coarse swept to 127 and V_out is still clipped at > 3.0V, 
    // the sensor wire is snapped (open circuit on R4). 
    if (bestCoarse == 127 && readAmplifiedVoltage() > FAULT_THRESHOLD_V) {
        Serial.print("HARDWARE FAULT: Open circuit detected on Sensor ");
        Serial.println(sensorIndex);
        disableAllSensors();
        return false;
    }

    // 2. MEDIUM SWEEP
    for (int i = 0; i <= 127; i++) {
        setDS3502Wiper(ADDR_MEDIUM, i);
        delayMicroseconds(500);
        float v = readAmplifiedVoltage();
        if (v < TARGET_BASELINE_V) {
            bestMedium = (i > 0) ? i - 1 : 0; 
            break;
        }
        bestMedium = i;
    }
    setDS3502Wiper(ADDR_MEDIUM, bestMedium);

    // 3. FINE SWEEP (Absolute Minimum Error Search)
    float minError = 100.0;
    for (int i = 0; i <= 127; i++) {
        setDS3502Wiper(ADDR_FINE, i);
        delayMicroseconds(500);
        float v = readAmplifiedVoltage();
        float error = abs(v - TARGET_BASELINE_V);
        if (error < minError) {
            minError = error;
            bestFine = i;
        }
    }
    setDS3502Wiper(ADDR_FINE, bestFine);

    // Lock baseline into Non-Volatile EEPROM
    setDS3502Mode(ADDR_COARSE, false); setDS3502Wiper(ADDR_COARSE, bestCoarse);
    setDS3502Mode(ADDR_MEDIUM, false); setDS3502Wiper(ADDR_MEDIUM, bestMedium);
    setDS3502Mode(ADDR_FINE, false);   setDS3502Wiper(ADDR_FINE, bestFine);

    Serial.print("Sensor "); Serial.print(sensorIndex);
    Serial.print(" Calibrated | Coarse: "); Serial.print(bestCoarse);
    Serial.print(" Med: "); Serial.print(bestMedium);
    Serial.print(" Fine: "); Serial.println(bestFine);
    
    return true;
}

// ---------------------------------------------------------
// FEATURE B: TELEMETRY API
// ---------------------------------------------------------
float getResidualDrift_mV(int sensorIndex) {
    selectSensor(sensorIndex);
    float v_out = readAmplifiedVoltage();

    // Fault detection during normal neural net operation
    if (v_out > FAULT_THRESHOLD_V) {
        disableAllSensors();
        return -9999.0; // Error code for host system
    }

    // Reverse engineer the instrumentation amplifier math to find physical bridge drift
    float residual_V = v_out - TARGET_BASELINE_V;
    float bridge_diff_V = residual_V / IA_GAIN;
    
    return bridge_diff_V * 1000.0; // Return in millivolts
}

// ---------------------------------------------------------
// SYSTEM INITIALIZATION
// ---------------------------------------------------------
void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000); 

    // Initialize MUX Pins
    pinMode(MUX_S0, OUTPUT); pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT); pinMode(MUX_S3, OUTPUT);
    pinMode(MUX_EN1, OUTPUT); pinMode(MUX_EN2, OUTPUT); pinMode(MUX_EN3, OUTPUT);
    disableAllSensors();

    // Initialize I2C Bus at 400kHz (Fast Mode for rapid calibration)
    Wire.begin();
    Wire.setClock(400000);

    // Initialize ADC for High-Performance Averaging
    pinMode(ADC_PIN, INPUT);
    adc->adc0->setAveraging(16); // Hardware averages 16 samples to destroy noise
    adc->adc0->setResolution(12);
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

    Serial.println("AARL Strain Gauge Acquisition System Initialized.");
    
    // Execute Feature A Auto-Calibration on Startup for Sensor 0
    Serial.println("Initiating Feature A: Auto-Calibration...");
    calibrateSensor(0); 
}

void loop() {
    // Execute Feature B: Continuous Telemetry Feed
    float drift = getResidualDrift_mV(0);
    
    if (drift == -9999.0) {
        Serial.println("ERR: Sensor disconnected. System safed.");
        while(1); // Halt
    } else {
        Serial.print("Bridge Drift (mV): ");
        Serial.println(drift, 4);
    }
    
    delay(10); // 100Hz telemetry update rate
}