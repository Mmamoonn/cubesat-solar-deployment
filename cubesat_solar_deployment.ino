/*
 * ============================================================
 * CubeSat-Based Solar Panel Deployment System
 * ============================================================
 * University  : University of Central Punjab (UCP)
 * Course      : Robotics Design and Creativity (RDC)
 * Group       : Excalibur
 * Authors     : Muhammad Mamoon  (L1F23BSRI0014)
 *               Ali Ahmar Awan  (L1F23BSRI0013)
 *               Asad Ali        (L1F23BSRI0024)
 * Session     : Spring 2025
 * ============================================================
 *
 * Description:
 * Simulates autonomous solar panel deployment on a CubeSat.
 * 4 LDR sensors detect the brightest light direction.
 * MG90s servo motor orients/deploys solar panels toward the
 * light source. RGB LEDs indicate system status at each phase.
 *
 * Pin Map:
 *   LDRs        : GPIO 34 (Top), 35 (Bottom), 32 (Left), 33 (Right)
 *   Servo PWM   : GPIO 27
 *   Red LED     : GPIO 13
 *   Yellow LED  : GPIO 26
 *   Green LED   : GPIO 25
 *   Power       : 5V from coupled 18650 Li-ion cells
 * ============================================================
 */

#include <ESP32Servo.h>

// ── Servo Pin ─────────────────────────────────────────────────
const int SERVO_PIN = 27;

// ── LED Pins ──────────────────────────────────────────────────
const int LED_RED    = 13;   // Initializing / Error
const int LED_YELLOW = 26;   // Searching / Adjusting
const int LED_GREEN  = 25;   // Deployed & Tracking

// ── LDR Sensor Pins (Analog Input) ───────────────────────────
const int LDR_TOP    = 34;
const int LDR_BOTTOM = 35;
const int LDR_LEFT   = 32;
const int LDR_RIGHT  = 33;

// ── Servo Settings ────────────────────────────────────────────
Servo panelServo;
int currentAngle  = 90;    // Start at 90° (neutral/closed)
int targetAngle   = 90;
const int DEPLOYED_ANGLE = 180;  // Fully deployed
const int STOWED_ANGLE   = 0;    // Fully stowed
const int STEP_SIZE      = 5;    // Degrees per adjustment step

// ── Threshold ─────────────────────────────────────────────────
const int LIGHT_THRESHOLD = 2000;  // Above = bright (0–4095 on ESP32 12-bit ADC)

// ── System States ─────────────────────────────────────────────
enum State { INITIALIZING, SEARCHING, TRACKING, ERROR_STATE };
State systemState = INITIALIZING;

// ── Setup ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  Serial.println("=== CubeSat Solar Panel Deployment System ===");
  Serial.println("Group: Excalibur | UCP Spring 2025\n");

  // LED pins
  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);

  // Servo initialization
  panelServo.attach(SERVO_PIN);
  panelServo.write(STOWED_ANGLE);   // Start stowed
  currentAngle = STOWED_ANGLE;

  // Initialization sequence
  setLEDs(HIGH, LOW, LOW);          // Red ON — initializing
  Serial.println("Initializing...");
  delay(2000);

  setLEDs(LOW, HIGH, LOW);          // Yellow ON — searching
  systemState = SEARCHING;
  Serial.println("Searching for light source...");
}

// ── Main Loop ─────────────────────────────────────────────────
void loop() {
  // Read all 4 LDR sensors
  int top    = analogRead(LDR_TOP);
  int bottom = analogRead(LDR_BOTTOM);
  int left   = analogRead(LDR_LEFT);
  int right  = analogRead(LDR_RIGHT);

  Serial.print("LDR → Top:"); Serial.print(top);
  Serial.print(" Bot:"); Serial.print(bottom);
  Serial.print(" L:"); Serial.print(left);
  Serial.print(" R:"); Serial.println(right);

  // Find brightest sensor
  int maxVal = max(max(top, bottom), max(left, right));

  if (maxVal < LIGHT_THRESHOLD) {
    // ── No significant light → Error / Stow ─────────────────
    systemState = ERROR_STATE;
    setLEDs(HIGH, LOW, LOW);
    stowPanel();
    Serial.println("No light detected. Panel stowed.");

  } else if (maxVal == top) {
    // ── Light from Top ───────────────────────────────────────
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(DEPLOYED_ANGLE);
    Serial.println("Light: TOP → Panel deployed upward.");

  } else if (maxVal == left) {
    // ── Light from Left ──────────────────────────────────────
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(45);
    Serial.println("Light: LEFT → Panel adjusted left.");

  } else if (maxVal == right) {
    // ── Light from Right ─────────────────────────────────────
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(135);
    Serial.println("Light: RIGHT → Panel adjusted right.");

  } else if (maxVal == bottom) {
    // ── Light from Bottom → neutral position ─────────────────
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(90);
    Serial.println("Light: BOTTOM → Panel at neutral.");
  }

  // If tracking confirmed → Green LED
  if (systemState == TRACKING) {
    delay(500);
    setLEDs(LOW, LOW, HIGH);
    Serial.println("Status: Panel Deployed & Tracking ✓");
  }

  delay(1000);  // Re-check every second
}

// ── Helper: Set all LEDs at once ─────────────────────────────
void setLEDs(int red, int yellow, int green) {
  digitalWrite(LED_RED,    red);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_GREEN,  green);
}

// ── Helper: Move servo smoothly to target angle ───────────────
void moveServoTo(int target) {
  if (target == currentAngle) return;

  int step = (target > currentAngle) ? STEP_SIZE : -STEP_SIZE;
  while (currentAngle != target) {
    currentAngle += step;
    if ((step > 0 && currentAngle > target) ||
        (step < 0 && currentAngle < target)) {
      currentAngle = target;
    }
    panelServo.write(currentAngle);
    delay(15);   // Smooth movement
  }
  Serial.print("Servo moved to: ");
  Serial.print(currentAngle);
  Serial.println("°");
}

// ── Helper: Stow the solar panel ─────────────────────────────
void stowPanel() {
  moveServoTo(STOWED_ANGLE);
  Serial.println("Panel stowed at 0°");
}
