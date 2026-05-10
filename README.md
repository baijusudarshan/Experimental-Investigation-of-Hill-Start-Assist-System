# Experimental Investigation of Hill Start Assist System

This repository contains the source code and documentation for an embedded control system designed for Go-Kart safety. The system features real-time inclination sensing using an MPU6050, Hill Start Assist (HSA) logic, and ABS integration powered by an ESP32 microcontroller.

## Project Team Members
Developed by the following members of the Department of Automobile and Mechanical Engineering:
* **Rikesh Lama** (THA078BAM031)
* **Sudarshan Baiju** (THA078BAM039)
* **Suresh Ghalan** (THA078BAM042)
* **Upendra Shahi** (THA078BAM043)

## Features
* **Inclination Sensing:** Uses MPU6050 to detect uphill/downhill gradients.
* **HSA Logic:** Automatically holds brake pressure to prevent rollback on inclines.
* **ABS Integration:** Real-time wheel speed monitoring using Hall Effect sensors.
* **Embedded Control:** Optimized C++ code for ESP32.

## Hardware Used
* ESP32 Development Board
* MPU6050 Accelerometer/Gyroscope
* Hall Effect Sensors (for RPM/Speed)
* Solonoid valve (Braking line)
* Hall Effect Sensors (for RPM/Speed)
  

