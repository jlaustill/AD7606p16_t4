#include "AD7606p16_t4.h"

AD7606p16_t4* AD7606p16_t4::instance = nullptr;

AD7606p16_t4::AD7606p16_t4(uint8_t RD, uint8_t CS, uint8_t CONVERSION_START, uint8_t BUSY, uint8_t RESET) {
    this->D0_D15[0] = 40; this->D0_D15[1] = 41; this->D0_D15[2] = 37; this->D0_D15[3] = 36;
    this->D0_D15[4] = 35; this->D0_D15[5] = 34; this->D0_D15[6] = 39; this->D0_D15[7] = 38;
    this->D0_D15[8] = 24; this->D0_D15[9] = 25; this->D0_D15[10] = 14; this->D0_D15[11] = 15;
    this->D0_D15[12] = 23; this->D0_D15[13] = 20; this->D0_D15[14] = 21; this->D0_D15[15] = 16;
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
        
        noInterrupts(); // Disable interrupts to ensure atomic read
        digitalWriteFast(instance->CS, LOW); // Enable data read
        for (uint8_t channel = 0; channel < 8; channel++) {
            instance->pulse(instance->RD); // Pulse the RD pin to read data
        
            uint32_t gpio6 = GPIO6_DR; // Read GPIO6 GDIR register
            uint32_t gpio8 = GPIO8_DR; // Read GPIO8 GDIR register

            uint16_t data = (gpio8 >> 10) & 0b0000000011111111; // Read D0-D7 from GPIO8
            data |= ((gpio6 >> 12) << 8)  & 0b0000001100000000; // Read D8-D9 from GPIO6
            data |= ((gpio6 >> 18) << 10) & 0b0000110000000000; // Read D10-D11 from GPIO6
            data |= ((gpio6 >> 25) << 12) & 0b0111000000000000; // Read D12-D14 from GPIO6
            data |= ((gpio6 >> 23) << 15) & 0b1000000000000000; // Read D15 from GPIO6

            instance->channels[channel] = data; // Store the read data in the channels array
        }

        instance->iPulse(instance->CONVERSION_START); // Pulse the RD pin to read data
        digitalWriteFast(instance->CS, HIGH); // Disable data read
        interrupts(); // Re-enable interrupts
    }
}

void AD7606p16_t4::getData(uint16_t* data) {
    noInterrupts(); // Disable interrupts to ensure atomic read
    for (uint8_t i = 0; i < 8; i++) {
        data[i] = instance->channels[i];
    }
    interrupts(); // Re-enable interrupts
}
