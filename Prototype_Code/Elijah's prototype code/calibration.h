#pragma once
#include "config.h"
#include "CD74HC4067.h"
#include <Wire.h>

// ---------------------------------------------------------------------------
// CalibrationData — stores the three wiper tap positions for one sensor
// ---------------------------------------------------------------------------
struct CalibrationData {
    uint8_t coarse = 0;
    uint8_t medium = 0;
    uint8_t fine   = 0;
    bool    valid  = false;
};

// ---------------------------------------------------------------------------
// MuxArray
//
// For LW and SG sensors only. All muxes share the same S0-S3 control pins
// (one CD74HC4067 object). Each mux board is enabled individually via its
// own active-LOW enable pin, so only one board drives the shared ADC line
// (ADC_PIN in config.h) at a time. Supports up to 48 sensors across three boards.
// ---------------------------------------------------------------------------
class MuxArray {
public:
    CD74HC4067 mux;       // shared S0-S3 — one instance for all LW/SG boards
    const uint8_t en[3];  // one enable pin per board (active LOW)

    MuxArray(uint8_t s0, uint8_t s1, uint8_t s2, uint8_t s3,
             uint8_t en0, uint8_t en1, uint8_t en2)
        : mux_4(s0, s1, s2, s3), en{en0, en1, en2} {}

    //  Sets all enable pins to be outputs, and turns off the muxes (active low enable)
    void begin() { 
        for (int i = 0; i < 3; i++) {
            pinMode(en[i], OUTPUT);
        }
        disableAll();
    }

    //  Turns off the three muxes (active low enable)
    void disableAll() {
        for (int i = 0; i < 3; i++) {
            digitalWrite(en[i], HIGH); // HIGH = disabled (active LOW)
        }
    }


    // Select sensor 0-47 across three 16-channel boards when given sensor index (0-47)
    void selectChannel(int sensorIndex) {
        disableAll();
        if (sensorIndex < 0 || sensorIndex > 47) return;

        int board   = sensorIndex / 16;
        int channel = sensorIndex % 16;

        mux_4.channel(channel);        // set S0-S3 via CD74HC4067 truth table
        delayMicroseconds(10);         // let address lines settle before enabling
        digitalWrite(en[board], LOW);  // enable the selected board
        delayMicroseconds(50);         // analog settling time
    }
};

// ---------------------------------------------------------------------------
// BridgeTuner
//
// Manages I2C digital potentiometers (coarse/medium/fine) for Wheatstone
// bridge balancing on LW and SG sensors. Stores per-sensor calibration in
// _cal[48].
// ---------------------------------------------------------------------------
class BridgeTuner {
    //  Create the array for calibration data for each sensor
private:
    static constexpr int NUM_SENSORS = 48;
    CalibrationData _cal[NUM_SENSORS];

    //  
    void setMode(uint8_t addr, bool sram) {
        Wire.beginTransmission(addr);
        Wire.write(0x02);
        Wire.write(sram ? 0x80 : 0x00);
        Wire.endTransmission();
    }

    //  Set the wiper of the digipot with address "addr" to position "pos"
    void setWiper(uint8_t addr, uint8_t pos) {
        if (pos > 127) pos = 127;
        Wire.beginTransmission(addr);
        Wire.write(0x00);
        Wire.write(pos);
        Wire.endTransmission();
    }

    //  Read the voltage of the ADC
    float readVoltage() {
        analogRead(ADC_PIN); // dummy read — let ADC capacitor charge
        int raw = analogRead(ADC_PIN);
        return (raw * 3.3f) / 4095.0f;
    }

    //  Start I2C connection and make ADC pin an input
public:
    void begin() {
        Wire.begin();
        Wire.setClock(400000);
        pinMode(ADC_PIN, INPUT);
    }

