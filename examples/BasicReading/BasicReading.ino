/*
 * AD7606p16_t4 Basic Reading Example
 * 
 * This example demonstrates high-speed 16-bit parallel reading from an AD7606 ADC
 * using optimized GPIO port access on Teensy 4.x boards.
 * 
 * Performance: 200 kSPS conversions per second configuratable with oversampling.
 * 
 * Hardware Requirements:
 * - Teensy 4.1, or MicroMod
 * - AD7606 ADC connected in 16-bit parallel mode
 * 
 * Pin Configuration:
 * - Data pins D0-D15: See README.md pin table for exact mappings (hardcoded in library)
 * - RD: pin 3
 * - CS: pin 4  
 * - CONVST: pin 6
 * - BUSY: pin 27
 * - RESET: pin 5
 * 
 * Note: Data pins are hardcoded for optimal GPIO port performance and cannot be changed.
 */

#include <AD7606p16_t4.h>

// Define your pin connections for the AD7606
#define BUSY_PIN 29
#define RESET_PIN 5
#define CONV_START_PIN 33
#define RD_PIN 3
#define CS_PIN 4

// Create AD7606 instance with default 10V reference (±5V range)
AD7606p16_t4 adc(RD_PIN, CS_PIN, CONV_START_PIN, BUSY_PIN, RESET_PIN);

// For ±10V range, use 20V reference:
// AD7606p16_t4 adc(RD_PIN, CS_PIN, CONV_START_PIN, BUSY_PIN, RESET_PIN, 20.0f);

// Buffer to hold channel data
int16_t channelData[8];
float voltageData[8];

// Timing variables
unsigned long lastPrint = 0;
unsigned long loopCount = 0;

void setup() {
    Serial.begin(115200);
    while (!Serial) delay(10); // Wait for serial connection
    
    Serial.println("AD7606p16_t4 Basic Reading Example");
    Serial.println("High-speed 16-bit parallel ADC library for Teensy 4.x");
    Serial.println("========================================");
    Serial.println();
}

void loop() {
    // Read data from AD7606 (non-blocking, ISR-driven)
    adc.getData(channelData);
    adc.getVoltages(voltageData);
    loopCount++;
    
    // Print results every second
    if (millis() - lastPrint >= 1000) {
        Serial.print("Loops/sec: ");
        Serial.println(loopCount);
        
        Serial.print("Voltages (V): ");
        for (int i = 0; i < 8; i++) {
            Serial.print(voltageData[i], 3);
            if (i < 7) Serial.print(", ");
        }
        Serial.println();
        
        Serial.print("Individual voltage reads: ");
        for (int i = 0; i < 8; i++) {
            Serial.print(adc.getVoltage(i), 3);
            if (i < 7) Serial.print(", ");
        }
        Serial.println();
        
        loopCount = 0;
        lastPrint = millis();
    }
    
    // Your other code here...
    delay(1); // Small delay to prevent overwhelming the serial output
}
