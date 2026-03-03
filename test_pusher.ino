/*
 * Turret Project - 28BYJ-48 Pusher Stepper Test
 * Board: Raspberry Pi Pico (arduino-pico core)
 *
 * Pins:
 *   GP4  -> ULN2003 IN1
 *   GP5  -> ULN2003 IN2
 *   GP14 -> ULN2003 IN3
 *   GP15 -> ULN2003 IN4
 *
 * Open Serial Monitor at 115200 baud to see progress.
 */

// ── Pin definitions ──────────────────────────────────────────────────────────
#define IN1 4
#define IN2 5
#define IN3 14
#define IN4 15

// ── Stepper constants ────────────────────────────────────────────────────────
// 28BYJ-48: 32 internal half-steps/rev × 64:1 gear = 2048 half-steps/output rev
#define STEPS_PER_REV 2048

// Half-step sequence [IN1, IN2, IN3, IN4]
const uint8_t HALF_STEP[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1},
};

const uint8_t COIL_PINS[4] = {IN1, IN2, IN3, IN4};

// ── Helpers ──────────────────────────────────────────────────────────────────
void setCoils(int stepIndex) {
  stepIndex = ((stepIndex % 8) + 8) % 8;  // safe modulo (handles negatives)
  for (int i = 0; i < 4; i++) {
    digitalWrite(COIL_PINS[i], HALF_STEP[stepIndex][i]);
  }
}

void releaseCoils() {
  // De-energize all coils — prevents heat buildup when idle
  for (int i = 0; i < 4; i++) {
    digitalWrite(COIL_PINS[i], LOW);
  }
}

int degreesToSteps(float deg) {
  return (int)((deg / 360.0f) * STEPS_PER_REV);
}

// direction: +1 = forward (push), -1 = backward (retract)
void stepMotor(int numSteps, int direction = 1, int delayMs = 2) {
  static int stepIndex = 0;
  for (int i = 0; i < numSteps; i++) {
    setCoils(stepIndex);
    stepIndex = ((stepIndex + direction) % 8 + 8) % 8;
    delay(delayMs);
  }
  releaseCoils();
}

void pushCycle(float pushDegrees = 180.0f, int delayMs = 3) {
  int steps = degreesToSteps(pushDegrees);

  Serial.print("Pushing   -> ");
  Serial.print(pushDegrees);
  Serial.print(" deg = ");
  Serial.print(steps);
  Serial.println(" half-steps");

  stepMotor(steps, +1, delayMs);
  delay(100);

  Serial.print("Retracting <- ");
  Serial.print(pushDegrees);
  Serial.print(" deg = ");
  Serial.print(steps);
  Serial.println(" half-steps");

  stepMotor(steps, -1, delayMs);
  delay(100);

  Serial.println("Cycle complete.\n");
}

// ── Arduino entry points ─────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(2000);  // give Serial Monitor time to connect

  for (int i = 0; i < 4; i++) {
    pinMode(COIL_PINS[i], OUTPUT);
    digitalWrite(COIL_PINS[i], LOW);
  }

  Serial.println("=== 28BYJ-48 Pusher Test ===\n");

  // Test 1: single slow cycle — good for verifying direction & mechanism
  Serial.println("Test 1: Single slow push cycle");
  pushCycle(180.0f, 4);
  delay(500);

  // Test 2: three fast cycles
  Serial.println("Test 2: Three fast push cycles");
  for (int i = 0; i < 3; i++) {
    Serial.print("  Cycle ");
    Serial.print(i + 1);
    Serial.println("/3");
    pushCycle(180.0f, 2);
    delay(300);
  }

  Serial.println("All tests done. Coils released.");
  releaseCoils();
}

void loop() {
  // Nothing — tests run once in setup()
}