    // JSON telemetry for one channel, using stored calibration taps
    String getTelemetry(int channel) {
        float v_out = readVoltage();
        if (v_out > FAULT_THRESHOLD_V) {
            return "{\"channel\":" + String(channel) + ",\"status\":\"FAULT_OPEN_CIRCUIT\"}";
        }

        float residual_V = v_out - TARGET_BASELINE_V;
        float v_diff_mV  = (residual_V / IA_GAIN) * 1000.0f;

        uint8_t coarse = 0, medium = 0, fine = 0;
        bool    calValid = false;
        if (channel >= 0 && channel < NUM_SENSORS) {
            coarse   = _cal[channel].coarse;
            medium   = _cal[channel].medium;
            fine     = _cal[channel].fine;
            calValid = _cal[channel].valid;
        }

        return "{\"channel\":"     + String(channel) +
               ",\"v_diff_mV\":"  + String(v_diff_mV, 4) +
               ",\"coarse_tap\":" + String(coarse) +
               ",\"medium_tap\":" + String(medium) +
               ",\"fine_tap\":"   + String(fine) +
               ",\"cal_valid\":"  + (calValid ? "true" : "false") +
               ",\"status\":\"OK\"}";
    }

    // Restore the three wipers to the stored calibration taps for a given sensor.
    // Call this after selectChannel() and before analogRead() during normal polling.
    // Has no effect if the sensor has not been calibrated yet.
    void applyCalibration(int sensorIndex) {
        if (sensorIndex < 0 || sensorIndex >= NUM_SENSORS) return;
        if (!_cal[sensorIndex].valid) return;
        setWiper(ADDR_COARSE, _cal[sensorIndex].coarse);
        setWiper(ADDR_MEDIUM, _cal[sensorIndex].medium);
        setWiper(ADDR_FINE,   _cal[sensorIndex].fine);
    }

    // Calibrate one sensor (mux channel must already be selected)
    void calibrate(int sensorIndex) {
        if (sensorIndex < 0 || sensorIndex >= NUM_SENSORS) return;

        uint8_t coarse = 0, medium = 0, fine = 0;

        setMode(ADDR_COARSE, true); setMode(ADDR_MEDIUM, true); setMode(ADDR_FINE, true);
        setWiper(ADDR_COARSE, 0);   setWiper(ADDR_MEDIUM, 0);   setWiper(ADDR_FINE, 0);
        delay(2);

        // Coarse sweep — highest tap still above TARGET_BASELINE_V
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_COARSE, i);
            delayMicroseconds(500);
            if (readVoltage() < TARGET_BASELINE_V) { coarse = (i > 0) ? i - 1 : 0; break; }
            coarse = i;
        }
        setWiper(ADDR_COARSE, coarse);

        // Medium sweep
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_MEDIUM, i);
            delayMicroseconds(500);
            if (readVoltage() < TARGET_BASELINE_V) { medium = (i > 0) ? i - 1 : 0; break; }
            medium = i;
        }
        setWiper(ADDR_MEDIUM, medium);

        // Fine sweep — tap closest to TARGET_BASELINE_V
        float minError = 100.0f;
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_FINE, i);
            delayMicroseconds(500);
            float err = abs(readVoltage() - TARGET_BASELINE_V);
            if (err < minError) { minError = err; fine = i; }
        }
        setWiper(ADDR_FINE, fine);

        // Commit taps to EEPROM on each pot
        setMode(ADDR_COARSE, false); setWiper(ADDR_COARSE, coarse);
        setMode(ADDR_MEDIUM, false); setWiper(ADDR_MEDIUM, medium);
        setMode(ADDR_FINE,   false); setWiper(ADDR_FINE,   fine);

        _cal[sensorIndex] = { coarse, medium, fine, true };
    }

    // Calibrate all 48 LW/SG sensors using the provided MuxArray
    void calibrateAll(MuxArray &muxArray) {
        for (int i = 0; i < NUM_SENSORS; i++) {
            muxArray.selectChannel(i);
            calibrate(i);
        }
        muxArray.disableAll();
    }

    const CalibrationData& getCalibration(int sensorIndex) const {
        static const CalibrationData empty{};
        if (sensorIndex < 0 || sensorIndex >= NUM_SENSORS) return empty;
        return _cal[sensorIndex];
    }

    bool allCalibrated() const {
        for (int i = 0; i < NUM_SENSORS; i++) {
            if (!_cal[i].valid) return false;
        }
        return true;
    }
};
