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

AD7606p16_t4 adc(RD_PIN, CS_PIN, CONV_PIN, BUSY_PIN, RESET_PIN);

int16_t channels[8];

void loop() {
    adc.getData(channels);  // Get latest readings
}
```

## Pin Configuration

The library uses optimized GPIO port mapping for maximum speed:

### Data Pins (D0-D15)
TODO, make a table representing the pins

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

### v0.0.1 (Initial Release)
- High-speed GPIO port reading implementation
- 200 kSPS conversions per second performance
- Support for Teensy 4.x boards
