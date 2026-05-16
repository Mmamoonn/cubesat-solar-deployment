# 🛰️ CubeSat-Based Solar Panel Deployment System

A mechatronic project simulating autonomous solar panel deployment on a **CubeSat** — a miniaturized satellite platform. The system uses an **ESP32 WROOM-32** microcontroller, **LDR sensors** for sun tracking, an **MG90s servo motor** for panel actuation, and **RGB status LEDs**, all powered by coupled Li-ion rechargeable cells.

---

## 📌 Project Overview

| Detail | Info |
|--------|------|
| University | University of Central Punjab (UCP) |
| Course | Robotics Design and Creativity (RDC) |
| Project Type | Final Project |
| Group Name | **Excalibur** |
| Session | Spring 2025 |
| Total Cost | PKR 2,615 |

---

## 👥 Group Members

| Name | Registration No. |
|------|-----------------|
| Muhammad Mamoon | L1F23BSRI0014 |
| Ali Ahmar Awan | L1F23BSRI0013 |
| Asad Ali | L1F23BSRI0024 |

---

## 🌍 Project Concept

CubeSats are small, standardized satellites (10×10×10 cm per unit) used for scientific research, Earth observation, and communication. A critical challenge in CubeSat design is the **autonomous deployment of solar panels** after launch — panels must fold during launch and unfold once in orbit to power the satellite.

This project simulates that deployment mechanism using:
- **LDR sensors** to detect light source direction (simulating sun tracking)
- **Servo motor** to physically deploy/orient solar panels
- **ESP32** to process sensor data and control deployment logic
- **LEDs** to indicate system status visually

---

## ⚙️ System Architecture

```
[4× LDR Sensors]           [2× Mini Solar Panels 6V/1W]
       │                            │
       ▼                            ▼
[ESP32-WROOM-32]  ←──── Power ────  [Coupled Li-ion 18650 Cells]
  FCC ID: 2AC7Z                     [Charging Board (TP4056)]
       │
  ┌────┴──────────────────┐
  ▼                       ▼
[MG90s Servo Motor]   [Status LEDs]
 Panel Deployment      🔴 Red   → Initializing / Error
                       🟡 Yellow → Searching / Adjusting
                       🟢 Green  → Panel Deployed / Tracking
```

---

## 🧠 How It Works

1. **Boot Sequence:** ESP32 powers on → Red LED ON (initializing)
2. **Sun Detection:** 4 LDR sensors compare light levels across directions
3. **Orientation:** ESP32 calculates brightest direction and drives servo to track it
4. **Panel Deployment:** Servo rotates solar panel to face the light source
5. **Status Feedback:** LEDs indicate system state at each phase
6. **Continuous Tracking:** System loops every second to re-check and re-orient

---

## 🌡️ LED Status Indicators

| LED | Color | State | Meaning |
|-----|-------|-------|---------|
| LED 1 | 🔴 Red | ON | System initializing or error |
| LED 2 | 🟡 Yellow | ON | Searching for light / adjusting |
| LED 3 | 🟢 Green | ON | Panel deployed & tracking sun |

---

## 🧰 Components List

| # | Component | Qty | Unit Price (PKR) | Total (PKR) |
|---|-----------|-----|-----------------|-------------|
| 1 | ESP32 WROOM-32 / Arduino Nano | 1 | 800 | 800 |
| 2 | LDR (Light Dependent Resistor) | 4 | 15 | 60 |
| 3 | Servo Motor MG90s (Metal Gear) | 1–2 | 280 | 560 |
| 4 | Mini Solar Panel (6V/1W) | 2 | 300 | 600 |
| 5 | 3.7V ICR 18650 Li-Ion Cell | 2 | 170 | 340 |
| 6 | 18650 Cell Battery Holder | 1 | 65 | 65 |
| 7 | Li-ion Charging Board (TP4056) | 1 | 40 | 40 |
| 8 | Vero Board | 1 | 150 | 150 |
| | **Total** | | | **PKR 2,615** |

---

## 📐 Pin Mapping (ESP32 WROOM-32)

### LEDs

| LED | Color | ESP32 GPIO | Resistor |
|-----|-------|-----------|----------|
| LED 1 | Red | GPIO 13 | 330Ω |
| LED 2 | Yellow | GPIO 26 | 330Ω |
| LED 3 | Green | GPIO 25 | 330Ω |

### Servo Motor (MG90s)

| Signal | ESP32 Pin |
|--------|-----------|
| VCC | 5V |
| GND | GND |
| Signal (PWM) | GPIO 27 |

### LDR Sensors

| Sensor | ESP32 Pin | Position |
|--------|-----------|----------|
| LDR 1 | GPIO 34 | Top |
| LDR 2 | GPIO 35 | Bottom |
| LDR 3 | GPIO 32 | Left |
| LDR 4 | GPIO 33 | Right |

> Each LDR is paired with a 10kΩ pull-down resistor to form a voltage divider.

---

## 💻 Source Code

Main sketch: `src/cubesat_solar_deployment.ino`

### Logic Summary

```cpp
readLDRs();                 // Read all 4 light sensors
findBrightestDirection();   // Compare readings
moveServoToTarget();        // Deploy/orient panel
updateLEDs();               // Reflect current state
delay(1000);                // Re-check every second
```

---

## 🎬 Media

| File | Description |
|------|-------------|
| `media/animation/cubesat_animation.mp4` | 3D animation of CubeSat deployment concept |
| `media/demo/project_demonstration.mp4` | Real working prototype demonstration |

---

## 📁 Repository Structure

```
cubesat-solar-deployment/
│
├── src/
│   └── cubesat_solar_deployment.ino
│
├── hardware/
│   ├── block-diagram/
│   │   └── block_diagram.jpeg
│   └── circuit-diagrams/
│       ├── circuit_diagram.jpeg
│       └── block_and_circuit_diagram.pdf
│
├── media/
│   ├── animation/
│   │   └── cubesat_animation.mp4
│   └── demo/
│       └── project_demonstration.mp4
│
├── docs/
│   └── components_list.pdf
│
├── README.md
├── TROUBLESHOOTING.md
└── LICENSE
```

---

## 🚀 Getting Started

1. Install **Arduino IDE** and add ESP32 board support:
   - Go to **File → Preferences → Additional Board URLs**
   - Add: `https://dl.espressif.com/dl/package_esp32_index.json`
   - Then **Tools → Board Manager** → search ESP32 → Install
2. Open `src/cubesat_solar_deployment.ino`
3. Select **Board:** ESP32 Dev Module, **Port:** correct COM port
4. Upload the code
5. Power via USB or coupled Li-ion cells
6. Observe LED sequence and servo movement as light source moves

---

## 🌐 Applications

- CubeSat and nanosatellite power systems
- Solar tracking systems for terrestrial panels
- Autonomous energy harvesting in remote IoT sensors
- Educational demonstration of space robotics concepts

---

## 📄 License

Licensed under **Creative Commons Attribution 4.0 International (CC BY 4.0)**.

---

## 🙏 Acknowledgements

- Course: Robotics Design and Creativity — University of Central Punjab
- Group Name: **Excalibur**
- Spring 2025
