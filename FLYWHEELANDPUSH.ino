// --- PIN DEFINITIONS ---
const int stepperPins[] = {6, 7, 8, 9};
const int buttonPin = 5;

// L298N Flywheels
const int in1 = 11; const int in2 = 12; const int enA = 10;
const int in3 = 13; const int in4 = 14; const int enB = 15;

// --- CONFIGURATION ---
// Speed: 3000 microseconds = 3 milliseconds (Same speed as before)
const unsigned long stepInterval = 3000; 
const int targetSteps = 5500;
const int revUpTime = 1000;
int flywheelPower = 255;

// --- STATE MACHINE ---
enum TurretState {
  IDLE,
  REV_UP,
  PUSHING,
  RETRACTING,
  COOLDOWN
};

TurretState currentState = IDLE;

// --- TIMING VARIABLES (The Secret to Multitasking) ---
unsigned long lastStepTime = 0;   // Timestamp of the last step
unsigned long stateStartTime = 0; // Timestamp of when we entered a state
int currentStepCount = 0;         // Tracks where the pusher is (0 to 5500)

void setup() {
  for (int i = 0; i < 4; i++) pinMode(stepperPins[i], OUTPUT);
  
  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT); pinMode(enA, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT); pinMode(enB, OUTPUT);
  stopFlywheels();

  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  unsigned long currentTime = micros(); // Get current time in microseconds
  unsigned long currentMillis = millis(); // Get current time in milliseconds

  switch (currentState) {
    
    // --- STATE: IDLE ---
    case IDLE:
      if (digitalRead(buttonPin) == LOW) {
        Serial.println("Starting Sequence...");
        stateStartTime = currentMillis; // Mark the time we started revving
        currentState = REV_UP;
      }
      break;

    // --- STATE: REV UP (Non-blocking wait) ---
    case REV_UP:
      startFlywheels();
      // Check if 1000ms has passed since we started revving
      if (currentMillis - stateStartTime >= revUpTime) {
        currentStepCount = 0; // Reset step counter
        currentState = PUSHING;
      }
      // logic could be added here to ABORT if button is released!
      break;

    // --- STATE: PUSHING ---
    case PUSHING:
      // Is it time to take a step?
      if (currentTime - lastStepTime >= stepInterval) {
        lastStepTime = currentTime; // Update the timestamp
        
        stepMotor(currentStepCount % 4); // Move motor
        currentStepCount++; // Count the step

        // Have we reached the end?
        if (currentStepCount >= targetSteps) {
          currentState = RETRACTING;
        }
      }
      break;

    // --- STATE: RETRACTING ---
    case RETRACTING:
      // Is it time to take a step?
      if (currentTime - lastStepTime >= stepInterval) {
        lastStepTime = currentTime; // Update timestamp
        
        stepMotor(currentStepCount % 4); // Move motor (same direction logic as before)
        currentStepCount--; // Count down

        // Have we returned to 0?
        if (currentStepCount <= 0) {
          currentState = COOLDOWN;
        }
      }
      break;

    // --- STATE: COOLDOWN ---
    case COOLDOWN:
      stopFlywheels();
      // Turn off coils
      for (int i = 0; i < 4; i++) digitalWrite(stepperPins[i], LOW);
      
      currentState = IDLE;
      break;
  }
}

// --- HELPER FUNCTIONS ---

void startFlywheels() {
  digitalWrite(in1, HIGH); digitalWrite(in2, LOW); analogWrite(enA, flywheelPower);
  digitalWrite(in3, HIGH); digitalWrite(in4, LOW); analogWrite(enB, flywheelPower);
}

void stopFlywheels() {
  digitalWrite(in1, LOW); digitalWrite(in2, LOW); analogWrite(enA, 0);
  digitalWrite(in3, LOW); digitalWrite(in4, LOW); analogWrite(enB, 0);
}

void stepMotor(int step) {
  bool sequence[4][4] = {
    {1, 1, 0, 0},
    {0, 1, 1, 0},
    {0, 0, 1, 1},
    {1, 0, 0, 1}
  };
  for (int j = 0; j < 4; j++) {
    digitalWrite(stepperPins[j], sequence[step][j]);
  }
}