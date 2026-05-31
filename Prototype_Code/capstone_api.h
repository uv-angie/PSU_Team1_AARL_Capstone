#pragma once
#include "config.h"
#include <Wire.h>
#include <ADC.h>

class MuxArray {
public:
    void begin() {
        pinMode(MUX_S0, OUTPUT); pinMode(MUX_S1, OUTPUT);
        pinMode(MUX_S2, OUTPUT); pinMode(MUX_S3, OUTPUT);
        pinMode(MUX_EN1, OUTPUT); pinMode(MUX_EN2, OUTPUT); pinMode(MUX_EN3, OUTPUT);
        disableAll();
    }

    void disableAll() {
        digitalWrite(MUX_EN1, HIGH);
        digitalWrite(MUX_EN2, HIGH);
        digitalWrite(MUX_EN3, HIGH);
    }

    void selectChannel(int sensorIndex) {
        disableAll();
        if (sensorIndex < 0 || sensorIndex > 47) return;

        int board = sensorIndex / 16;
        int channel = sensorIndex % 16;

        digitalWrite(MUX_S0, channel & 0x01);
        digitalWrite(MUX_S1, (channel >> 1) & 0x01);
        digitalWrite(MUX_S2, (channel >> 2) & 0x01);
        digitalWrite(MUX_S3, (channel >> 3) & 0x01);

        if (board == 0) digitalWrite(MUX_EN1, LOW);
        else if (board == 1) digitalWrite(MUX_EN2, LOW);
        else if (board == 2) digitalWrite(MUX_EN3, LOW);

        delayMicroseconds(50); // Analog settling time
    }
};

class BridgeTuner {
private:
    ADC *adc;
    uint8_t currentCoarse = 0, currentMedium = 0, currentFine = 0;

    void setMode(uint8_t addr, bool sram) {
        Wire.beginTransmission(addr);
        Wire.write(0x02);
        Wire.write(sram ? 0x80 : 0x00);
        Wire.endTransmission();
    }

    void setWiper(uint8_t addr, uint8_t pos) {
        if (pos > 127) pos = 127;
        Wire.beginTransmission(addr);
        Wire.write(0x00);
        Wire.write(pos);
        Wire.endTransmission();
    }

public:
    void begin() {
        Wire.begin();
        Wire.setClock(400000);
        adc = new ADC();
        pinMode(ADC_PIN, INPUT);
        adc->adc0->setAveraging(16);
        adc->adc0->setResolution(12);
    }

    float readVoltage() {
        int raw = adc->adc0->analogRead(ADC_PIN);
        return (raw * 3.3f) / 4095.0f;
    }

    // Returns a JSON-formatted string with telemetry
    String getTelemetry(int channel) {
        float v_out = readVoltage();
        if (v_out > FAULT_THRESHOLD_V) {
            return "{\"channel\":" + String(channel) + ",\"status\":\"FAULT_OPEN_CIRCUIT\"}";
        }
        
        float residual_V = v_out - TARGET_BASELINE_V;
        float v_diff_mV = (residual_V / IA_GAIN) * 1000.0f;

        return "{\"channel\":" + String(channel) + 
               ",\"v_diff_mV\":" + String(v_diff_mV, 4) + 
               ",\"coarse_tap\":" + String(currentCoarse) + 
               ",\"medium_tap\":" + String(currentMedium) + 
               ",\"fine_tap\":" + String(currentFine) + 
               ",\"status\":\"OK\"}";
    }

    void calibrate() {
        setMode(ADDR_COARSE, true); setMode(ADDR_MEDIUM, true); setMode(ADDR_FINE, true);
        setWiper(ADDR_COARSE, 0); setWiper(ADDR_MEDIUM, 0); setWiper(ADDR_FINE, 0);
        delay(2);

        // Coarse Sweep
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_COARSE, i); delayMicroseconds(500);
            if (readVoltage() < TARGET_BASELINE_V) { currentCoarse = (i>0)? i-1 : 0; break; }
            currentCoarse = i;
        }
        setWiper(ADDR_COARSE, currentCoarse);

        // Medium Sweep
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_MEDIUM, i); delayMicroseconds(500);
            if (readVoltage() < TARGET_BASELINE_V) { currentMedium = (i>0)? i-1 : 0; break; }
            currentMedium = i;
        }
        setWiper(ADDR_MEDIUM, currentMedium);

        // Fine Sweep
        float minError = 100.0f;
        for (int i = 0; i <= 127; i++) {
            setWiper(ADDR_FINE, i); delayMicroseconds(500);
            float err = abs(readVoltage() - TARGET_BASELINE_V);
            if (err < minError) { minError = err; currentFine = i; }
        }
        setWiper(ADDR_FINE, currentFine);

        // Save to EEPROM
        setMode(ADDR_COARSE, false); setWiper(ADDR_COARSE, currentCoarse);
        setMode(ADDR_MEDIUM, false); setWiper(ADDR_MEDIUM, currentMedium);
        setMode(ADDR_FINE, false);   setWiper(ADDR_FINE, currentFine);
    }
};