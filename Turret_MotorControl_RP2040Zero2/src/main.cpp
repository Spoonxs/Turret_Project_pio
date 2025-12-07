#include <Arduino.h>

const int TEST_PIN = 2;  // GP2 on the RP2040 Zero

void setup() {
  pinMode(TEST_PIN, OUTPUT);

  Serial.begin(115200);
  delay(2000);  // give Serial Monitor time to attach
  Serial.println("RP2040 Zero: PlatformIO test is running!");
}

void loop() {
  digitalWrite(TEST_PIN, HIGH);
  Serial.println("Pin 2 HIGH");
  delay(500);

  digitalWrite(TEST_PIN, LOW);
  Serial.println("Pin 2 LOW");
  delay(500);
}
