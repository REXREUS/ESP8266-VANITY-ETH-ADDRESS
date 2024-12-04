# ETH Vanity Address Generator for ESP8266/ESP32
 A microcontroller-based Ethereum vanity address generator running on ESP8266 or ESP32. This program searches for Ethereum addresses starting with a specific prefix and sends the results via email.
# Key Features
- Support for ESP8266 and ESP32
- Custom prefix Ethereum address search
- Automatic email notifications upon address discovery
- System monitoring and memory usage tracking
- Keccak-256 algorithm implementation for address generation
- Integrated WiFi connectivity
  
## Requirements
- Hardware
- ESP8266 or ESP32 board
- Stable internet connection
- Software & Libraries
- Arduino IDE
- ESP Mail Client library
- WiFi library
- micro-ecc library
- Keccak library (included)

## Configuration
Configure these variables in the main file before use:
```cpp
// WiFi Configuration
const char* ssid = "your_ssid";
const char* password = "your_password";

// Email Configuration
#define SMTP_HOST "smtp.gmail.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL "your_email_address@gmail.com"
#define AUTHOR_PASSWORD "your_google_app_password"
#define RECIPIENT_EMAIL "recipient_email_address@gmail.com"

// Desired address prefix
const String TARGET_PREFIX = "0x0";
```
## How It Works
1. Program initializes WiFi connection
2. Starts address search with specified prefix
3. For each attempt:
   - Generates random private key
   - Derives public key using secp256k1 curve
   - Generates Ethereum address using Keccak-256 algorithm
   - Checks if address matches desired prefix
4. When matching address is found, sends details via email

## System Monitoring
The program includes system monitoring features displaying:
- Memory usage
- CPU frequency
- Flash chip speed
- Available sketch space
- Chip ID

## Project Structure
```
├── ESP8266-ETH-vanity-addresses.ino
├── src/
│   ├── Keccak.cpp
│   └── Keccak.h
└── README.md
```
## Security
Important:
- Store private keys securely
- Use Google App Password for SMTP
- Never share WiFi or email credentials

## Contributing
Contributions are welcome! Please submit pull requests for:
- Bug fixes
- Performance improvements
- New features
- Documentation

## License
This project is licensed under the MIT License.

## Disclaimer
**This program is provided "as is" without any warranties. Use at your own risk. Created with ❤️ for the blockchain and IoT community**
