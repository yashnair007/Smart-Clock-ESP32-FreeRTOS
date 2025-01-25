# Smart-Clock-ESP32-FreeRTOS
Smart Clock with FreeRTOS Scheduling in ESP32

This project is a **Smart Clock** that combines an LCD display, DHT11 temperature sensor, and Wi-Fi connectivity to display real-time data such as:

- **Current time and date** (synchronized using NTP).
- **Temperature** readings from a DHT11 sensor.
- A **scrolling message** for customization.

The system is built using **FreeRTOS**, enabling multitasking with efficient resource management.

## Features

- **Real-Time Clock (RTC):** Synchronizes time using the NTP server.
- **Temperature Monitoring:** Continuously monitors and displays the temperature in °C.
- **Scrolling Message:** Displays a customizable scrolling message.
- **FreeRTOS Multitasking:** Handles tasks like temperature reading and LCD updates concurrently using FreeRTOS.
- **Wi-Fi Connectivity:** Connects to an NTP server for accurate time updates.

## Components Used

1. **ESP32 Microcontroller**: Provides Wi-Fi connectivity and FreeRTOS support.
2. **DHT11 Sensor**: Measures temperature.
3. **LCD (20x4) with I2C Module**: Displays time, date, temperature, and scrolling messages.
4. **Wi-Fi Network**: Used for NTP synchronization.

## Libraries Required

- [Wire](https://github.com/arduino/ArduinoCore-avr/tree/master/libraries/Wire): For I2C communication.
- [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C): For LCD control.
- [DHT Sensor Library](https://github.com/adafruit/DHT-sensor-library): For reading temperature data.
- [FreeRTOS](https://github.com/espressif/arduino-esp32/tree/master/libraries/FreeRTOS): For multitasking.
- [WiFi](https://github.com/espressif/arduino-esp32/tree/master/libraries/WiFi): For connecting to the Wi-Fi network.
- [NTPClient](https://github.com/arduino-libraries/NTPClient): For fetching time from NTP servers.
- [TimeLib](https://github.com/PaulStoffregen/Time): For time calculations.

## Setup Instructions

### Hardware Setup

1. **Connect the LCD:**
   - SCL to GPIO 22 (ESP32 default).
   - SDA to GPIO 21 (ESP32 default).

2. **Connect the DHT11 Sensor:**
   - Data pin to GPIO 4.
   - VCC to 3.3V.
   - GND to GND.

3. **Power the ESP32:** 
   - Via USB or an external power source.

### Software Setup

1. Install the required libraries in the Arduino IDE.
2. Clone this repository and open the `SmartClock.ino` file in the Arduino IDE.
3. Update the Wi-Fi credentials in the code:
   ```cpp
   const char* ssid = "Your_SSID";
   const char* password = "Your_PASSWORD";
