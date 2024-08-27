#include "LD2450.h"
#include <cmath>  // Required for sqrt and pow functions

// Constructor
LD2450::LD2450() : radarSerial(nullptr), numTargets(3), last_target_data("") {}

// Destructor
LD2450::~LD2450() {}

// Initialize the radar sensor
bool LD2450::begin(HardwareSerial &serial) {
    radarSerial = &serial;
    return true;  // Return true if initialization is successful
}

bool LD2450::presenceDetected() {
    // Implement based on LD2450 sensor protocol
    return radarTargets[0].valid || radarTargets[1].valid || radarTargets[2].valid;
}

bool LD2450::stationaryTargetDetected() {
    // Implement based on LD2450 sensor protocol
    return radarTargets[0].valid;
}

bool LD2450::movingTargetDetected() {
    // Implement based on LD2450 sensor protocol
    return radarTargets[1].valid;
}

int LD2450::stationaryTargetDistance() {
    if (stationaryTargetDetected()) {
        return radarTargets[0].distance;
    }
    return 0;
}

// Get moving target distance
int LD2450::movingTargetDistance() {
    if (movingTargetDetected()) {
        return radarTargets[1].distance;
    }
    return 0;
}

// Get moving target energy
int LD2450::movingTargetEnergy() {
    if (movingTargetDetected()) {
        return radarTargets[1].speed;
    }
    return 0;
}

uint8_t LD2450::read() {
    if (radarSerial == nullptr) {
        return 0;
    }

    byte rec_buf[256];
    int len = radarSerial->available();
    if (len > 0) {
        len = radarSerial->readBytes(rec_buf, sizeof(rec_buf));
        printRawData(rec_buf, len);  // Print raw data for debugging
        parseData(rec_buf, len);
    }
    return len;
}

// Get the number of supported targets
uint16_t LD2450::getSensorSupportedTargetCount() {
    return numTargets;
}

// Get information about a specific target
LD2450::RadarTarget LD2450::getTarget(uint16_t index) {
    if (index < numTargets) {
        return radarTargets[index];
    } else {
        RadarTarget invalidTarget = {false, 0, 0, 0, 0, 0}; // Default invalid target
        return invalidTarget;
    }
}

void LD2450::parseData(byte rec_buf[], int len) {
    if (len < 30) {
        Serial.println("Insufficient data length.");
        return;
    }

    for (int i = 0; i < numTargets; ++i) {
        radarTargets[i].valid = false;
    }

    int index = 0;
    while (index < len) {
        // Assuming the data format starts with 0xAA, 0xFF, 0x03, 0x00, and ends with 0x55, 0xCC
        if (rec_buf[index] == 0xAA && rec_buf[index + 1] == 0xFF && rec_buf[index + 2] == 0x03 &&
            rec_buf[index + 3] == 0x00 && rec_buf[index + 28] == 0x55 && rec_buf[index + 29] == 0xCC) {

            for (uint16_t targetCounter = 0; targetCounter < numTargets; ++targetCounter) {
                if (index + 12 <= len) {
                    RadarTarget target;
                    target.x = (int16_t)(rec_buf[index + 4] | (rec_buf[index + 5] << 8));
                    target.y = (int16_t)(rec_buf[index + 6] | (rec_buf[index + 7] << 8));
                    target.speed = (int16_t)(rec_buf[index + 8] | (rec_buf[index + 9] << 8));
                    target.resolution = (uint16_t)(rec_buf[index + 10] | (rec_buf[index + 11] << 8));

                    // Correct the interpretation of data
                    if (rec_buf[index + 5] & 0x80) target.x -= 0x8000; else target.x = -target.x;
                    if (rec_buf[index + 7] & 0x80) target.y -= 0x8000; else target.y = -target.y;
                    if (rec_buf[index + 9] & 0x80) target.speed -= 0x8000; else target.speed = -target.speed;

                    target.distance = sqrt(pow(target.x, 2) + pow(target.y, 2)) / 10.0;

                    target.valid = (target.resolution != 0);
                    radarTargets[targetCounter] = target;
                    radarTargets[targetCounter].id = targetCounter + 1;

                    // Build debug output
                    String status = (abs(target.speed) > 0) ? "Moving" : "Stationary";
             