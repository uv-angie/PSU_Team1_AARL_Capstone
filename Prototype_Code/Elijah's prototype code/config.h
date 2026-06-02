#pragma once
#include <Arduino.h>

// MUX Control Pins
constexpr uint8_t MUX_S0 = 2;
constexpr uint8_t MUX_S1 = 3;
constexpr uint8_t MUX_S2 = 4;
constexpr uint8_t MUX_S3 = 5;
constexpr uint8_t MUX_EN1 = 6;
constexpr uint8_t MUX_EN2 = 7;
constexpr uint8_t MUX_EN3 = 8;

// Analog & Math Constants
constexpr uint8_t ADC_PIN = 41; 
constexpr float FAULT_THRESHOLD_V = 3.0f;
constexpr float TARGET_BASELINE_V = 1.24f; // INA125 Pseudo-ground
constexpr float IA_GAIN = 64.0f;

// DS3502 I2C Addresses
constexpr uint8_t ADDR_COARSE = 0x28;
constexpr uint8_t ADDR_MEDIUM = 0x29;
constexpr uint8_t ADDR_FINE   = 0x2A;