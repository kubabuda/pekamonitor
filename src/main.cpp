#include <Arduino.h>

const int buttonPin = D7;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  Serial.print("Hello from WeMos!");
}

void loop() {
  Serial.print("Button is");
  Serial.println(digitalRead(buttonPin) == HIGH ? "ON" : "OFF");
  delay(500);
}