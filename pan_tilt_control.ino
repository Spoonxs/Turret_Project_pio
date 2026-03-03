/*
 * Pan-Tilt Stepper Motor Controller with Serial Commands
 * Commands: L=Left, R=Right, U=Up, D=Down, H=Home, E=Enable, S=Stop
 */

// Pin definitions
#define PAN_STEP 2
#define PAN_DIR 3
#define PAN_EN 28

#define TILT_STEP 12
#define TILT_DIR 13
#define TILT_EN 29

// Movement parameters
int stepSize = 50;        // Steps per command (adjust for your needs)
int stepDelay = 1000;     // Microseconds between steps (1000 = 1ms, slower = smoother)

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Configure pins
  pinMode(PAN_STEP, OUTPUT);
  pinMode(PAN_DIR, OUTPUT);
  pinMode(PAN_EN, OUTPUT);
  pinMode(TILT_STEP, OUTPUT);
  pinMode(TILT_DIR, OUTPUT);
  pinMode(TILT_EN, OUTPUT);
  
  // Initialize to disabled state
  digitalWrite(PAN_STEP, LOW);
  digitalWrite(PAN_DIR, LOW);
  digitalWrite(PAN_EN, HIGH);
  digitalWrite(TILT_STEP, LOW);
  digitalWrite(TILT_DIR, LOW);
  digitalWrite(TILT_EN, HIGH);
  
  printHelp();
  enableMotors();
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    
    // Convert to uppercase
    if (cmd >= 'a' && cmd <= 'z') {
      cmd = cmd - 32;
    }
    
    switch (cmd) {
      case 'L':
        moveLeft();
        break;
      case 'R':
        moveRight();
        break;
      case 'U':
        moveUp();
        break;
      case 'D':
        moveDown();
        break;
      case 'H':
        home();
        break;
      case 'E':
        enableMotors();
        break;
      case 'S':
        disableMotors();
        break;
      case '?':
        printHelp();
        break;
      case '\n':
      case '\r':
        // Ignore newlines
        break;
      default:
        Serial.println("Unknown command. Press ? for help");
        break;
    }
  }
}

void enableMotors() {
  digitalWrite(PAN_EN, LOW);
  digitalWrite(TILT_EN, LOW);
  Serial.println("Motors ENABLED");
}

void disableMotors() {
  digitalWrite(PAN_EN, HIGH);
  digitalWrite(TILT_EN, HIGH);
  Serial.println("Motors DISABLED");
}

void stepMotor(int stepPin, int dirPin, int steps, int direction, int delayTime) {
  digitalWrite(dirPin, direction);
  delayMicroseconds(100);
  
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayTime);
  }
}

void moveLeft() {
  Serial.print("Moving LEFT ");
  Serial.print(stepSize);
  Serial.println(" steps");
  stepMotor(PAN_STEP, PAN_DIR, stepSize, LOW, stepDelay);
}

void moveRight() {
  Serial.print("Moving RIGHT ");
  Serial.print(stepSize);
  Serial.println(" steps");
  stepMotor(PAN_STEP, PAN_DIR, stepSize, HIGH, stepDelay);
}

void moveUp() {
  Serial.print("Moving UP ");
  Serial.print(stepSize);
  Serial.println(" steps");
  stepMotor(TILT_STEP, TILT_DIR, stepSize, HIGH, stepDelay);
}

void moveDown() {
  Serial.print("Moving DOWN ");
  Serial.print(stepSize);
  Serial.println(" steps");
  stepMotor(TILT_STEP, TILT_DIR, stepSize, LOW, stepDelay);
}

void home() {
  Serial.println("Returning to HOME position (centering)");
  // This is a simple example - you may need to adjust based on your setup
  // Move both axes to a known position
  Serial.println("Moving to center position...");
  
  // You can customize this based on your needs
  // For now, just a placeholder that does nothing
  Serial.println("HOME command received (customize as needed)");
}

void printHelp() {
  Serial.println("\n=== Pan-Tilt Controller ===");
  Serial.println("Commands:");
  Serial.println("  L - Move LEFT");
  Serial.println("  R - Move RIGHT");
  Serial.println("  U - Move UP");
  Serial.println("  D - Move DOWN");
  Serial.println("  H - HOME (center)");
  Serial.println("  E - ENABLE motors");
  Serial.println("  S - STOP (disable motors)");
  Serial.println("  ? - Show this help");
  Serial.println("\nSettings:");
  Serial.print("  Steps per move: ");
  Serial.println(stepSize);
  Serial.print("  Step delay: ");
  Serial.print(stepDelay);
  Serial.println(" us");
  Serial.println("\nReady for commands!\n");
}
