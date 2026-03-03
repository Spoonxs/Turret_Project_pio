/*
 * TurretShootingTest
 * Flywheel Turret Control System
 * - DC Motors (Flywheels) controlled via L298N
 * - 28BYJ-48 Mini Stepper Motor (Pusher) for feeding projectiles
 * - Full-step mode for 2048 steps per revolution
 * - Fire via Button or Serial Monitor
 * - Retract function to reset pusher position
 */

// Mini Stepper Motor Pins (Pusher)
#define STEPPER_IN1 4
#define STEPPER_IN2 5
#define STEPPER_IN3 14
#define STEPPER_IN4 15

// L298N Motor Driver Pins (DC Flywheels)
#define MOTOR_ENA 6
#define MOTOR_IN1 7
#define MOTOR_IN2 8
#define MOTOR_ENB 11
#define MOTOR_IN3 9
#define MOTOR_IN4 10

// Button Pin
#define BUTTON_PIN 12

// Configuration
#define FLYWHEEL_SPEED 255        // PWM speed (0-255)
#define STEPPER_SPEED 2           // Delay in ms between steps (lower = faster)
#define STEPS_PER_SHOT 2048       // Steps to push one projectile (28BYJ-48 = 2048 steps/rev)
#define RETRACT_STEPS 2048        // Steps to retract pusher back to start position
#define FLYWHEEL_SPINUP_TIME 1000 // Time to let flywheels reach speed (ms)

// Stepper sequence (full-step for 28BYJ-48)
const int stepSequence[4][4] = {
  {1, 0, 0, 1},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1}
};

int currentStep = 0;
bool flywheelsRunning = false;
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  // Initialize Serial
  Serial.begin(9600);
  Serial.println("=== Flywheel Turret Control ===");
  Serial.println("Commands:");
  Serial.println("  'f' or 'F' - Fire one shot");
  Serial.println("  'r' or 'R' - Retract pusher to start position");
  Serial.println("  's' or 'S' - Start flywheels");
  Serial.println("  'x' or 'X' - Stop flywheels");
  Serial.println("  'h' or 'H' - Help menu");
  Serial.println("================================");
  
  // Setup Stepper pins
  pinMode(STEPPER_IN1, OUTPUT);
  pinMode(STEPPER_IN2, OUTPUT);
  pinMode(STEPPER_IN3, OUTPUT);
  pinMode(STEPPER_IN4, OUTPUT);
  
  // Setup L298N pins
  pinMode(MOTOR_ENA, OUTPUT);
  pinMode(MOTOR_IN1, OUTPUT);
  pinMode(MOTOR_IN2, OUTPUT);
  pinMode(MOTOR_ENB, OUTPUT);
  pinMode(MOTOR_IN3, OUTPUT);
  pinMode(MOTOR_IN4, OUTPUT);
  
  // Setup Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Initialize motors to stopped state
  stopFlywheels();
  stopStepper();
  
  Serial.println("System ready!");
}

void loop() {
  // Check for serial commands
  if (Serial.available() > 0) {
    char command = Serial.read();
    handleSerialCommand(command);
  }
  
  // Check button with debouncing
  checkButton();
}

void handleSerialCommand(char cmd) {
  switch (cmd) {
    case 'f':
    case 'F':
      Serial.println("Firing...");
      fireShot();
      break;
      
    case 'r':
    case 'R':
      Serial.println("Retracting pusher...");
      retractPusher();
      break;
      
    case 's':
    case 'S':
      Serial.println("Starting flywheels...");
      startFlywheels();
      break;
      
    case 'x':
    case 'X':
      Serial.println("Stopping flywheels...");
      stopFlywheels();
      break;
      
    case 'h':
    case 'H':
      printHelp();
      break;
      
    default:
      // Ignore newlines and other characters
      if (cmd != '\n' && cmd != '\r') {
        Serial.println("Unknown command. Press 'h' for help.");
      }
      break;
  }
}

void checkButton() {
  int reading = digitalRead(BUTTON_PIN);
  
  // Check if button state changed
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }
  
  // If stable for debounce period
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If button pressed (LOW because of INPUT_PULLUP)
    if (reading == LOW) {
      Serial.println("Button pressed - Firing!");
      fireShot();
      delay(500); // Prevent multiple rapid fires
    }
  }
  
  lastButtonState = reading;
}

void fireShot() {
  // Start flywheels if not already running
  if (!flywheelsRunning) {
    Serial.println("Spinning up flywheels...");
    startFlywheels();
    delay(FLYWHEEL_SPINUP_TIME);
  }
  
  // Push projectile with stepper
  Serial.println("Pushing projectile...");
  runStepper(STEPS_PER_SHOT);
  
  Serial.println("Shot complete!");
}

void startFlywheels() {
  // Motor A - Forward
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, FLYWHEEL_SPEED);
  
  // Motor B - Forward
  digitalWrite(MOTOR_IN3, HIGH);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENB, FLYWHEEL_SPEED);
  
  flywheelsRunning = true;
}

void stopFlywheels() {
  analogWrite(MOTOR_ENA, 0);
  analogWrite(MOTOR_ENB, 0);
  digitalWrite(MOTOR_IN1, LOW);
  digitalWrite(MOTOR_IN2, LOW);
  digitalWrite(MOTOR_IN3, LOW);
  digitalWrite(MOTOR_IN4, LOW);
  
  flywheelsRunning = false;
}

void runStepper(int steps) {
  for (int i = 0; i < steps; i++) {
    setStepperStep(currentStep);
    currentStep = (currentStep + 1) % 4;
    delay(STEPPER_SPEED);
  }
  stopStepper();
}

void setStepperStep(int step) {
  digitalWrite(STEPPER_IN1, stepSequence[step][0]);
  digitalWrite(STEPPER_IN2, stepSequence[step][1]);
  digitalWrite(STEPPER_IN3, stepSequence[step][2]);
  digitalWrite(STEPPER_IN4, stepSequence[step][3]);
}

void stopStepper() {
  digitalWrite(STEPPER_IN1, LOW);
  digitalWrite(STEPPER_IN2, LOW);
  digitalWrite(STEPPER_IN3, LOW);
  digitalWrite(STEPPER_IN4, LOW);
}

void retractPusher() {
  Serial.print("Retracting ");
  Serial.print(RETRACT_STEPS);
  Serial.println(" steps...");
  runStepperReverse(RETRACT_STEPS);
  Serial.println("Retract complete!");
}

void runStepperReverse(int steps) {
  for (int i = 0; i < steps; i++) {
    setStepperStep(currentStep);
    currentStep = (currentStep - 1);
    if (currentStep < 0) currentStep = 3; // Wrap around for reverse
    delay(STEPPER_SPEED);
  }
  stopStepper();
}

void printHelp() {
  Serial.println("\n=== Flywheel Turret Commands ===");
  Serial.println("  'f' or 'F' - Fire one shot");
  Serial.println("  'r' or 'R' - Retract pusher to start position");
  Serial.println("  's' or 'S' - Start flywheels");
  Serial.println("  'x' or 'X' - Stop flywheels");
  Serial.println("  'h' or 'H' - Show this help");
  Serial.println("================================");
  Serial.print("Flywheel status: ");
  Serial.println(flywheelsRunning ? "RUNNING" : "STOPPED");
  Serial.println();
}
