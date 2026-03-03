/*
 * TurretShootingTest
 * Flywheel Turret Control System
 * - DC Motors (Flywheels) controlled via L298N
 * - 28BYJ-48 Mini Stepper Motor (Pusher) for feeding projectiles
 * - Full-step mode for 2048 steps per revolution
 * 
 * Commands:
 *   P - Push (fire pusher forward)
 *   R - Retract (return pusher)
 *   F - Toggle flywheels ON/OFF
 *   S - Stop all
 *   + - Increase flywheel speed
 *   - - Decrease flywheel speed
 *   ? - Help
 */

// Mini Stepper Motor Pins (Pusher - 28BYJ-48)
#define STEPPER_IN1 4
#define STEPPER_IN2 5
#define STEPPER_IN3 14
#define STEPPER_IN4 15

// L298N Motor Driver Pins (DC Flywheels)
#define MOTOR_ENA 6
#define MOTOR_IN1 7
#define MOTOR_IN2 8
#define MOTOR_IN3 9
#define MOTOR_IN4 10
#define MOTOR_ENB 11

// Configuration
int flywheelSpeed = 255;          // PWM speed (0-255)
#define STEPPER_SPEED 1           // Delay in ms between steps (lower = faster)
#define STEPS_PER_PUSH 11000       // Steps to push (28BYJ-48 = 2048 steps/rev)

// Stepper sequence (full-step for 28BYJ-48)
const int stepSequence[4][4] = {
  {1, 0, 0, 1},
  {1, 0, 1, 0},
  {0, 1, 1, 0},
  {0, 1, 0, 1}
};

int currentStep = 0;
bool flywheelsRunning = false;

void setup() {
  Serial.begin(115200);
  delay(500);
  
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
  
  // Initialize motors to stopped state
  stopFlywheels();
  stopStepper();
  
  printHelp();
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = toupper(Serial.read());
    
    switch (cmd) {
      case 'P':
        push();
        break;
        
      case 'R':
        retract();
        break;
        
      case 'F':
        toggleFlywheels();
        break;
        
      case 'S':
        stopAll();
        break;
        
      case '+':
        flywheelSpeed = min(255, flywheelSpeed + 25);
        Serial.print("Speed: ");
        Serial.println(flywheelSpeed);
        if (flywheelsRunning) startFlywheels();
        break;
        
      case '-':
        flywheelSpeed = max(100, flywheelSpeed - 25);
        Serial.print("Speed: ");
        Serial.println(flywheelSpeed);
        if (flywheelsRunning) startFlywheels();
        break;
        
      case '?':
        printHelp();
        break;
        
      case '\n':
      case '\r':
        break;
        
      default:
        Serial.println("Unknown. ? for help");
        break;
    }
  }
}

// --- Flywheel Control ---
void startFlywheels() {
  // Motor A - Forward
  digitalWrite(MOTOR_IN1, HIGH);
  digitalWrite(MOTOR_IN2, LOW);
  analogWrite(MOTOR_ENA, flywheelSpeed);
  
  // Motor B - Forward
  digitalWrite(MOTOR_IN3, HIGH);
  digitalWrite(MOTOR_IN4, LOW);
  analogWrite(MOTOR_ENB, flywheelSpeed);
  
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

void toggleFlywheels() {
  if (flywheelsRunning) {
    stopFlywheels();
    Serial.println("Flywheels: OFF");
  } else {
    startFlywheels();
    Serial.print("Flywheels: ON (");
    Serial.print(flywheelSpeed);
    Serial.println(")");
  }
}

// --- Pusher Stepper Control ---
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

void push() {
  Serial.println("PUSH");
  for (int i = 0; i < STEPS_PER_PUSH; i++) {
    setStepperStep(currentStep);
    currentStep = (currentStep + 1) % 4;
    delay(STEPPER_SPEED);
  }
  stopStepper();
  Serial.println("Done");
}

void retract() {
  Serial.println("RETRACT");
  for (int i = 0; i < STEPS_PER_PUSH; i++) {
    setStepperStep(currentStep);
    currentStep = (currentStep - 1);
    if (currentStep < 0) currentStep = 3;
    delay(STEPPER_SPEED);
  }
  stopStepper();
  Serial.println("Done");
}

void stopAll() {
  stopFlywheels();
  stopStepper();
  Serial.println("ALL STOPPED");
}

void printHelp() {
  Serial.println("\n=== Turret Test ===");
  Serial.println("P - Push");
  Serial.println("R - Retract");
  Serial.println("F - Toggle flywheels");
  Serial.println("S - Stop all");
  Serial.println("+ - Speed up");
  Serial.println("- - Slow down");
  Serial.println("? - Help");
  Serial.print("\nFlywheels: ");
  Serial.println(flywheelsRunning ? "ON" : "OFF");
  Serial.print("Speed: ");
  Serial.println(flywheelSpeed);
  Serial.println("Ready!\n");
}
