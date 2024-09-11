#ifndef LD2412_H
#define LD2412_H

#include <HardwareSerial.h>

class LD2412 {
public:
  LD2412(HardwareSerial &serial);
  void setup();
  void configureSensor();
  void readData();

  bool presenceDetected();
  bool stationaryTargetDetected();
  bool movingTargetDetected();
  uint16_t stationaryTargetDistance();  
  uint16_t movingTargetDistance();

private:
  HardwareSerial &serial;
  bool detectedTarget = false; 
  uint16_t stationaryDistance = 0;  
  uint16_t movingDistance = 0;      

  static const int bufferSize = 16; 
  bool debug = true;                 

  void handleMessage(char *buffer);  
  void handleTargetState(char targetState, char *buffer);

#endif