#include <Arduino.h>
#include "capstone_api.h"

MuxArray mux;
BridgeTuner tuner;
int activeChannel = -1;
bool isStreaming = false;

void setup() {
    Serial.begin(115200);
    mux.begin();
    tuner.begin();
}

void loop() {
    // 1. Parse incoming Serial Commands
    if (Serial.available() > 0) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();

        if (cmd.startsWith("SET ")) {
            activeChannel = cmd.substring(4).toInt();
            mux.selectChannel(activeChannel);
            Serial.println("{\"msg\":\"Channel updated\",\"channel\":" + String(activeChannel) + "}");
            isStreaming = false;
        } 
        else if (cmd.startsWith("CALIBRATE")) {
            if (activeChannel != -1) {
                tuner.calibrate();
                Serial.println(tuner.getTelemetry(activeChannel));
            } else {
                Serial.println("{\"error\":\"No channel selected\"}");
            }
        } 
        else if (cmd.startsWith("READ")) {
            if (activeChannel != -1) {
                Serial.println(tuner.getTelemetry(activeChannel));
            }
        }
        else if (cmd == "STREAM ON") {
            isStreaming = true;
        }
        else if (cmd == "STREAM OFF") {
            isStreaming = false;
        }
    }

    // 2. Handle Continuous Telemetry Feed
    if (isStreaming && activeChannel != -1) {
        Serial.println(tuner.getTelemetry(activeChannel));
        delay(20); // 50Hz update rate
    }
}