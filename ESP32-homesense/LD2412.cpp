#include "LD2412.h"
#include <Arduino.h>

LD2412::LD2412(HardwareSerial &serial) : serial(serial) {}

void LD2412::setup() {
  // The setup will no longer initialize the serial communication
  // Serial initialization is done in main.ino
  Serial.println("LD2412 sensor initialized...");
}

void LD2412::configureSensor() {
  // Add any necessary sensor configuration commands here
  Serial.println("LD2412 sensor configuration complete.");
}

void LD2412::readData() {
  static char buffer[bufferSize];  // Buffer to hold the incoming message
  int bytesRead = 0;

  // Read the full message from the sensor
  while (serial.available() && bytesRead < bufferSize) {
    buffer[bytesRead] = serial.read();
    bytesRead++;
  }

  // If a full message is captured, process it
  if (bytesRead == bufferSize) {
    handleMessage(buffer);
  }
}

bool LD2412::presenceDetected() {
  return detectedTarget;  // Return the detected target status
}

bool LD2412::stationaryTargetDetected() {
  return stationaryDistance > 0;  // Return true if a stationary target is detected
}

bool LD2412::movingTargetDetected() {
  return movingDistance > 0;  // Return true if a moving target is detected
}

uint16_t LD2412::stationaryTargetDistance() {
  return stationaryDistance;  // Return the stationary target distance
}

uint16_t LD2412::movingTargetDistance() {
  return movingDistance;  // Return the moving target distance
}

void LD2412::handleMessage(char *buffer) {
  if (debug) {
    Serial.println("Full Message Parsed:");
    for (int i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  char targetState = buffer[8];  // The target state byte
  handleTargetState(targetState, buffer);
}

void LD2412::handleTargetState(char targetState, char *buffer) {
  if (targetState == 0x00) {
    Serial.println("Target State: No target");
    detectedTarget = false;
    return;
  }

  if (targetState == 0x01 || targetState == 0x03) {
    movingDistance = (buffer[9] & 0xFF) | ((buffer[10] & 0xFF) << 8);
    if (movingDistance > 0) {
      Serial.print("Movement target detected. Distance: ");
      Serial.print(movingDistance);
      Serial.println(" cm");
      detectedTarget = true;
    }
  }

  if (targetState == 0x02 || targetState == 0x03) {
    stationaryDistance = (buffer[12] & 0xFF) | ((buffer[13] & 0xFF) << 8);
    if (stationaryDistance > 0) {
      Serial.print("Stationary target detected. Distance: ");
      Serial.print(stationaryDistance);
      Serial.println(" cm");
      detectedTarget = true;
    } else {
      Serial.println("No valid stationary target detected.");
      detectedTarget = false;
    }
  }
}