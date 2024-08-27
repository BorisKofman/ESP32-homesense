#ifndef LD2450_H
#define LD2450_H

#include <Arduino.h>  // This includes basic Arduino types like uint16_t
#include <HardwareSerial.h>  // This includes the HardwareSerial class

class LD2450 {
public:
    // Constructor and Destructor
    LD2450();
    ~LD2450();

    // Initialization methods
    bool begin(HardwareSerial &serial);
    bool begin(Stream &radarStream);
    #ifdef ENABLE_SOFTWARESERIAL_SUPPORT
    bool begin(SoftwareSerial &radarStream, bool already_initialized);
    #endif

    // Radar functions
    void setNumberOfTargets(uint16_t _numTargets);
    String getLastTargetMessage();
    uint8_t read();
    uint16_t getSensorSupportedTargetCount();
    struct RadarTarget {
        bool valid;
        int id;
        int distance;
        int speed;
        int16_t x;           // X-coordinate
        int16_t y;           // Y-coordinate
        uint16_t resolution; // Resolution
    };
    RadarTarget getTarget(uint16_t index);

    // Radar detection functions
    bool presenceDetected();
    bool stationaryTargetDetected();
    bool movingTargetDetected();
    int stationaryTargetDistance();
    int movingTargetDistance();
    int movingTargetEnergy();

private:
    HardwareSerial *radarSerial;
    RadarTarget radarTargets[3];
    uint16_t numTargets = 3;
    String last_target_data;

    void parseData(byte rec_buf[], int len);
    uint8_t ProcessSerialDataIntoRadarData(byte rec_buf[], int len);
};

#endif // LD2450_H