# 🔧 Troubleshooting Guide
## CubeSat-Based Solar Panel Deployment System

---

## 📋 Table of Contents

1. [Servo Motor Issues](#1-servo-motor-issues)
2. [LDR Sensor Issues](#2-ldr-sensor-issues)
3. [LED Status Issues](#3-led-status-issues)
4. [Power & Battery Issues](#4-power--battery-issues)
5. [ESP32 Upload & Connection Issues](#5-esp32-upload--connection-issues)
6. [Panel Tracking Issues](#6-panel-tracking-issues)
7. [General Tips](#7-general-tips)

---

## 1. Servo Motor Issues

### ❌ Servo doesn't move at all
**Fix:**
- Confirm the servo is powered from 5V (not 3.3V — MG90s requires 5V)
- Verify Signal wire goes to GPIO 27
- Ensure `ESP32Servo` library is installed: **Sketch → Include Library → Manage Libraries → search "ESP32Servo"**
- Test with a minimal sketch:
  ```cpp
  #include <ESP32Servo.h>
  Servo s; void setup(){ s.attach(27); s.write(90); }
  void loop(){}
  ```

---

### ❌ Servo jitters or vibrates without moving cleanly
**Cause:** Power instability — servo draws spike current on movement.
**Fix:**
- Add a **100µF–470µF electrolytic capacitor** across servo VCC and GND rails
- Power the servo directly from the Li-ion battery output, NOT from ESP32's 3.3V pin
- Ensure battery cells are fully charged

---

### ❌ Servo rotates in wrong direction
**Fix:**
- Swap deployment logic in code — reverse DEPLOYED_ANGLE and STOWED_ANGLE:
  ```cpp
  const int DEPLOYED_ANGLE = 0;    // Was 180
  const int STOWED_ANGLE   = 180;  // Was 0
  ```

---

### ❌ Servo moves but panel doesn't deploy mechanically
**Cause:** Mechanical linkage is loose or misaligned.
**Fix:**
- Check servo horn is tightly secured to the panel hinge mechanism
- Verify the servo bracket/mount is fixed and doesn't shift during rotation

---

## 2. LDR Sensor Issues

### ❌ LDR always reads maximum value (4095)
**Cause:** Missing pull-down resistor or direct short to VCC.
**Fix:**
- Each LDR must be in a **voltage divider** with a 10kΩ resistor:
  ```
  3.3V → LDR → Junction (→ ESP32 Analog Pin) → 10kΩ → GND
  ```
- ESP32 ADC pins are 3.3V max — never connect LDR directly to 5V

---

### ❌ LDR always reads 0
**Cause:** LDR not connected or resistor value too high.
**Fix:**
- Test LDR by shining a bright light directly on it — value should rise significantly
- Try a lower resistor value (4.7kΩ) if readings are consistently low in bright light

---

### ❌ All 4 LDRs read similar values — no direction detected
**Cause:** LDRs too close together or ambient light is too even.
**Fix:**
- Space LDR sensors further apart on the CubeSat faces (top/bottom/left/right)
- Add small baffles/dividers between sensors to block stray light between them
- Test in a room with a single directional light source (desk lamp)

---

### ❌ System constantly switches direction (unstable tracking)
**Cause:** Light readings are noisy or two sensors nearly equal.
**Fix:**
- Add hysteresis — only switch direction if one sensor is significantly brighter:
  ```cpp
  if (top > left + 200 && top > right + 200) {
      // Only move if top is clearly dominant
  }
  ```
- Average multiple readings:
  ```cpp
  int readAvg(int pin){ int s=0; for(int i=0;i<5;i++){s+=analogRead(pin);delay(10);} return s/5; }
  ```

---

## 3. LED Status Issues

### ❌ None of the LEDs light up
**Fix:**
- Confirm 330Ω resistors are in series with each LED
- Check LED polarity: anode (longer leg) → resistor → GPIO, cathode (shorter leg) → GND
- Test each LED individually: `digitalWrite(LED_RED, HIGH)` in `setup()`

---

### ❌ Green LED never turns ON
**Cause:** System never reaching TRACKING state — LDR threshold not met.
**Fix:**
- Lower the `LIGHT_THRESHOLD` value in code (try 1000 or 500):
  ```cpp
  const int LIGHT_THRESHOLD = 1000;
  ```
- Open Serial Monitor (115200 baud) to see actual LDR readings and set threshold accordingly

---

### ❌ Red LED stays ON permanently
**Cause:** No light source detected or system stuck in ERROR_STATE.
**Fix:**
- Place a bright light (torch/desk lamp) directly in front of one LDR
- Check Serial Monitor for LDR values — confirm they exceed threshold when lit
- Verify LDR wiring and voltage divider circuit

---

## 4. Power & Battery Issues

### ❌ ESP32 resets when servo moves
**Cause:** Servo current draw exceeds what USB can supply.
**Fix:**
- Power from Li-ion cells instead of USB during testing
- Add 100µF–470µF capacitor across the 5V supply rail
- Use the TP4056 charging board's output for a regulated supply

---

### ❌ Battery drains very fast
**Fix:**
- Add `esp_light_sleep_start()` between sensor reads to reduce idle current
- Only move servo when direction changes significantly (add deadband of ±10°):
  ```cpp
  if (abs(targetAngle - currentAngle) > 10) moveServoTo(targetAngle);
  ```

---

### ❌ TP4056 charging board getting hot
**Cause:** Charging current too high or cells already full.
**Fix:**
- This is normal during charging — the board has thermal protection
- Charge in a ventilated area; unplug when LED indicates full charge

---

## 5. ESP32 Upload & Connection Issues

### ❌ "Failed to connect to ESP32: Timed out"
**Fix:**
- Hold the **BOOT button** on ESP32 while clicking Upload, release after upload starts
- Try a different USB cable (data cable, not charge-only)
- Select correct port: **Tools → Port → COMx** (Windows) or `/dev/ttyUSBx` (Linux)

---

### ❌ ESP32Servo library not found
**Fix:**
- In Arduino IDE: **Sketch → Include Library → Manage Libraries**
- Search **"ESP32Servo"** by Kevin Harrington → Install

---

### ❌ ADC readings are inaccurate on ESP32
**Note:** ESP32 ADC has known non-linearity, especially near 0V and 3.3V.
**Fix:**
- Use GPIO 32–39 (ADC1 pins) for LDRs — avoid ADC2 pins (used by WiFi)
- Apply ADC calibration or use relative comparisons between sensors rather than absolute values

---

## 6. Panel Tracking Issues

### ❌ Panel tracks to wrong direction
**Cause:** LDR position assignments don't match physical placement.
**Fix:**
- Verify physical LDR positions match code assignments (TOP=34, BOT=35, LEFT=32, RIGHT=33)
- Re-label or swap pin assignments in code to match your actual hardware layout

---

### ❌ Panel deploys but doesn't reach maximum angle
**Cause:** `DEPLOYED_ANGLE` limited or servo mechanically blocked.
**Fix:**
- Check for physical obstruction in the deployment mechanism
- Increase `DEPLOYED_ANGLE` up to 180° if the servo range allows
- Check servo specification — MG90s has 0°–180° range

---

## 7. General Tips

| Tip | Details |
|-----|---------|
| 🔆 Use directional light | Test with a desk lamp for clear direction — avoid diffuse ambient lighting |
| 📋 Monitor Serial | Open at 115200 baud to see LDR readings and servo angles in real time |
| ⚡ Power servo from battery | Never rely on USB 5V to power the MG90s servo under load |
| 🎚️ Tune threshold | Adjust `LIGHT_THRESHOLD` based on your actual LDR readings |
| 🔋 Charge before demo | Always start with fully charged 18650 cells |
| 🏗️ Secure mounting | Mount servo firmly — loose mounts cause erratic panel behavior |
| 🕐 Smooth movement | `STEP_SIZE = 5` and `delay(15)` give smooth rotation — reduce step for smoother, increase for faster |

---

## 📬 Still Having Issues?

- [ESP32 Pinout Reference](https://randomnerdtutorials.com/esp32-pinout-reference-gpios/)
- [ESP32Servo Library](https://github.com/madhephaestus/ESP32Servo)
- [MG90s Servo Datasheet](https://www.electronicoscaldas.com/datasheet/MG90S_Tower-Pro.pdf)
- [TP4056 Li-ion Charger Guide](https://lastminuteengineers.com/tp4056-lithium-battery-charger-tutorial/)

---

*Troubleshooting Guide — CubeSat Solar Panel Deployment*
*Group Excalibur: Muhammad Mamoon, Ali Ahmar Awan, Asad Ali*
*Robotics Design and Creativity — UCP Spring 2025*
