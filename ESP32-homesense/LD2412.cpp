#include "LD2412.h"
#include <Arduino.h>

LD2412::LD2412(HardwareSerial &serial) : serial(serial) {}

void LD2412::begin() {
  Serial.println("LD2412 sensor initialized...");
}

void LD2412::configureSensor() {
  Serial.println("LD2412 sensor configuration complete.");
}

void LD2412::read() {
  static char buffer[bufferSize];  
  int bytesRead = 0;

  while (serial.available() && bytesRead < bufferSize) {
    buffer[bytesRead] = serial.read();
    bytesRead++;
  }

  if (bytesRead == bufferSize) {
    handleMessage(buffer);
  }
}

bool LD2412::presenceDetected() {
  // Presence detected if either distance is greater than 0
  return stationaryDistance > 0 || movingDistance > 0;
}

bool LD2412::stationaryTargetDetected() {
  return stationaryDistance > 0;
}

bool LD2412::movingTargetDetected() {
  return movingDistance > 0;
}

uint16_t LD2412::stationaryTargetDistance() {
  return stationaryDistance;
}

uint16_t LD2412::movingTargetDistance() {
  return movingDistance;
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

  char targetState = buffer[8];  
  handleTargetState(targetState, buffer);
}

void LD2412::handleTargetState(char targetState, char *buffer) {
  if (targetState == 0x00) {
    Serial.println("Target State: No target");
    stationaryDistance = 0;
    movingDistance = 0;
    return;
  }

  if (targetState == 0x01 || targetState == 0x03) {
    movingDistance = (buffer[9] & 0xFF) | ((buffer[10] & 0xFF) << 8);
    if (movingDistance > 0) {
      Serial.print("Movement target detected. Distance: ");
      Serial.print(movingDistance);
      Serial.println(" cm");
    }
  }

  if (targetState == 0x02 || targetState == 0x03) {
    stationaryDistance = (buffer[12] & 0xFF) | ((buffer[13] & 0xFF) << 8);
    if (stationaryDistance > 0) {
      Serial.print("Stationary target detected. Distance: ");
      Serial.print(stationaryDistance);
      Serial.println(" cm");
    } else {
      Serial.println("No valid stationary target detected.");
    }
  }
}