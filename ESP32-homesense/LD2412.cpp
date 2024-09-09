
#include "LD2412.h"

LD2412::LD2412(HardwareSerial& serial) : _serial(serial) {}

void LD2412::begin(int baudRate, int dataBits, int rxPin, int txPin) {
    _serial.begin(baudRate, dataBits, rxPin, txPin);
    delay(500);
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

void LD2412::readData() {
    while (_serial.available()) {
        uint8_t data = _serial.read();
        // Implement data handling logic based on the protocol
    }
}

void LD2412::configureSensor() {
    sendEnableConfiguration();
    // Example: Setting basic configuration
    uint8_t configValues[5] = {0x01, 0x0C, 0x05, 0x00}; // min distance, max distance, unmanned duration, polarity
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
        // Handle ACK response logic here
    }
}
