# Blue-ESP32

A Bluetooth Low Energy (BLE) controlled RGB LED and OLED display system consisting of an ESP32 microcontroller and a companion iOS application.

## Description

Blue-ESP32 is a project that demonstrates wireless control of hardware peripherals using BLE communication. The system includes:

- **ESP32 Firmware**: Runs on an ESP32 board, acting as a BLE server that controls an RGB LED and displays text on an OLED screen.
- **iOS App "Blue"**: A SwiftUI-based mobile application that discovers and connects to the ESP32 device, allowing users to control the LED color and send text messages to the display.

## Features

- **Wireless Control**: Control RGB LED colors and OLED text display via Bluetooth LE
- **Real-time Updates**: Instant color changes and text display on the ESP32
- **Automatic Connection**: iOS app automatically scans for and connects to the ESP32 device
- **Simple UI**: Intuitive color picker and text input interface
- **Cross-platform Compatibility**: ESP32 firmware works with any BLE-compatible device

## Hardware Requirements

- ESP32 development board (e.g., ESP32-WROOM-32)
- RGB LED (common cathode)
- SSD1306 OLED display (128x64, I2C)
- Jumper wires and breadboard for connections

### Pin Connections (ESP32)

| Component | Pin |
|-----------|-----|
| RGB LED - Red | GPIO 27 |
| RGB LED - Green | GPIO 26 |
| RGB LED - Blue | GPIO 25 |
| OLED SDA | GPIO 21 (default I2C) |
| OLED SCL | GPIO 22 (default I2C) |

## Software Requirements

### ESP32 Firmware
- Arduino IDE with ESP32 board support
- Required libraries:
  - BLE (built-in with ESP32)
  - Adafruit_GFX
  - Adafruit_SSD1306

### iOS App
- Xcode 15.0 or later
- iOS 15.0 or later
- Swift 5.0 or later

## Installation and Setup

### ESP32 Firmware Setup

1. **Install Arduino IDE** and add ESP32 board support:
   - Open Arduino IDE
   - Go to File > Preferences
   - Add `https://dl.espressif.com/dl/package_esp32_index.json` to Additional Board Manager URLs
   - Tools > Board > Boards Manager > Search for ESP32 and install

2. **Install Required Libraries**:
   - Sketch > Include Library > Manage Libraries
   - Install "Adafruit GFX Library" and "Adafruit SSD1306"

3. **Upload the Firmware**:
   - Open `LED_Control.ino` in Arduino IDE
   - Select your ESP32 board from Tools > Board
   - Select the correct COM port
   - Click Upload

4. **Hardware Assembly**:
   - Connect RGB LED to GPIO 27, 26, 25 (with appropriate resistors)
   - Connect OLED display to I2C pins (21 SDA, 22 SCL)
   - Power the ESP32

### iOS App Setup

1. **Clone the Repository**:
   ```bash
   git clone <repository-url>
   cd Blue-ESP32
   ```

2. **Open in Xcode**:
   - Open `Blue.xcodeproj` in Xcode
   - Ensure your development team is set in the project settings

3. **Build and Run**:
   - Connect an iOS device or use Simulator
   - Build and run the app (Cmd+R)

## Usage

1. **Power on the ESP32**: The device will start advertising as "ESP32_RGB_Display"

2. **Launch the iOS App**: The app will automatically start scanning for BLE devices

3. **Connect**: Once connected, you'll see the control interface

4. **Control the LED**:
   - Use the color picker to select any color
   - The RGB LED will update in real-time

5. **Send Text**:
   - Enter text in the text field
   - Tap "Send Text" to display it on the OLED screen

## Architecture

### BLE Communication

The system uses a custom BLE service with two characteristics:

- **LED Characteristic** (`beb5483e-36e1-4688-b7f5-ea07361b26a8`): Accepts 3 bytes (RGB values 0-255)
- **Text Characteristic** (`1c95d5e3-d8f7-413a-bf3d-7d3d930a6d48`): Accepts UTF-8 encoded text strings

### ESP32 (Peripheral/Server)
- Advertises BLE service
- Receives write requests from central device
- Controls PWM for RGB LED
- Updates OLED display with received text

### iOS App (Central/Client)
- Scans for ESP32 device by name and service UUID
- Establishes connection and discovers characteristics
- Sends color data as RGB bytes
- Sends text data as UTF-8 encoded strings

## Troubleshooting

- **Connection Issues**: Ensure Bluetooth is enabled on both devices and they're in range
- **LED Not Working**: Check pin connections and resistor values
- **Display Not Showing**: Verify I2C connections and OLED address (0x3C)
- **App Not Finding Device**: Confirm ESP32 is powered and advertising

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

Aditya Mishra - 2025