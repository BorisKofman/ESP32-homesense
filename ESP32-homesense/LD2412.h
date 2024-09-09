
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
  
  private:
    HardwareSerial& _serial;
    void sendEnableConfiguration();
    void sendEndConfiguration();
    void handleAckResponse();
};

#endif // LD2412_H
