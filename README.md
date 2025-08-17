# AD7606p16_t4

High-speed 16-bit parallel AD7606 ADC library for Teensy 4.x

## Features

🚀 **Ultra-High Performance**: 200 kSPS per second  
⚡ **Optimized GPIO Access**: Direct port register reading
🎯 **Teensy 4.x Optimized**: Leverages GPIO6, GPIO7 & GPIO8 fast ports  
📊 **8-Channel Simultaneous**: All 8 AD7606 channels read in parallel  
🔧 **Easy Integration**: Drop-in replacement for slower bit-banging approaches  

## Performance Comparison

| Method | Speed | CPU Usage |
|--------|-------|-----------|
| Individual `digitalRead()` | ~1K/sec | High (lockups) |
| **AD7606p16_t4** | **200 kSPS** | **Minimal** |

## Hardware Requirements

- **Teensy 4.1, or MicroMod (4.0 might work if you can somehow access all the pins, but I dobut it)**
- **AD7606 ADC** in 16-bit parallel mode
- **16 data pins** + 5 control pins

## Quick Start

### Installation

**PlatformIO:**
```ini
[env:teensy41]
lib_deps = 
    https://github.com/yourusername/AD7606p16_t4.git
```

**Arduino IDE:**
- Download library ZIP from GitHub
- Sketch → Include Library → Add .ZIP Library

### Basic Usage

```cpp
#include <AD7606p16_t4.h>

// Default constructor (10V reference for ±5V range)
AD7606p16_t4 adc(RD_PIN, CS_PIN, CONV_PIN, BUSY_PIN, RESET_PIN);

// Custom reference voltage (20V for ±10V range)
// AD7606p16_t4 adc(RD_PIN, CS_PIN, CONV_PIN, BUSY_PIN, RESET_PIN, 20.0f);

int16_t channels[8];
float voltages[8];

void loop() {
    adc.getData(channels);      // Get raw 16-bit readings
    adc.getVoltages(voltages);  // Get all voltages at once
    
    float ch0_voltage = adc.getVoltage(0);  // Get single channel voltage
}
```

## Pin Configuration

The library uses optimized GPIO port mapping for maximum speed:

### Data Pins (D0-D15)

| Data Pin | Teensy4.1 Pin | Teensy4.1 GPIO | TeensyMM Pin | TeensyMM GPIO |
|----------|---------------|----------------|--------------|---------------|
| D0       | 19            | GPIO6.16       | 37           | GPIO8.12      |
| D1       | 18            | GPIO6.17       | 36           | GPIO8.13      |
| D2       | 14            | GPIO6.18       | 35           | GPIO8.14      |
| D3       | 15            | GPIO6.19       | 34           | GPIO8.15      |
| D4       | 40            | GPIO6.20       | 38           | GPIO8.16      |
| D5       | 41            | GPIO6.21       | 39           | GPIO8.17      |
| D6       | 17            | GPIO6.22       | 40           | GPIO7.4       |
| D7       | 16            | GPIO6.23       | 41           | GPIO7.5       |
| D8       | 22            | GPIO6.24       | 42           | GPIO7.6       |
| D9       | 23            | GPIO6.25       | 43           | GPIO7.7       |
| D10      | 20            | GPIO6.26       | 44           | GPIO7.8       |
| D11      | 21            | GPIO6.27       | 45           | GPIO7.9       |
| D12      | 38            | GPIO6.28       | 63           | GPIO7.10      |
| D13      | 39            | GPIO6.29       | 9            | GPIO7.11      |
| D14      | 26            | GPIO6.30       | 32           | GPIO7.12      |
| D15      | 27            | GPIO6.31       | 8            | GPIO7.16      |

### Control Pins
- **RD**: Read signal (any available pin)
- **CS**: Chip Select (any available pin)
- **CONVST**: Conversion Start (any available pin)
- **BUSY**: Busy signal (any interrupt-capable pin)
- **RESET**: Reset signal (any available pin)

## How It Works

Traditional approaches read pins individually:
```cpp
// SLOW: 16 individual reads
for (int i = 0; i < 16; i++) {
    data |= digitalRead(pins[i]) << i;  // 16 function calls!
}
```

**AD7606p16_t4** reads entire GPIO ports simultaneously:
```cpp
// FAST: 1 port reads + bit extraction
data = (GPIO6_PSR >> 16) & 0b1111111111111111; // Read D0-D15 from GPIO6
// Extract bits with optimized operations
```

## API Reference

### Methods

#### `void getData(int16_t* data)`
Gets raw 16-bit ADC readings for all 8 channels.
- **Parameters**: `data` - Array of 8 int16_t values to store readings
- **Returns**: None
- **Thread-safe**: Yes (uses interrupt disabling)

#### `float getVoltage(uint8_t channel)`
Gets voltage reading for a single channel.
- **Parameters**: `channel` - Channel number (0-7)
- **Returns**: Voltage as float (uses configured vRef)
- **Thread-safe**: Yes (uses interrupt disabling)
- **Note**: Returns 0.0f for invalid channel numbers

#### `void getVoltages(float* voltages)`
Gets voltage readings for all 8 channels at once.
- **Parameters**: `voltages` - Array of 8 float values to store voltages
- **Returns**: None
- **Thread-safe**: Yes (uses interrupt disabling)
- **Note**: Uses configured reference voltage from constructor

#### `void reset()`
Performs hardware reset of the AD7606.
- **Parameters**: None
- **Returns**: None

## Performance Goals

1. **Minimize ISR duration** - library handles this automatically
2. **Ability to Call `getData()` regularly** to get latest readings

## Supported Boards

- ✅ Teensy 4.1  
- ✅ Teensy MicroMod

## Examples

- **BasicReading**: Simple voltage measurement

## License

MIT License - see [LICENSE](LICENSE) file

## Contributing

Contributions welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## Changelog

### v0.1.0 
- **BREAKING**: Add configurable vRef parameter to constructor (defaults to 10V)
- **FIX**: Correct voltage calculation formula (was reading half the actual voltage)
- **FIX**: Proper 2's complement handling for signed 16-bit values
- Remove debug code from ISR for better performance

### v0.0.1 (Initial Release)
- High-speed GPIO port reading implementation
- 200 kSPS conversions per second performance
- Support for Teensy 4.x boards
