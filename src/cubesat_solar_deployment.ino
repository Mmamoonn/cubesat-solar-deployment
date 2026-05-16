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
const int STOWED_ANGLE   = 0;    
const int STEP_SIZE      = 5;    // Degrees per adjustment step

// ── Threshold ─────────────────────────────────────────────────
const int LIGHT_THRESHOLD = 2000; 

// ── System States ─────────────────────────────────────────────
enum State { INITIALIZING, SEARCHING, TRACKING, ERROR_STATE };
State systemState = INITIALIZING;

void setup() {
  Serial.begin(115200);
  Serial.println("=== CubeSat Solar Panel Deployment System ===");

  pinMode(LED_RED,    OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN,  OUTPUT);

  panelServo.attach(SERVO_PIN);
  panelServo.write(STOWED_ANGLE);  
  currentAngle = STOWED_ANGLE;

  setLEDs(HIGH, LOW, LOW);         
  Serial.println("Initializing...");
  delay(2000);

  setLEDs(LOW, HIGH, LOW);         
  systemState = SEARCHING;
  Serial.println("Searching for light source...");
}

void loop() {
  int top    = analogRead(LDR_TOP);
  int bottom = analogRead(LDR_BOTTOM);
  int left   = analogRead(LDR_LEFT);
  int right  = analogRead(LDR_RIGHT);

  Serial.print("LDR → Top:"); Serial.print(top);
  Serial.print(" Bot:"); Serial.print(bottom);
  Serial.print(" L:"); Serial.print(left);
  Serial.print(" R:"); Serial.println(right);

  int maxVal = max(max(top, bottom), max(left, right));

  if (maxVal < LIGHT_THRESHOLD) {
    systemState = ERROR_STATE;
    setLEDs(HIGH, LOW, LOW);
    stowPanel();
    Serial.println("No light detected. Panel stowed.");

  } else if (maxVal == top) {
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(DEPLOYED_ANGLE);
    Serial.println("Light: TOP → Panel deployed upward.");

  } else if (maxVal == left) {
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(45);
    Serial.println("Light: LEFT → Panel adjusted left.");

  } else if (maxVal == right) {
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(135);
    Serial.println("Light: RIGHT → Panel adjusted right.");

  } else if (maxVal == bottom) {
    systemState = TRACKING;
    setLEDs(LOW, HIGH, LOW);
    moveServoTo(90);
    Serial.println("Light: BOTTOM → Panel at neutral.");
  }

  if (systemState == TRACKING) {
    delay(500);
    setLEDs(LOW, LOW, HIGH);
    Serial.println("Status: Panel Deployed & Tracking ✓");
  }

  delay(1000); 
}

void setLEDs(int red, int yellow, int green) {
  digitalWrite(LED_RED,    red);
  digitalWrite(LED_YELLOW, yellow);
  digitalWrite(LED_GREEN,  green);
}

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
    delay(15);
  }
  Serial.print("Servo moved to: ");
  Serial.print(currentAngle);
  Serial.println("°");
}
void stowPanel() {
  moveServoTo(STOWED_ANGLE);
  Serial.println("Panel stowed at 0°");
}
