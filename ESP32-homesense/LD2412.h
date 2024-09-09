#ifndef LD2412_H
#define LD2412_H

#include <Arduino.h>

class LD2412 {
  public:
    LD2412(HardwareSerial& serial);
    void begin(int baudRate, int dataBits, int rxPin, int txPin);
    void sendCommand(uint8_t commandWord, uint8_t* commandValue, uint8_t valueLen);
    void readData();
    void configureSensor();

    bool presenceDetected();  // New method to detect presence
    bool stationaryTargetDetected();  // New method to detect stationary target
    uint16_t stationaryTargetDistance();  // New method to get stationary target distance
    bool movingTargetDetected();  // New method to detect moving target
    uint16_t movingTargetDistance();  // New method to get moving target distance
  
  private:
    HardwareSerial& _serial;
    void sendEnableConfiguration();
    void sendEndConfiguration();
    void handleAckResponse();

    // Internal variables to store radar data
    bool presence;
    uint16_t stationaryDistance;
    uint16_t movingDistance;
};

#endif // LD2412_H