#pragma once
#include <Arduino.h>

class AD7606p16_t4 {
    public:
        AD7606p16_t4(uint8_t RD, uint8_t CS, uint8_t CONVERSION_START, uint8_t BUSY, uint8_t RESET, float vRef = 10.0f);
        void getData(int16_t* data);
        float getVoltage(uint8_t channel);
        void getVoltages(float* voltages);
        void reset();

    private:
        void pulse(uint8_t pin);
        void iPulse(uint8_t pin);
        void startConversion();


        uint8_t D0_D15[16]; // Data pins D0-D15
        uint8_t RD;         // Read pin
        uint8_t CS;         // Chip Select pin
        uint8_t CONVERSION_START;    // Conversion St
        uint8_t BUSY;       // Busy pin
        uint8_t RESET;      // Reset pin

        volatile int16_t channels[8]; // Array to hold the 8 channels data
        float vRef; // Reference voltage (default 10V for ±5V range)
        static void busyFallingISR();
        static AD7606p16_t4* instance; // Instance pointer for ISR
};
