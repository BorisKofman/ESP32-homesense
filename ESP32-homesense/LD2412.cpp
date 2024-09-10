#include "LD2412.h"

LD2412::LD2412(HardwareSerial& serial) : _serial(serial), presence(false), stationaryDistance(0), movingDistance(0) {}

void LD2412::begin(int baudRate, int dataBits, int rxPin, int txPin) {
    _serial.begin(baudRate, dataBits, rxPin, txPin);
    delay(500);
}

void LD2412::readData() {
    while (_serial.available()) {
        uint8_t data = _serial.read();

        // Parse the data based on the documentation
        if (data == 0x02) {  // Example: Target basic information type
            uint8_t targetState = _serial.read();  // Target state byte
            switch (targetState) {
                case 0x00:  // No target
                    presence = false;
                    break;
                case 0x01:  // Movement target
                    presence = true;
                    movingDistance = (_serial.read() << 8) | _serial.read();  // Read 2 bytes for movement distance
                    _serial.read();  // Skip the energy value
                    stationaryDistance = 0;  // No stationary target
                    break;
                case 0x02:  // Stationary target
                    presence = true;
                    _serial.read();  // Skip movement distance and energy
                    stationaryDistance = (_serial.read() << 8) | _serial.read();  // Read 2 bytes for stationary distance
                    break;
                case 0x03:  // Both movement and stationary targets
                    presence = true;
                    movingDistance = (_serial.read() << 8) | _serial.read();  // Read 2 bytes for movement distance
                    _serial.read();  // Skip movement energy
                    stationaryDistance = (_serial.read() << 8) | _serial.read();  // Read 2 bytes for stationary distance
                    break;
                default:
                    presence = false;
                    stationaryDistance = 0;
                    movingDistance = 0;
                    break;
            }
        }
    }
}

bool LD2412::presenceDetected() {
    return presence;
}

bool LD2412::stationaryTargetDetected() {
    return stationaryDistance > 0;
}

uint16_t LD2412::stationaryTargetDistance() {
    return stationaryDistance;
}

bool LD2412::movingTargetDetected() {
    return movingDistance > 0;
}

uint16_t LD2412::movingTargetDistance() {
    return movingDistance;
}

void LD2412::sendCommand(uint8_t commandWord, uint8_t* commandValue, uint8_t valueLen) {
    _serial.write(0xFD); // Frame header
    _serial.write(0xFC); 
    _serial.write(0xFB); 
    _serial.write(0xFA); 
    _serial.write(valueLen + 4); // Frame length
    _serial.write(commandWord);  // Command word
    for (int i = 0; i < valueLen; i++) {
        _serial.write(commandValue[i]); // Command values
    }
    _serial.write(0x04);  // Frame end
    _serial.write(0x03); 
    _serial.write(0x02); 
    _serial.write(0x01); 
}

void LD2412::configureSensor() {
    sendEnableConfiguration();
    // Set basic parameters (e.g., distance range, sensitivity)
    uint8_t configValues[5] = {0x01, 0x0C, 0x05, 0x00};  // Example values
    sendCommand(0x0002, configValues, 5); // Basic parameter configuration
    sendEndConfiguration();
}

void LD2412::sendEnableConfiguration() {
    uint8_t commandValue[1] = {0x0001};
    sendCommand(0x00FF, commandValue, 1); // Enable configuration
    handleAckResponse();
}

void LD2412::sendEndConfiguration() {
    uint8_t commandValue[1] = {0x00};
    sendCommand(0x00FE, commandValue, 1); // End configuration
    handleAckResponse();
}

void LD2412::handleAckResponse() {
    while (_serial.available()) {
        uint8_t data = _serial.read();
        // Handle ACK response
    }
}