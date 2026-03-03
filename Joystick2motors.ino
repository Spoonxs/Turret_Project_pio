// --- Motor 1 (e.g. pan) ---
#define step_pin1 2   // Step pin for motor 1
#define dir_pin1  3   // Direction pin for motor 1

// --- Motor 2 (e.g. tilt) ---
#define step_pin2 4   // Step pin for motor 2
#define dir_pin2  5   // Direction pin for motor 2

// Optional microstep pins, shared by both drivers (if you wired them)
#define MS1 10
#define MS2 11
// SLEEP is tied to RESET and pulled high in hardware, so no SLEEP pin here

// Joystick axes (RP2040 analog pins)
#define X_pin 26      // Joystick X axis
#define Y_pin 27      // Joystick Y axis

// Higher = slower (because it's the delay between steps, in ms)
int step_speed = 10;  

void setup() {
  // Motor 1 pins
  pinMode(dir_pin1, OUTPUT);
  pinMode(step_pin1, OUTPUT);
  
  // Motor 2 pins
  pinMode(dir_pin2, OUTPUT);
  pinMode(step_pin2, OUTPUT);

  // Microstepping (if MS1/MS2 actually connected)
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  digitalWrite(MS1, LOW);   // Full steps
  digitalWrite(MS2, LOW);   // Full steps

  // Joystick pins: analogRead(X_pin / Y_pin) works directly, no pinMode needed
}

void loop() {
  int x = analogRead(X_pin);  // 0–1023 equivalent (you already tuned thresholds)
  int y = analogRead(Y_pin);  // same idea for Y axis
  Serial.print("x axis");
  Serial.println(x);
  Serial.print("y axis");
  Serial.println(y);
  // -------- Motor 1 control from X axis --------
  if (x > 930) {  // tweak these to what you found works
    // Move motor 1 one way
    digitalWrite(dir_pin1, LOW);   // pick which way is "left"
    digitalWrite(step_pin1, HIGH);
    delay(step_speed);
    digitalWrite(step_pin1, LOW);
    delay(step_speed);
  }
  else if (x < 212) {
    // Move motor 1 the other way
    digitalWrite(dir_pin1, HIGH);
    digitalWrite(step_pin1, HIGH);
    delay(step_speed);
    digitalWrite(step_pin1, LOW);
    delay(step_speed);
  }
  // else (312–712): motor 1 stopped

  // -------- Motor 2 control from Y axis --------
  if (y > 930) {  // same thresholds to start; tweak like you did for X
    // Move motor 2 one way (e.g. up)
    digitalWrite(dir_pin2, LOW);
    digitalWrite(step_pin2, HIGH);
    delay(step_speed);
    digitalWrite(step_pin2, LOW);
    delay(step_speed);
  }
  else if (y < 212) {
    // Move motor 2 the other way (e.g. down)
    digitalWrite(dir_pin2, HIGH);
    digitalWrite(step_pin2, HIGH);
    delay(step_speed);
    digitalWrite(step_pin2, LOW);
    delay(step_speed);
  }
  // else (312–712): motor 2 stopped
}
