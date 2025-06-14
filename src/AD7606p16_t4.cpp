#include "AD7606p16_t4.h"

AD7606p16_t4* AD7606p16_t4::instance = nullptr;

AD7606p16_t4::AD7606p16_t4(uint8_t RD, uint8_t CS, uint8_t CONVERSION_START, uint8_t BUSY, uint8_t RESET) {
    #ifdef ARDUINO_TEENSY41
        // Teensy 4.1: GPIO6[16:31] consecutive bits
        // D0-D15 map to GPIO6 bits 16-31
        this->D0_D15[0] = 19;  // GPIO6.16
        this->D0_D15[1] = 18;  // GPIO6.17  
        this->D0_D15[2] = 14;  // GPIO6.18
        this->D0_D15[3] = 15;  // GPIO6.19
        this->D0_D15[4] = 40;  // GPIO6.20
        this->D0_D15[5] = 41;  // GPIO6.21
        this->D0_D15[6] = 17;  // GPIO6.22
        this->D0_D15[7] = 16;  // GPIO6.23
        this->D0_D15[8] = 22;  // GPIO6.24
        this->D0_D15[9] = 23;  // GPIO6.25
        this->D0_D15[10] = 20; // GPIO6.26
        this->D0_D15[11] = 21; // GPIO6.27
        this->D0_D15[12] = 38; // GPIO6.28
        this->D0_D15[13] = 39; // GPIO6.29
        this->D0_D15[14] = 26; // GPIO6.30
        this->D0_D15[15] = 27; // GPIO6.31
        
    #elif defined(ARDUINO_TEENSY_MICROMOD)
        this->D0_D15[0] = 37;
        this->D0_D15[1] = 36;
        this->D0_D15[2] = 35;
        this->D0_D15[3] = 34;
        this->D0_D15[4] = 38;
        this->D0_D15[5] = 39; 
        this->D0_D15[6] = 40; 
        this->D0_D15[7] = 41;
        this->D0_D15[8] = 42; 
        this->D0_D15[9] = 43; 
        this->D0_D15[10] = 44; 
        this->D0_D15[11] = 45;
        this->D0_D15[12] = 63; 
        this->D0_D15[13] = 9; 
        this->D0_D15[14] = 32; 
        this->D0_D15[15] = 8;
    #else
        #error "Unsupported board."
    #endif
    this->RD = RD;
    this->CS = CS;
    this->CONVERSION_START = CONVERSION_START;
    this->BUSY = BUSY;
    this->RESET = RESET;

    for (uint8_t i = 0; i < 8; i++) {
        this->channels[i] = 0; // Initialize channels to 0
    }

    this->instance = this; // Set the instance pointer for ISR

    pinMode(this->RESET, OUTPUT);
    pinMode(this->CS, OUTPUT);
    pinMode(this->RD, OUTPUT);
    pinMode(this->CONVERSION_START, OUTPUT);
    pinMode(this->BUSY, INPUT);
    for (uint8_t i = 0; i < 16; i++) {
        pinMode(this->D0_D15[i], INPUT);
    }
    digitalWriteFast(this->RESET, LOW);
    digitalWriteFast(this->CS, HIGH); 
    digitalWriteFast(this->CONVERSION_START, LOW);
    digitalWriteFast(this->RD, HIGH);

    attachInterrupt(digitalPinToInterrupt(this->BUSY), this->busyFallingISR, FALLING);

    this->reset();
    this->startConversion();
}

void AD7606p16_t4::reset()
{
    digitalWriteFast(this->RESET, HIGH);
    delayMicroseconds(25);
    digitalWriteFast(this->RESET, LOW);
    delayMicroseconds(1);
}

void AD7606p16_t4::pulse(uint8_t pin)
{
    digitalWriteFast(pin, HIGH);
    delayNanoseconds(25);  // Meet minimum 25ns pulse width
    digitalWriteFast(pin, LOW);
}

void AD7606p16_t4::iPulse(uint8_t pin)
{
    digitalWriteFast(pin, LOW);
    delayNanoseconds(25);  // Meet minimum 25ns pulse width
    digitalWriteFast(pin, HIGH);
}

void AD7606p16_t4::startConversion()
{
    this->pulse(this->CONVERSION_START);
}

void AD7606p16_t4::busyFallingISR() {
    if (instance) {
        digitalWriteFast(instance->CS, LOW); // Enable data read
        
        noInterrupts(); // Disable interrupts to ensure atomic read
        for (uint8_t channel = 0; channel < 8; channel++) {
            if (channel > 0) instance->pulse(instance->RD); // Pulse the RD pin to read data
            
            uint16_t raw = 0;

            #ifdef ARDUINO_TEENSY41
                raw = (GPIO6_PSR >> 16) & 0xFFFF;
            #elif defined(ARDUINO_TEENSY_MICROMOD)
                raw = (GPIO8_PSR >> 12) & 0b0000000000111111;           // D0–D5
                raw |= ((GPIO7_PSR >> 4) & 0b0000000111111111) << 6;    // D6–D14
                raw |= ((GPIO7_PSR >> 16) & 0b1) << 15;                 // D15
            #else 
                #error "Unsupported board"
            #endif

            int16_t data = static_cast<int16_t>(raw);
            instance->channels[channel] = data;
        }
        interrupts(); // Re-enable interrupts

        digitalWriteFast(instance->CS, HIGH); // Disable data read
        instance->startConversion(); // Pulse the RD pin to read data
    }
}

void AD7606p16_t4::getData(int16_t* data) {
    noInterrupts(); // Disable interrupts to ensure atomic read
    for (uint8_t i = 0; i < 8; i++) {
        data[i] = instance->channels[i];
    }
    interrupts(); // Re-enable interrupts
}

float AD7606p16_t4::getVoltage(uint8_t channel) {
    if (channel >= 8) return 0.0f; // Invalid channel
    
    noInterrupts(); // Disable interrupts to ensure atomic read
    int16_t rawValue = instance->channels[channel];
    interrupts(); // Re-enable interrupts
    
    return (rawValue * 5.0f) / 32768.0f;
}

void AD7606p16_t4::getVoltages(float* voltages) {
    noInterrupts(); // Disable interrupts to ensure atomic read
    for (uint8_t i = 0; i < 8; i++) {
        voltages[i] = (instance->channels[i] * 5.0f) / 32768.0f;
    }
    interrupts(); // Re-enable interrupts
}
