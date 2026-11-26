# IoT-Automatic-Fire-Detection-Extinguishing-System
IoT automatic fire detection and extinguishing prototype featuring Arduino UNO, 5 sensors, PCA9685 4-servo aiming arm, relay water pump, ESP8266 web dashboard (FIREWATCH-01), ESP32-CAM live stream, and ThingSpeak logging.

# FIREWATCH-01: IoT Automatic Fire Detection and Extinguishing System

An end-to-end IoT fire safety prototype that combines multi-sensor environmental monitoring, automated robotic water suppression, live video surveillance, and a real-time web control dashboard.

## Overview
Traditional fire alarms only trigger local warnings and rely on manual intervention, leaving room for delays and false alerts from single-sensor setups. **FIREWATCH-01** integrates an **Arduino UNO**, **Wemos D1 Mini (ESP8266)**, and **ESP32-CAM** into a coordinated physical-to-web system.

Five environmental sensors continuously monitor for fire hazards and water supply levels. When a flame is detected, a radar-sweeping servo locks onto the fire direction, a robotic arm aims a water nozzle at the source, and a relay-controlled pump automatically suppresses it—while streaming live telemetry, video, and manual override controls to a browser dashboard.

## Key Features
* **Multi-Sensor Decision Logic (6 Priority States):** Evaluates Flame, Smoke (`MQ-2`), Temperature (`DHT11`), and Ambient Light (`LDR`) across six priority modes (`DANGER`, `FLAME`, `SMOKE`, `HEAT`, `LIGHT`, and `NORMAL`) to distinguish active flames from early warnings and reduce false triggers.
* **Automated Radar Scan & Nozzle Aiming:** Controls four servo motors via an I2C **PCA9685** driver—one scanning servo sweeps the flame sensor left and right, locks position upon flame detection, and aligns the robotic arm base toward the fire while two joint servos stabilize the arm.
* **Relay-Controlled Water Suppression & Tank Monitoring:** Drives an external 5V 3A water pump through a relay module for electrical isolation, paired with an **HC-SR04** ultrasonic sensor that continuously monitors water tank availability.
* **Local Audio-Visual Feedback:** Displays real-time status, temperature, smoke level, and water tank state (`✓` / `X`) on a **16x2 I2C LCD**, paired with a color-coded **RGB LED** and frequency-coded **buzzer** (250 Hz, 300 Hz, 500 Hz, and 1000 Hz).
* **Embedded Web Dashboard (`FIREWATCH-01`):** Served directly from ESP8266 flash memory (`PROGMEM`) in 1 KB chunks, exposing `GET /data` (2-second polling) and `POST /control` endpoints over bidirectional JSON `SoftwareSerial` communication with the Arduino.
* **Live Video & Manual Override:** Embeds an **ESP32-CAM** MJPEG stream (port `81`) with dynamic IP connection, plus **AUTO / MANUAL** control toggles for the water pump, buzzer, and nozzle servo angle (with a `RESET TO AUTO` restore button) and a live event log.
* **Cloud Telemetry:** Uploads sensor readings every 20 seconds to **ThingSpeak** for historical tracking.

## System Architecture & Hardware
* **Controllers & Modules:** Arduino UNO, Wemos D1 Mini (ESP8266), AI Thinker ESP32-CAM, FTDI FT232RL USB-to-TTL Adapter
* **Sensors:** Infrared Flame Sensor, MQ-2 Smoke/Gas Sensor, DHT11 Temperature Sensor, HC-SR04 Ultrasonic Water Level Sensor, LDR Light Sensor
* **Actuators & Drivers:** PCA9685 16-Channel I2C PWM Servo Driver, 4x Servo Motors (Robotic Arm + Flame Scanner), Relay Module, 5V DC Water Pump (External 5V 3A Power Supply with Common Ground)
* **Local Indicators:** 16x2 I2C LCD Display, RGB LED, Active Buzzer
* **Software & Libraries:** Arduino IDE (C/C++), `Wire`, `Adafruit_PWMServoDriver`, `LiquidCrystal_I2C`, `DHT`, `SoftwareSerial`, `ArduinoJson`, HTML/CSS/JavaScript, ThingSpeak

## Verification & Testing
Validated across all 14 functional requirements and 18 integration test cases covering:
* Individual and combined sensor triggers (flame, smoke, heat > 35°C, bright light, and water tank distance thresholds)
* Automatic robotic aiming, relay pump activation, LCD/RGB/buzzer feedback
* Bidirectional Arduino–ESP8266 JSON sync, 2-second web polling, ESP32-CAM live streaming, ThingSpeak 20-second uploads, and `MANUAL` / `AUTO` override switching

## Demo & Prototype Gallery
<img width="703" height="405" alt="image" src="https://github.com/user-attachments/assets/7888d2f9-6ad3-403e-a183-4bacfa3cec2d" />
<img width="403" height="260" alt="image" src="https://github.com/user-attachments/assets/f4daa6d8-39c2-4b0e-890f-aa6df550d4f4" />
<img width="402" height="260" alt="image" src="https://github.com/user-attachments/assets/fda6f2c8-6d1a-4fa3-95b0-16574e328634" />
<img width="403" height="260" alt="image" src="https://github.com/user-attachments/assets/f13c65dd-9c6e-4f19-82bd-8e1c4d373a58" />
<img width="360" height="284" alt="image" src="https://github.com/user-attachments/assets/ae9328bb-7dd7-46b7-91d1-5114844131b8" />

## My Role & Contributors
Developed as a Group Project for **SWE403: Technology and Application of Internet of Things** at Xiamen University Malaysia (XMUM).
* **Wong Yi Pin (My Contribution):** Core embedded control logic & coding, relay-controlled water pump hardware integration, and system architecture/requirements documentation.
* **Team Members:** Derol Chong Chun Kit, Li Yiqian, Zhang Tianying.
